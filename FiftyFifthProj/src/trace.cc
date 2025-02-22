// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "trace.h"

#include <stddef.h>

#include <algorithm>
#include <map>
#include <mutex>
#include <sstream>
#include <vector>

#include "command_line.h"
#include "file_path.h"
#include "file_util.h"
#include "string_escape.h"
#include "logging.h"
#include "stringprintf.h"
#include "filesystem_utils.h"
#include "label.h"

namespace {

constexpr uint64_t kNanosecondsToMicroseconds = 1'000;

class TraceLog {
 public:
  TraceLog() { events_.reserve(16384); }
  // Trace items leaked intentionally.

  void Add(std::unique_ptr<TraceItem> item) {
    std::lock_guard<std::mutex> lock(lock_);
    events_.push_back(std::move(item));
  }

  // Returns a copy for threadsafety.
  std::vector<TraceItem*> events() const {
    std::vector<TraceItem*> events;
    std::lock_guard<std::mutex> lock(lock_);
    events.reserve(events_.size());
    for (const auto& e : events_)
      events.push_back(e.get());
    return events;
  }

 private:
  mutable std::mutex lock_;

  std::vector<std::unique_ptr<TraceItem>> events_;

  TraceLog(const TraceLog&) = delete;
  TraceLog& operator=(const TraceLog&) = delete;
};

TraceLog* trace_log = nullptr;

struct Coalesced {
  Coalesced() : name_ptr(nullptr), total_duration(0.0), count(0) {}

  const std::string* name_ptr;  // Pointer to a string with the name in it.
  double total_duration;
  int count;
};

bool DurationGreater(const TraceItem* a, const TraceItem* b) {
  return a->delta().raw() > b->delta().raw();
}

bool CoalescedDurationGreater(const Coalesced& a, const Coalesced& b) {
  return a.total_duration > b.total_duration;
}

void SummarizeParses(std::vector<const TraceItem*>& loads, std::ostream& out) {
  out << "File parse times: (time in ms, name)\n";

  std::sort(loads.begin(), loads.end(), &DurationGreater);
  for (auto* load : loads) {
    out << base::StringPrintf(" %8.2f  ", load->delta().InMillisecondsF());
    out << load->name() << std::endl;
  }
}

void SummarizeCoalesced(std::vector<const TraceItem*>& items,
                        std::ostream& out) {
  // Group by file name.
  std::map<std::string, Coalesced> coalesced;
  for (auto* item : items) {
    Coalesced& c = coalesced[item->name()];
    c.name_ptr = &item->name();
    c.total_duration += item->delta().InMillisecondsF();
    c.count++;
  }

  // Sort by duration.
  std::vector<Coalesced> sorted;
  for (const auto& pair : coalesced)
    sorted.push_back(pair.second);
  std::sort(sorted.begin(), sorted.end(), &CoalescedDurationGreater);

  for (const auto& cur : sorted) {
    out << base::StringPrintf(" %8.2f  %d  ", cur.total_duration, cur.count);
    out << *cur.name_ptr << std::endl;
  }
}

void SummarizeFileExecs(std::vector<const TraceItem*>& execs,
                        std::ostream& out) {
  out << "File execute times: (total time in ms, # executions, name)\n";
  SummarizeCoalesced(execs, out);
}

void SummarizeScriptExecs(std::vector<const TraceItem*>& execs,
                          std::ostream& out) {
  out << "Script execute times: (total time in ms, # executions, name)\n";
  SummarizeCoalesced(execs, out);
}

}  // namespace

TraceItem::TraceItem(Type type,
                     const std::string& name,
                     std::thread::id thread_id)
    : type_(type), name_(name), thread_id_(thread_id) {}

TraceItem::~TraceItem() = default;

ScopedTrace::ScopedTrace(TraceItem::Type t, const std::string& name)
    : done_(false) {
  if (trace_log) {
    item_ = std::make_unique<TraceItem>(t, name, std::this_thread::get_id());
    item_->set_begin(TicksNow());
  }
}

ScopedTrace::ScopedTrace(TraceItem::Type t, const Label& label) : done_(false) {
  if (trace_log) {
    item_ = std::make_unique<TraceItem>(t, label.GetUserVisibleName(false),
                                        std::this_thread::get_id());
    item_->set_begin(TicksNow());
  }
}

ScopedTrace::~ScopedTrace() {
  Done();
}

void ScopedTrace::SetToolchain(const Label& label) {
  if (item_)
    item_->set_toolchain(label.GetUserVisibleName(false));
}

void ScopedTrace::SetCommandLine(const base::CommandLine& cmdline) {
  if (item_)
    item_->set_cmdline(FilePathToUTF8(cmdline.GetArgumentsString()));
}

void ScopedTrace::Done() {
  if (!done_) {
    done_ = true;
    if (trace_log) {
      item_->set_end(TicksNow());
      AddTrace(std::move(item_));
    }
  }
}

void EnableTracing() {
  if (!trace_log)
    trace_log = new TraceLog;
}

bool TracingEnabled() {
  return !!trace_log;
}

void AddTrace(std::unique_ptr<TraceItem> item) {
  trace_log->Add(std::move(item));
}

std::string SummarizeTraces() {
  if (!trace_log)
    return std::string();

  std::vector<TraceItem*> events = trace_log->events();

  // Classify all events.
  std::vector<const TraceItem*> parses;
  std::vector<const TraceItem*> file_execs;
  std::vector<const TraceItem*> script_execs;
  std::vector<const TraceItem*> check_headers;
  int headers_checked = 0;
  for (auto* event : events) {
    switch (event->type()) {
      case TraceItem::TRACE_FILE_PARSE:
        parses.push_back(event);
        break;
      case TraceItem::TRACE_FILE_EXECUTE:
        file_execs.push_back(event);
        break;
      case TraceItem::TRACE_SCRIPT_EXECUTE:
        script_execs.push_back(event);
        break;
      case TraceItem::TRACE_CHECK_HEADERS:
        check_headers.push_back(event);
        break;
      case TraceItem::TRACE_CHECK_HEADER:
        headers_checked++;
        break;
      case TraceItem::TRACE_IMPORT_LOAD:
      case TraceItem::TRACE_IMPORT_BLOCK:
      case TraceItem::TRACE_SETUP:
      case TraceItem::TRACE_FILE_EXECUTE_TEMPLATE:
      case TraceItem::TRACE_FILE_LOAD:
      case TraceItem::TRACE_FILE_WRITE:
      case TraceItem::TRACE_FILE_WRITE_GENERATED:
      case TraceItem::TRACE_FILE_WRITE_NINJA:
      case TraceItem::TRACE_DEFINE_TARGET:
      case TraceItem::TRACE_ON_RESOLVED:
      case TraceItem::TRACE_WALK_METADATA:
        break;  // Ignore these for the summary.
    }
  }

  std::ostringstream out;
  SummarizeParses(parses, out);
  out << std::endl;
  SummarizeFileExecs(file_execs, out);
  out << std::endl;
  SummarizeScriptExecs(script_execs, out);
  out << std::endl;

  // Generally there will only be one header check, but it's theoretically
  // possible for more than one to run if more than one build is going in
  // parallel. Just report the total of all of them.
  if (!check_headers.empty()) {
    double check_headers_time = 0;
    for (auto* cur : check_headers)
      check_headers_time += cur->delta().InMillisecondsF();

    out << "Header check time: (total time in ms, files checked)\n";
    out << base::StringPrintf(" %8.2f  %d\n", check_headers_time,
                              headers_checked);
  }

  return out.str();
}

void SaveTraces(const base::FilePath& file_name) {
  std::ostringstream out;

  out << "{\"traceEvents\":[";

  std::string quote_buffer;  // Allocate outside loop to prevent reallocationg.

  // Trace viewer doesn't handle integer > 2^53 well, so re-numbering them to
  // small numbers.
  std::map<std::thread::id, int> tidmap;
  std::vector<TraceItem*> events = trace_log->events();
  for (const auto* item : events) {
    int id = tidmap.size();
    tidmap.emplace(item->thread_id(), id);
  }

  // Write main thread metadata (assume this is being written on the main
  // thread).
  out << "{\"pid\":0,\"tid\":\"" << tidmap[std::this_thread::get_id()] << "\"";
  out << ",\"ts\":0,\"ph\":\"M\",";
  out << "\"name\":\"thread_name\",\"args\":{\"name\":\"Main thread\"}},";

  for (size_t i = 0; i < events.size(); i++) {
    const TraceItem& item = *events[i];

    if (i != 0)
      out << ",";
    out << "{\"pid\":0,\"tid\":\"" << tidmap[item.thread_id()] << "\"";
    out << ",\"ts\":" << item.begin() / kNanosecondsToMicroseconds;
    out << ",\"ph\":\"X\"";  // "X" = complete event with begin & duration.
    out << ",\"dur\":" << item.delta().InMicroseconds();

    quote_buffer.resize(0);
    base::EscapeJSONString(item.name(), true, &quote_buffer);
    out << ",\"name\":" << quote_buffer;

    out << ",\"cat\":";
    switch (item.type()) {
      case TraceItem::TRACE_SETUP:
        out << "\"setup\"";
        break;
      case TraceItem::TRACE_FILE_LOAD:
        out << "\"load\"";
        break;
      case TraceItem::TRACE_FILE_PARSE:
        out << "\"parse\"";
        break;
      case TraceItem::TRACE_FILE_EXECUTE:
        out << "\"file_exec\"";
        break;
      case TraceItem::TRACE_FILE_EXECUTE_TEMPLATE:
        out << "\"file_exec_template\"";
        break;
      case TraceItem::TRACE_FILE_WRITE:
        out << "\"file_write\"";
        break;
      case TraceItem::TRACE_FILE_WRITE_GENERATED:
        out << "\"file_write_generated\"";
        break;
      case TraceItem::TRACE_FILE_WRITE_NINJA:
        out << "\"file_write_ninja\"";
        break;
      case TraceItem::TRACE_IMPORT_LOAD:
        out << "\"import_load\"";
        break;
      case TraceItem::TRACE_IMPORT_BLOCK:
        out << "\"import_block\"";
        break;
      case TraceItem::TRACE_SCRIPT_EXECUTE:
        out << "\"script_exec\"";
        break;
      case TraceItem::TRACE_DEFINE_TARGET:
        out << "\"define\"";
        break;
      case TraceItem::TRACE_ON_RESOLVED:
        out << "\"onresolved\"";
        break;
      case TraceItem::TRACE_CHECK_HEADER:
        out << "\"hdr\"";
        break;
      case TraceItem::TRACE_CHECK_HEADERS:
        out << "\"header_check\"";
        break;
      case TraceItem::TRACE_WALK_METADATA:
        out << "\"walk_metadata\"";
        break;
    }

    if (!item.toolchain().empty() || !item.cmdline().empty()) {
      out << ",\"args\":{";
      bool needs_comma = false;
      if (!item.toolchain().empty()) {
        quote_buffer.resize(0);
        base::EscapeJSONString(item.toolchain(), true, &quote_buffer);
        out << "\"toolchain\":" << quote_buffer;
        needs_comma = true;
      }
      if (!item.cmdline().empty()) {
        quote_buffer.resize(0);
        base::EscapeJSONString(item.cmdline(), true, &quote_buffer);
        if (needs_comma)
          out << ",";
        out << "\"cmdline\":" << quote_buffer;
        needs_comma = true;
      }
      out << "}";
    }
    out << "}";
  }

  out << "]}";

  std::string out_str = out.str();
  base::WriteFile(file_name, out_str.data(), static_cast<int>(out_str.size()));
}
