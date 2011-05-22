#ifndef _SVC_ACTOR_H_
#define _SVC_ACTOR_H_

#include <string>

typedef int svc_actor_t;
typedef std::string actor_type;

class i_svc_actor
{
public:
    virtual ~i_svc_actor()
        {}
    
    virtual int run() = 0;

    virtual svc_actor_t id() const = 0;

    virtual actor_type type() const = 0;
};

#endif
