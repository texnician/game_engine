#ifndef _SID_H_
#define _SID_H_

#include "define.h"


typedef unsigned int sid_t;

class sid
{
public:
    explicit sid(const char* const str);
    
    inline sid_t get_id() const
        {
            return id_;
        }
    
    inline const char* get_str() const
        {
            return str_;
        }

private:
    static sid_t hash_name(const char* const str);
    static const char* const WILDCARD_STR;
    
    sid_t id_;
    const char* str_;
};

inline bool operator == (const sid& lhs, const sid& rhs)
{
    return lhs.get_id() == rhs.get_id();
}

inline bool operator < (const sid& lhs, const sid& rhs)
{
    bool r = lhs.get_id() < rhs.get_id();
    return r;
}

#endif
