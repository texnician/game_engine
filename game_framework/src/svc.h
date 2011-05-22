#ifndef _SVC_H_
#define _SVC_H_

#include "define.h"

class i_svc_actor;

class i_svc
{
public:
    virtual ~i_svc()
        {}
    
    virtual int initialize() = 0;

    virtual int run_event_loop() = 0;

    virtual int handle_actor(const i_svc_actor& actor, HANDLE fd) = 0;

    virtual int register_handle(HANDLE fd) = 0;
};

#endif
