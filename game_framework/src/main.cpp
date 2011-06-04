#include  <libguile.h>
#include "simple_svc.h"
#include "io_multiplex_svc.h"
#include "select_pool.h"
#include "thread_svc.h"
#include "sid.h"
#include "log.h"

static void
inner_main (void *closure, int argc, char **argv)
{
    /* module initializations would go here */
    // SCM val = scm_current_thread();
    // scm_i_thread *thptr = SCM_I_THREAD_DATA(val);
    // printf("thread %s in guile mode.\n",
    //        (thptr->guile_mode ? "is" : "is NOT"));


    g_log::reporting_level() = L_DEBUG2;
    const int count = 3;
    // LOG(L_DEBUG, "A loop with %d iterations", count)
    // {
    //     if (L_DEBUG > g_log::reporting_level()) ;
    //     else do {
    //             g_log().log(L_DEBUG, __FILE__, __LINE__, "A loop with %d iterations", count);
    //         } while (0);
    // }
    
    // g_log().get(L_DEBUG).log("A loop with %d iterations", count);
    g_log().get_stream(L_DEBUG) << "A loop with " << count << " iterations";
    // g_log().log(L_DEBUG, __FILE__, __LINE__, "A loop with %d iteration", count);
    
    for (int i = 0; i != count; ++i)
    {
        // g_log().log(L_DEBUG1, __FILE__, __LINE__, "the counter i = %d", i);
        g_log().get_stream(L_DEBUG1) << "the counter i = " << i;
    }
    // // thread_svc svc;
    // // svc.initialize();
    // // svc.run_event_loop();
}

int main(int argc, char *argv[])
{
    // simple_svc svc;
    // svc.run_event_loop();
    
    // io_multiplex_svc svc(UNIQUE_PTR<io_pool>(new select_pool(10)));
    // svc.initialize();
    // svc.run_event_loop();

    // GC_all_interior_pointers = 0;
    // GC_INIT();

    
    scm_boot_guile(argc, argv, inner_main, 0);

    return 0;
}
