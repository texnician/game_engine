#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>   // for uintptr_t
#include "define.h"
#include "assert.h"
#include "log.h"

const struct log_level_entry log_level_table[__MAX_LOG_LEVEL__] = {
    { L_FATA, "FATA", "F" },
    { L_ALERT, "ALERT", "A" },
    { L_CRIT, "CRIT", "C" },
    { L_ERROR, "ERROR", "E" },
    { L_WARNING, "WARNING", "W" },
    { L_NOTICE, "NOTICE", "N" },
    { L_INFO, "INFO", "I" },
    { L_DEBUG, "DEBUG", "D" },
};

const char* level_c_str(const log_level_t& level)
{
    ASSERT(level < __MAX_LOG_LEVEL__ && level >= L_FATA && "level > __MAX_LOG_LEVEL__");
    return log_level_table[level].name;
}

const char* level_abbrev_c_str(const log_level_t& level)
{
    ASSERT(level < __MAX_LOG_LEVEL__ && level >= L_FATA && "level > __MAX_LOG_LEVEL__");
    return log_level_table[level].abbrev;
}

#if defined(__linux__)

#include <sys/time.h>

std::string log_now_time()
{
    char buffer[100];
    time_t t;
    time(&t);
    tm r = {0};
    strftime(buffer, sizeof(buffer), "%m%d %X", localtime_r(&t, &r));
    struct timeval tv;
    gettimeofday(&tv, 0);
    char result[100] = {0};
    std::sprintf(result, "%s.%06ld", buffer, (long)tv.tv_usec); 
    return result;
}

#else

#include <windows.h>

std::string log_now_time()
{
    const int MAX_LEN = 200;
    char buffer[MAX_LEN];
    if (GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0, 
            "HH':'mm':'ss", buffer, MAX_LEN) == 0)
        return "Error in NowTime()";

    char result[100] = {0};
    static DWORD first = GetTickCount();
    std::sprintf(result, "%s.%06ld", buffer, (long)(GetTickCount() - first)); 
    return result;
}

#endif  // __linux__

std::string fmt_log(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    char buf[MAXLINE];
    memset(buf, 0, sizeof(buf));
#if defined(__linux__)
    vsnprintf(buf, sizeof(buf), fmt, ap);
#else
    vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, ap);
#endif
    va_end(ap);

    return std::string(buf);
}

file_ptr& log2file::get_file_stream()
{
    static file_ptr pf = stderr;
    return pf;
}

void log2file::set_file_stream(const file_ptr& ptr)
{
    // need mutex lock
    get_file_stream() = ptr;
}

void log2file::out_put(const std::string& msg)
{
    // need mutex lock
    file_ptr pf = get_file_stream();
    if (0 != pf) {
        fprintf(pf, "%s", msg.c_str());
        fflush(pf);
    }
}

#if defined(__linux__)
#include <sys/syscall.h>
#endif

pid_t get_tid()
{
#if defined(__linux__)
#ifndef __NR_gettid
#define __NR_gettid SYS_gettid
#endif
    static bool lacks_gettid = false;
    if (!lacks_gettid) {
        pid_t tid = syscall(__NR_gettid);
        if (tid != -1) {
            return tid;
        }
        // Technically, this variable has to be volatile, but there is a small
        // performance penalty in accessing volatile variables and there should
        // not be any serious adverse effect if a thread does not immediately see
        // the value change to "true".
        lacks_gettid = true;
    }
#endif  // __linux__

    // If gettid() could not be used, we use one of the following.
#if defined(__linux__)
    return getpid();  // Linux:  getpid returns thread ID when gettid is absent
#elif defined OS_WINDOWS || defined OS_CYGWIN
    return GetCurrentThreadId();
#else
    // If none of the techniques above worked, we use pthread_self().
    return (pid_t)(uintptr_t)pthread_self();
#endif
}
