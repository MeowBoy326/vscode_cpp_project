/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "GLog.h"


//namespace gcanvas {

    #define TRACE_TAG_BUF_SIZE 128
    #define TRACE_MSG_BUF_SIZE 512

    #define LOG_MSG_BUF_SIZE 1024


    // external system log handler
    GCanvasSystemLog gcanvasSystemLog;

    // default log level : Error
    LogLevel g_log_level = LOG_LEVEL_ERROR;

    LogLevel GetLogLevel() { return g_log_level; };

    void SetLogLevel(LogLevel logLevel) {
        if (g_log_level == logLevel) return;
#ifdef DEBUG
        LogLevel oldLevel = g_log_level;
        LOG_I("[SetLogLevel] %d=>%d", oldLevel, logLevel);
#endif
        g_log_level = logLevel;
    };


    // should rename to GCanvasTraceData
    GCanvasLog::GCanvasLog() {
        tag.resize(TRACE_TAG_BUF_SIZE, '\0');
        detail.resize(TRACE_MSG_BUF_SIZE, '\0');
    }


    // @deprecated, will delete soon
    void fillLogInfo(GCanvasLog &log, const char *tag, const char *format, ...) {
        snprintf((char*)log.tag.data(), TRACE_TAG_BUF_SIZE, "%s", tag);
        va_list va;
        va_start(va, format);
        vsnprintf((char*)log.detail.data(), TRACE_MSG_BUF_SIZE, format, va);
        va_end(va);
    }



    void FillLogInfo(GCanvasLog &log, const char *tag, const char *format, ...) {
        snprintf((char*)log.tag.data(), TRACE_TAG_BUF_SIZE, "%s", tag);
        va_list va;
        va_start(va, format);
        vsnprintf((char*)log.detail.data(), TRACE_MSG_BUF_SIZE, format, va);
        va_end(va);
    }


    void AppendErrorLogInfo(std::vector<GCanvasLog> *errVec, const char *tag, const char *format, ...)
    {
        if (errVec == nullptr) {
            return;
        }

        GCanvasLog log;

        snprintf((char*)log.tag.data(), TRACE_TAG_BUF_SIZE, "%s", tag);

        va_list va;
        va_start(va, format);
        vsnprintf((char*)log.detail.data(), TRACE_MSG_BUF_SIZE, format, va);
        va_end(va);

        errVec->push_back(log);
    }


#if defined(__ANDROID__)

    int TransLogLevel(LogLevel logLevel) {
        if (LOG_LEVEL_DEBUG == logLevel) return ANDROID_LOG_DEBUG;
        if (LOG_LEVEL_INFO == logLevel) return ANDROID_LOG_INFO;
        if (LOG_LEVEL_WARN == logLevel) return ANDROID_LOG_WARN;
        if (LOG_LEVEL_ERROR == logLevel) return ANDROID_LOG_ERROR;
        if (LOG_LEVEL_FATAL == logLevel) return ANDROID_LOG_FATAL;
        return ANDROID_LOG_DEFAULT;
    }

#endif

    void LogExt(LogLevel logLevel, const char *tag, const char *format, ...) {
        if (g_log_level > logLevel) return;

        va_list va;
        char buffer[LOG_MSG_BUF_SIZE];

        va_start(va, format);
        vsnprintf(buffer, LOG_MSG_BUF_SIZE, format, va);
        va_end(va);

#if defined(__ANDROID__)
        if (gcanvasSystemLog) {
            gcanvasSystemLog(logLevel, tag, buffer);
        } else {
            __android_log_write(TransLogLevel(logLevel), tag, buffer);
        }
#else
        if (gcanvasSystemLog) {
            gcanvasSystemLog(logLevel, tag, buffer);
        } else {
            printf("%s\n", buffer);
        }
#endif
    }



    void LogException(GCanvasHooks *hooks, std::string contextId, const char *tag, const char *format, ...)
    {
        va_list va;
        char buffer[LOG_MSG_BUF_SIZE];

        va_start(va, format);
        vsnprintf(buffer, LOG_MSG_BUF_SIZE, format, va);
        va_end(va);


#if defined(__ANDROID__)
        if (hooks && hooks->GCanvasException) {
            hooks->GCanvasException(contextId.data(), tag, buffer, hooks);
        } else {
            __android_log_write(TransLogLevel(g_log_level), tag, buffer);
        }
#else
        if (hooks && hooks->GCanvasException) {
            hooks->GCanvasException(contextId.c_str(), tag, buffer, nullptr);
        }
#endif
    }


    API_EXPORT void LogExceptionVector(GCanvasHooks *hooks, std::string contextId, std::vector<GCanvasLog> &vec)
    {
        if (vec.size()) {
            for (int i = 0; i < vec.size(); i++) {
                GCanvasLog& log = vec[i];
                LogException(hooks, contextId, log.tag.data(), log.detail.data());
            }
        }
    }
//}
