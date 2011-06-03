#include <stdio.h>
#include "assert.h"
#include "log.h"

const struct log_level_entry log_level_table[MAX_LOG_LEVEL] = {
    { L_ERROR, "L_ERROR"},
    { L_WARNING, "L_WARNING" },
    { L_INFO, "L_INFO" },
    { L_DEBUG, "L_DEBUG" },
    { L_DEBUG2, "L_DEBUG2" },
    { L_DEBUG3, "L_DEBUG3" },
    { L_DEBUG4, "L_DEBUG4" }
};

log::~log()
{
    if (level_ >= log::reporting_level()) {
        oss_ << std::endl;
        fprintf(stderr, "%s", oss_.str().c_str());
        fflush(stderr);
    }
}

const char* log::level_c_str(const log_level_t& level)
{
    ASSERT(level < MAX_LOG_LEVEL && level >= L_ERROR && "level > MAX_LOG_LEVEL");
    return log_level_table[level].name;
}

log_level_t& log::reporting_level()
{
    static log_level_t global_reporting_level = L_DEBUG4;
    return global_reporting_level;
}

oss& log::get(log_level_t level)
{
    oss_ << "- " << log::now_time();
    oss_ << " "  << level_c_str(level) << ": ";
    oss_ << std::string(level > L_DEBUG ? 0 : level - L_DEBUG, '\t');
    level_ = level;
    return oss_;
}

#if defined(__linux__)

#include <sys/time.h>

std::string log::now_time()
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

std::string log::now_time()
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
