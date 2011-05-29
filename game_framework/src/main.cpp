#include  <libguile.h>
#include "simple_svc.h"
#include "io_multiplex_svc.h"
#include "select_pool.h"
#include "thread_svc.h"
#include "sid.h"

static void
inner_main (void *closure, int argc, char **argv)
{
    /* module initializations would go here */
    // SCM val = scm_current_thread();
    // scm_i_thread *thptr = SCM_I_THREAD_DATA(val);
    // printf("thread %s in guile mode.\n",
    //        (thptr->guile_mode ? "is" : "is NOT"));

    std::vector<sid> sid_vec;
    sid_vec.push_back(sid("*"));
    sid_vec.push_back(sid("game_logic"));
    sid_vec.push_back(sid("gateway"));
    sid_vec.push_back(sid("model"));
    sid_vec.push_back(sid("reponsitory"));
    sid_vec.push_back(sid("player_dead"));
    sid_vec.push_back(sid("level_up"));
    sid_vec.push_back(sid("router"));

    for (std::vector<sid>::iterator it = sid_vec.begin();
         it != sid_vec.end(); ++it)
    {
        printf("0x%x => %s\n", it->get_id(), it->get_str());
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
