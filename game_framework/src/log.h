#ifndef _LOG_H_
#define _LOG_H_

#include <sstream>

enum log_level_t {
    L_ERROR = 0,
    L_WARNING,
    L_INFO,
    L_DEBUG,
    L_DEBUG1,
    L_DEBUG2,
    L_DEBUG3,
    L_DEBUG4,
};

#define MAX_LOG_LEVEL 8

extern "C" {
    struct log_level_entry 
    {
        log_level_t lv;
        const char* name;
    };
}

extern const struct log_level_entry log_level_table[MAX_LOG_LEVEL];


typedef std::ostringstream oss;

class g_log
{
public:
    g_log()
        {}

    ~g_log();

    oss& get(log_level_t level = L_INFO);

    // Get global reporting level.
    static log_level_t& reporting_level();

    static const char* level_c_str(const log_level_t& level);

    static std::string now_time();
    
private:
    // Disable copy ctor and assigin operator.
    g_log(const g_log&);
    g_log& operator = (const g_log&);

    oss oss_;

    log_level_t level_;
};

#endif  // _LOG_H_
