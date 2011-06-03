#ifndef _LOG_H_
#define _LOG_H_

#include <sstream>

enum log_level_t {
    L_ERROR,
    L_WARNING,
    L_INFO,
    L_DEBUG,
    L_DEBUG2,
    L_DEBUG3,
    L_DEBUG4,
};

#define MAX_LOG_LEVEL 7

extern "C" {
    struct log_level_entry 
    {
        log_level_t lv;
        const char* name;
    };
}

extern const struct log_level_entry log_level_table[MAX_LOG_LEVEL];


typedef std::ostringstream oss;

class log
{
public:
    log();

    virtual ~log();

    oss& get(log_level_t level);

    inline const log_level_t& get_level() const
        {
            return level_;
        }

    static const char* level_c_str(const log_level_t& level);
    
private:
    // Disable copy ctor and assigin operator.
    log(const log&);
    log& operator = (const log&);

    oss& oss_;

    log_level_t level_;
};

#endif
