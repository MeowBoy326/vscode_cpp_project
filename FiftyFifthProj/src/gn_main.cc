// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <algorithm>
#include <string>

#include "command_line.h"
#include "utf_string_conversions.h"
#include "commands.h"
#include "err.h"
#include "location.h"
#include "standard_out.h"
#include "switches.h"
#include "build_config.h"
#include "msg_loop.h"
#include "sys_info.h"

#include "last_commit_position.h"

namespace {

std::vector<std::string> GetArgs(const base::CommandLine& cmdline) {
  base::CommandLine::StringVector in_args = cmdline.GetArgs();
#if defined(OS_WIN)
  std::vector<std::string> out_args;
  for (const auto& arg : in_args)
    out_args.push_back(base::UTF16ToUTF8(arg));
  return out_args;
#else
  return in_args;
#endif
}

}  // namespace

int main(int argc, char** argv) {
#if defined(OS_WIN)
  base::CommandLine::set_slash_is_not_a_switch();
#endif
  base::CommandLine::Init(argc, argv);

  const base::CommandLine& cmdline = *base::CommandLine::ForCurrentProcess();
  std::vector<std::string> args = GetArgs(cmdline);

  std::string command;
  if (cmdline.HasSwitch("help") || cmdline.HasSwitch("h")) {
    // Make "-h" and "--help" default to help command.
    command = commands::kHelp;
  } else if (cmdline.HasSwitch(switches::kVersion)) {
    // Make "--version" print the version and exit.
    OutputString(std::string(LAST_COMMIT_POSITION) + "\n");
    exit(0);
  } else if (args.empty()) {
    // No command, print error and exit.
    Err(Location(), "No command specified.",
        "Most commonly you want \"gn gen <out_dir>\" to make a build dir.\n"
        "Or try \"gn help\" for more commands.")
        .PrintToStdout();
    return 1;
  } else {
    command = args[0];
    args.erase(args.begin());
  }

  if (!commands::CommandSwitches::Init(cmdline))
    return 1;

  const commands::CommandInfoMap& command_map = commands::GetCommands();
  commands::CommandInfoMap::const_iterator found_command =
      command_map.find(command);

  int retval;
  if (found_command != command_map.end()) {
    MsgLoop msg_loop;
    retval = found_command->second.runner(args);
  } else {
    Err(Location(), "Command \"" + command + "\" unknown.").PrintToStdout();
    OutputString(
        "Available commands (type \"gn help <command>\" for more details):\n");
    for (const auto& cmd : commands::GetCommands())
      PrintShortHelp(cmd.second.help_short);

    retval = 1;
  }

  exit(retval);  // Don't free memory, it can be really slow!
}
