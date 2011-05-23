#include "define.h"
#include "acceptor.h"
#include "io_multiplex_svc.h"
#include "io_pool.h"
#include "echo_handler.h"

io_multiplex_svc::io_multiplex_svc(std::unique_ptr<io_pool> pool)
    : pool_(std::move(pool))
{}

io_multiplex_svc::~io_multiplex_svc()
{}

int io_multiplex_svc::initialize()
{
    register_handler(handler_ptr(new echo_handle(this, STDIN_FILENO)));
    return 0;
}

int io_multiplex_svc::run_event_loop()
{
    acceptor ac(this, 1055);
    ac.initialize();

    // add acceptor handler
    pool_->on_add_handler(ac.id());
    
    while (1) {
        std::list<HANDLE> obsolete_handlers;
        const io_pool::io_event_list events = pool_->get_io_events();
        for (io_pool::io_event_list::const_iterator iter = events.begin();
             iter != events.end(); ++iter)
        {
            if (iter->fd == ac.id()) {
                // accept new connection
                ac.run();
            }
            else if (iter->event == io_pool::READ_EVENT) {
                if (handlers_[iter->fd]->handle_input(iter->fd) < 0) {
                    obsolete_handlers.push_back(iter->fd);
                }
            }
            else if (iter->event == io_pool::WRITE_EVENT) {
                if (handlers_[iter->fd]->handle_output(iter->fd) < 0) {
                    obsolete_handlers.push_back(iter->fd);
                }
            }
            else {
                assert(false && "Invalid I/O event.");
            }
        }
        // Clear obsolete handlers.
        for (std::list<HANDLE>::const_iterator iter = obsolete_handlers.begin();
             iter != obsolete_handlers.end(); ++iter)
        {
            remove_handler(*iter);
        }
    }
    return 0;
}

int io_multiplex_svc::remove_handler(HANDLE fd)
{
    handler_map::iterator it = handlers_.find(fd);
    if (it != handlers_.end()) {
        pool_->on_remove_handler(fd);
        handlers_.erase(it);
        return 0;
    }
    else {
        return -1;
    }
}

int io_multiplex_svc::handle_actor(const i_svc_actor& actor, HANDLE fd)
{
    return register_handler(handler_ptr(new echo_handle(this, fd)));
}

int io_multiplex_svc::register_handler(handler_ptr p)
{
    handlers_[p->id()] = p;
    pool_->on_add_handler(p->id());
    
    return 0;
}
