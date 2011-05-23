#ifndef _SVC_H_
#define _SVC_H_

#include "define.h"
#include <map>
#include <memory>

class i_net_event_handler;
typedef std::shared_ptr<i_net_event_handler> handler_ptr;
typedef std::map<HANDLE, handler_ptr> handler_map;

class i_svc_actor;

class i_svc
{
public:
    virtual ~i_svc()
        {}
    
    virtual int initialize() = 0;

    virtual int run_event_loop() = 0;

    virtual int handle_actor(const i_svc_actor& actor, HANDLE fd) = 0;

    virtual int register_handler(handler_ptr) = 0;
};

#endif
