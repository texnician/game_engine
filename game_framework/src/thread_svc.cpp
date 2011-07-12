#include "define.h"
#if defined(OS_LINUX)
#include <libguile.h>
#include "thread_svc.h"
#include "echo_handler.h"
#include "acceptor.h"
#include "scripted_handler.h"
#include <random>

namespace {
const unsigned long MAX_RANGE = 100000;
std::mt19937 eng;
std::uniform_int<> dist(0, MAX_RANGE);
std::variate_generator<std::mt19937&, std::uniform_int<> > roll(eng, dist);
}

static double random_in_range(double low, double high)
{
    double range(high - low);
    double rand = double(roll());
    return  double(low) + range * rand / double(MAX_RANGE);
}

static SCM c_random_in_range(SCM low, SCM high)
{
    double result = random_in_range(scm_to_double(low), scm_to_double(high));
    return scm_from_double(result);
}

thread_svc::thread_svc()
    : thread_id_(-1)
{
}

void* thread_svc::script_vm_init(void* argp)
{
    LOG(L_INFO, "Init script VM(guile)...");
    LOG(L_DEBUG, "1. define all SMOB types");
    LOG(L_DEBUG, "2. export primitives to Scheme using scm_c_define_gsubr");
    
    // thread_svc::run_handler_thread(argp);
    scm_with_guile(&thread_svc::run_handler_thread, argp);
    
    return 0;
}

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

int thread_svc::handle_actor(const i_svc_actor& actor, GHANDLE fd)
{
    return register_handler(handler_ptr(new scripted_handler(fd)));
}

static SCM test(SCM in)
{
    return scm_from_int(scm_to_int(in));
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

    LOG(L_INFO, "Thread %d running", arg.thread_id);
    
    SCM val = scm_current_thread();
    scm_i_thread *thptr = SCM_I_THREAD_DATA(val);
    LOG(L_INFO, "thread %d %s in guile mode.", arg.thread_id,
        (thptr->guile_mode ? "is" : "is NOT"));

    scm_c_define_gsubr("test", 1, 0, 0, (void*)&test);
    scm_c_define_gsubr("c-random-in-range", 2, 0, 0, (void*)&c_random_in_range);
    scm_c_primitive_load("init.scm");
    
    while (hdptr->handle_input(hdptr->id()) >= 0)
    {}

    // put this info in  guard object's dtor
    LOG(L_INFO, "Thread %d exit.", arg.thread_id);
    return 0;
}

int thread_svc::register_handler(handler_ptr p)
{
    // // SCM val = scm_current_thread();
    // // scm_i_thread *thptr = SCM_I_THREAD_DATA(val);
    // // printf("thread %s in guile mode.\n",
    // //        (thptr->guile_mode ? "is" : "is NOT"));
    
    pthread_t tid;
    if (pthread_create(&tid, 0, &thread_svc::script_vm_init,
                       new thread_arg(++thread_id_, p)) < 0)
    {
        handle_error("pthread_create");
    }
    thread_map_[thread_id_] = tid;
    return 0;
}
#endif
