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
    
}

const char* log::level_c_str(const log_level_t& level)
{
    ASSERT(level < MAX_LOG_LEVEL && level >= L_ERROR && "level > MAX_LOG_LEVEL");
    return log_level_table[level].name;
}

oss& log::get(log_level_t level)
{
    oss_ << "- " << 0;
    oss_ << " "  << level_c_str(level) << ": ";
    oss_ << std::string(level > L_DEBUG ? 0 : level - L_DEBUG, '\t');
    level_ = level;
    return oss_;
}

    // static const log_level_t& get_level();

    // static void set_level(log_level_t&);
