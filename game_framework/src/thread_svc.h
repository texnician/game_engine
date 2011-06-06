#ifndef _THREAD_SVC_H_
#define _THREAD_SVC_H_

#include <pthread.h>
#include "svc.h"
#include "log.h"

class thread_svc : public i_svc
{
public:
    struct thread_arg
    {
        thread_arg(int id, const handler_ptr& p)
            : thread_id(id), hdptr(p)
            {}
        ~thread_arg()
            {
                LOG(L_DEBUG, "thread arg %d dtor.", thread_id);
            }
        int thread_id;
        handler_ptr hdptr;
    };

    thread_svc();

    virtual int initialize();

    virtual int run_event_loop();

    virtual int handle_actor(const i_svc_actor& actor, HANDLE fd);

    virtual int register_handler(handler_ptr);

private:
    typedef std::map<int, pthread_t> thread_map;
    
    static void* run_handler_thread(void *argp);
    
    static void* script_vm_init(void*);

    static void* get_thread_info(void*);
    
    int thread_id_;
    thread_map thread_map_;
};

#endif
