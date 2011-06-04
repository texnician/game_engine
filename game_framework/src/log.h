#ifndef _LOG_H_
#define _LOG_H_

#include <sstream>

enum log_level_t {
    L_EMERG = 0,     // system is unusable
    L_ALERT,         // action must be taken immediately
    L_CRIT,          // critical conditions
    L_ERROR,         // error conditions
    L_WARNING,       // warning conditions
    L_NOTICE,        // normal but significant condition
    L_INFO,          // informational
    L_DEBUG,         // debug-level 0 messages
    L_DEBUG1,        // debug-level 1 messages
    L_DEBUG2,        // debug-level 2 messages
    L_DEBUG3,        // debug-level 3 messages
    L_DEBUG4,        // debug-level 4 messages
};

#define MAX_LOG_LEVEL 12

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
    explicit g_log()
        {}

    ~g_log();

    // Get log message stream.
    oss& get_stream(log_level_t level = L_INFO,
                    const char* file = 0, int line = 0);

    void log(log_level_t level, const char* file, int line,
             const char* fmt, ...);

    // Get global reporting level.
    static log_level_t& reporting_level();

    static const char* level_c_str(const log_level_t& level);

    static std::string now_time();
    
private:
    // Disable copy ctor and assigin operator.
    g_log(const g_log&);
    g_log& operator = (const g_log&);

    log_level_t level_;
    
    oss oss_;
};

#define S_LOG(level)                            \
    if (level > g_log::reporting_level());      \
    else                                        \
        g_log().get_stream(level)

#define LOG(level, ...)                             \
    if (level > g_log::reporting_level());              \
    else do {                                               \
    g_log().log(level, __FILE__, __LINE__, __VA_ARGS__);    \
    } while(0)

#endif  // _LOG_H_
