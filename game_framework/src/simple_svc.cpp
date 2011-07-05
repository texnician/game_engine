#include "define.h"
#include "simple_svc.h"
#include "acceptor.h"
#include "echo_handler.h"

simple_svc::~simple_svc()
{
}

int simple_svc::initialize()
{
    return 0;
}

int simple_svc::run_event_loop()
{
    acceptor ac(this, 1055);
    ac.initialize();
    
    while (1) {
        ac.run();
        
        for (handler_map::iterator it = handlers_.begin();
             it != handlers_.end();)
        {
            handler_ptr p = it->second;
            if (p->handle_input(it->first) < 0) {
                handlers_.erase(it++);
            }
            else {
                ++it;
            }
        }
    }
    return 0;
}

int simple_svc::register_handler(handler_ptr p)
{
    handlers_[p->id()] = p;
    return 0;
}

int simple_svc::handle_actor(const i_svc_actor& actor, GHANDLE fd)
{
    register_handler(handler_ptr(new echo_handle(this, fd)));
    
    return 0;
}
