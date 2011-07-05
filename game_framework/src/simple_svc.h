#ifndef _SIMPLE_SVC_H_
#define _SIMPLE_SVC_H_

#include "svc.h"

class i_net_event_handler;

class simple_svc : public i_svc
{
public:
    ~simple_svc();
    
    virtual int initialize();

    virtual int run_event_loop();

    virtual int handle_actor(const i_svc_actor& actor, GHANDLE fd);

    virtual int register_handler(handler_ptr);

private:
    handler_map handlers_;
};

#endif
