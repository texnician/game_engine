#ifndef _SIMPLE_SVC_H_
#define _SIMPLE_SVC_H_

#include <map>
#include <memory>
#include "svc.h"

class i_net_event_handle;

class simple_svc : public i_svc
{
public:
    ~simple_svc();
    
    virtual int initialize();

    virtual int run_event_loop();

    virtual int handle_actor(const i_svc_actor& actor, HANDLE fd);

    virtual int register_handle(HANDLE fd);

private:
    typedef std::shared_ptr<i_net_event_handle> handler_ptr;
    typedef std::map<HANDLE, handler_ptr> handler_map;
    
    handler_map handlers_;
};

#endif
