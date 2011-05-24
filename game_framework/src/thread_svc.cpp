#include "define.h"
#include "thread_svc.h"
#include "echo_handler.h"
#include "acceptor.h"

thread_svc::thread_svc()
    : thread_id_(-1)
{}

int thread_svc::initialize()
{
    register_handler(handler_ptr(new echo_handle(this, STDIN_FILENO)));
    return 0;
}

int thread_svc::run_event_loop()
{
    acceptor ac(this, 1055);
    ac.initialize();

    while (1) {
        ac.run();
    }
    return 0;
}

int thread_svc::handle_actor(const i_svc_actor& actor, HANDLE fd)
{
    return register_handler(handler_ptr(new echo_handle(this, fd)));
}

void* thread_svc::run_handler_thread(void *argp)
{
    UNIQUE_PTR<thread_arg> unip(reinterpret_cast<thread_svc::thread_arg*>(argp));
    thread_svc::thread_arg& arg = *unip;
    handler_ptr hdptr = arg.hdptr;
    pthread_t tid = pthread_self();

    if (pthread_detach(tid) < 0) {
        handle_error("pthread_detach");
    }
    
    fprintf(stderr, "Thread %d running.\n", arg.thread_id);
    
    while (hdptr->handle_input(hdptr->id()) >= 0)
    {}
    
    fprintf(stderr, "Thread %d exit.\n", arg.thread_id);
    return 0;
}

int thread_svc::register_handler(handler_ptr p)
{
    pthread_t tid;
    if (pthread_create(&tid, 0, &thread_svc::run_handler_thread,
                       new thread_arg(++thread_id_, p)) < 0)
    {
        handle_error("pthread_create");
    }
    thread_map_[thread_id_] = tid;
    return 0;
}
