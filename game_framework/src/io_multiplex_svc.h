#ifndef _IO_MULTIPLEX_SVC_H_
#define _IO_MULTIPLEX_SVC_H_

#include <memory>
#include "svc.h"

class io_pool;

class io_multiplex_svc : public i_svc
{
public:
    io_multiplex_svc(std::unique_ptr<io_pool> pool);
    ~io_multiplex_svc();
    
    virtual int initialize();

    virtual int run_event_loop();

    virtual int handle_actor(const i_svc_actor& actor, HANDLE fd);

    virtual int register_handler(handler_ptr);

private:
    int remove_handler(HANDLE fd);

    handler_map handlers_;
    std::unique_ptr<io_pool> pool_;
};

#endif
