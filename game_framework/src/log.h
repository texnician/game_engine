#ifndef _LOG_H_
#define _LOG_H_

#include <sstream>
#include "atomic_pod.h"

enum log_level_t {
    L_FATA = 0,     // system is unusable
    L_ALERT,         // action must be taken immediately
    L_CRIT,          // critical conditions
    L_ERROR,         // error conditions
    L_WARNING,       // warning conditions
    L_NOTICE,        // normal but significant condition
    L_INFO,          // informational
    L_DEBUG,         // debug messages
};

#ifndef FILELOG_MAX_LEVEL
#define FILELOG_MAX_LEVEL L_DEBUG
#endif

#define __MAX_LOG_LEVEL__ 8

extern "C" {
    struct log_level_entry 
    {
        log_level_t lv;
        const char* name;
        const char* abbrev;
    };
}

extern const struct log_level_entry log_level_table[__MAX_LOG_LEVEL__];


typedef std::ostringstream oss;

template<typename OutputPolicy>
class g_log
{
public:
    explicit g_log()
        {}

    ~g_log();

    // Get log message stream.
    oss& get_stream(log_level_t level = L_INFO, int indent = 0,
                    const char* file = 0, int line = 0);
    
    // Get global reporting level.
    static atomic_pod<int>& reporting_level();
    
private:
    // Disable copy ctor and assigin operator.
    g_log(const g_log&);
    g_log& operator = (const g_log&);

    log_level_t level_;
    
    oss oss_;
};

DLL_API size_t move_string(char *buf, size_t len, std::string& src);

DLL_API const char* level_c_str(const log_level_t& level);

DLL_API const char* level_abbrev_c_str(const log_level_t& level);

#if defined(OS_WINDOWS)
DLL_API size_t safe_log_now_time(char* buf, size_t len);

DLL_API size_t safe_fmt_log(char* buf, size_t len, const char* fmt, ...);
#endif

std::string log_now_time();

std::string fmt_log(const char* fmt, ...);

std::string vfmt_log(const char* fmt, va_list vargs);

int DLL_API get_tid();

template<typename OutputPolicy>
g_log<OutputPolicy>::~g_log()
{
    oss_ << std::endl;
    OutputPolicy::out_put(oss_.str());
}

template<typename OutputPolicy>
atomic_pod<int>& g_log<OutputPolicy>::reporting_level()
{
    static atomic_pod<int> global_reporting_level(L_DEBUG);
    return global_reporting_level;
}

template<typename OutputPolicy>
oss& g_log<OutputPolicy>::get_stream(log_level_t level, int indent,
                                     const char* file, int line)
{
    oss_ << level_abbrev_c_str(level);
#if defined(OS_WINDOWS)
    char buf[64];
    safe_log_now_time(buf, 64);
    oss_ << ' ' << buf;
#else
    oss_ << ' ' << log_now_time();
#endif
    oss_ << ' ' << get_tid();
    if (file != 0 && line > 0) {
        oss_ << ' ' << file << ':' << line;
    }
    oss_ << "] ";
    while (indent > 0) {
        oss_ << "| ";
        --indent;
    }
    level_ = level;
    return oss_;
}

typedef FILE* file_ptr;

class DLL_API log2file
{
public:
    static void set_file_stream(const file_ptr& ptr);
    
    static void out_put(const std::string& msg);

private:
    static file_ptr& get_file_stream();
};

typedef g_log<log2file> file_log;

// runtime log
namespace rt_log
{
    enum rt_log_flag
    {
        NOT_INIT = -1,
        DEFAULT = 0,
        DISABLED = 1,
        ENABLED = 2
    };

    typedef atomic_pod<int> atomic_rt_flag;
    
    // Init runtime log flag at file:line
    void init(atomic_rt_flag& flag, const char* file, int line);

    // Set runtime log flag at file:line
    void set(const char* file, int line, rt_log_flag flag);

    bool log_enabled(atomic_rt_flag& aflag, log_level_t level,
                     const char* file, int line);
};

#define S_LOG_WITHLINE_INDENT_IF(level, file, line, indent, cond)   \
    if (level > FILELOG_MAX_LEVEL);                                 \
    else if (level > file_log::reporting_level());                  \
    else if (!(cond));                                              \
    else                                                            \
        file_log().get_stream(level, (indent), (file), (line))

#define S_LOG_INDENT_IF(level, indent, cond)                \
    S_LOG_WITHLINE_INDENT_IF(level, 0, 0, (indent), (cond))

#define S_LOG_IF(level, cond)                   \
    S_LOG_INDENT_IF(level, 0, (cond))

#define S_LOG_INDENT(level, indent)             \
    S_LOG_INDENT_IF(level, (indent), true)

#define S_LOG(level)                            \
    S_LOG_INDENT(level, 0)

#if defined(OS_WINDOWS)
#define LOG_INDENT_IF(level, indent, cond, ...)                         \
    do {                                                                \
        char __fmt_buf__[MAXLINE] = {0};                                \
        safe_fmt_log(__fmt_buf__, MAXLINE, __VA_ARGS__);                \
        S_LOG_WITHLINE_INDENT_IF(level, __FILE__, __LINE__, indent, (cond)) \
            << __fmt_buf__;                                             \
    } while (0)
    
#else
#define LOG_INDENT_IF(level, indent, cond, ...)                         \
    S_LOG_WITHLINE_INDENT_IF(level, __FILE__, __LINE__, indent, (cond)) << fmt_log(__VA_ARGS__)
#endif

#define LOG_IF(level, cond, ...)                    \
    LOG_INDENT_IF(level, 0, (cond), __VA_ARGS__)

#define LOG_INDENT(level, indent, ...)                  \
    LOG_INDENT_IF(level, (indent), true, __VA_ARGS__)

#define LOG(level, ...)                         \
    LOG_INDENT(level, 0, __VA_ARGS__)

#define RTS_LOG(level)                                                  \
    if (bool already_logged = false);                                   \
    else for (static rt_log::atomic_rt_flag flag(rt_log::NOT_INIT);     \
              !already_logged && rt_log::log_enabled(flag, level, __FILE__, __LINE__); \
              already_logged = true)                                    \
             S_LOG(level)

#if defined(OS_WINDOWS)
#define RT_LOG(level, ...)                                              \
    do {                                                                \
        char __fmt_buf__[MAXLINE] = {0};                                \
        safe_fmt_log(__fmt_buf__, MAXLINE, __VA_ARGS__);                \
        RTS_LOG(level) << __fmt_buf__;                                  \
    } while (0)
#else
#define RT_LOG(level, ...)                      \
    RTS_LOG(level) << fmt_log(__VA_ARGS__)
#endif

#endif  // _LOG_H_
