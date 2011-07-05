#ifndef _IO_MULTIPLEX_SVC_H_
#define _IO_MULTIPLEX_SVC_H_

#include <memory>
#include "svc.h"

class io_pool;

class io_multiplex_svc : public i_svc
{
public:
    io_multiplex_svc(UNIQUE_PTR<io_pool> pool);
    ~io_multiplex_svc();
    
    virtual int initialize();

    virtual int run_event_loop();

    virtual int handle_actor(const i_svc_actor& actor, GHANDLE fd);

    virtual int register_handler(handler_ptr);

private:
    int remove_handler(GHANDLE fd);

    handler_map handlers_;
    UNIQUE_PTR<io_pool> pool_;
};

#endif
