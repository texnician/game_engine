#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "assert.h"
#include "log.h"

const struct log_level_entry log_level_table[MAX_LOG_LEVEL] = {
    { L_EMERG, "EMERG" },
    { L_ALERT, "ALERT" },
    { L_CRIT, "CRIT" },
    { L_ERROR, "ERROR"},
    { L_WARNING, "WARNING" },
    { L_NOTICE, "NOTICE" },
    { L_INFO, "INFO" },
    { L_DEBUG, "DEBUG" },
    { L_DEBUG1, "DEBUG1" },
    { L_DEBUG2, "DEBUG2" },
    { L_DEBUG3, "DEBUG3" },
    { L_DEBUG4, "DEBUG4" }
};

g_log::~g_log()
{
    if (level_ <= g_log::reporting_level()) {
        oss_ << std::endl;
        fprintf(stderr, "%s", oss_.str().c_str());
        fflush(stderr);
    }
}

const char* g_log::level_c_str(const log_level_t& level)
{
    ASSERT(level < MAX_LOG_LEVEL && level >= L_ERROR && "level > MAX_LOG_LEVEL");
    return log_level_table[level].name;
}

log_level_t& g_log::reporting_level()
{
    static log_level_t global_reporting_level = L_DEBUG4;
    return global_reporting_level;
}

oss& g_log::get_stream(log_level_t level, const char* file, int line)
{
    oss_ << g_log::now_time();
    oss_ << ' '  << level_c_str(level);
    if (file != 0 && line > 0) {
        oss_ << ' ' << file << ':' << line;
    }
    oss_ << ": ";
    oss_ << std::string(level > L_DEBUG ? level - L_DEBUG : 0, ' ');
    level_ = level;
    return oss_;
}

void g_log::log(log_level_t level, const char* file, int line,
                const char* fmt, ...)
{
    static const int MAXLINE = 1024;
    
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

    get_stream(level, file, line) << buf;
}

#if defined(__linux__)

#include <sys/time.h>

std::string g_log::now_time()
{
    char buffer[11];
    time_t t;
    time(&t);
    tm r = {0};
    strftime(buffer, sizeof(buffer), "%X", localtime_r(&t, &r));
    struct timeval tv;
    gettimeofday(&tv, 0);
    char result[100] = {0};
    std::sprintf(result, "%s.%03ld", buffer, (long)tv.tv_usec / 1000); 
    return result;
}

#else

#include <windows.h>

std::string g_log::now_time()
{
    const int MAX_LEN = 200;
    char buffer[MAX_LEN];
    if (GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0, 
            "HH':'mm':'ss", buffer, MAX_LEN) == 0)
        return "Error in NowTime()";

    char result[100] = {0};
    static DWORD first = GetTickCount();
    std::sprintf(result, "%s.%03ld", buffer, (long)(GetTickCount() - first) % 1000); 
    return result;
}

#endif  // __linux__
