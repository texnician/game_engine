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
    oss_ << "- " << 0;
    oss_ << " "  << level_c_str(level) << ": ";
    oss_ << std::string(level > L_DEBUG ? 0 : level - L_DEBUG, '\t');
    level_ = level;
    return oss_;
}
