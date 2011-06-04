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
    file_log::reporting_level() = L_DEBUG;

    for (int i = 0; i != __MAX_LOG_LEVEL__; ++i)
    {
        LOG_INDENT_IF(log_level_t(i), i, i < __MAX_LOG_LEVEL__, "the counter %d", i);
    }
    
    thread_svc svc;
    svc.initialize();
    svc.run_event_loop();
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
