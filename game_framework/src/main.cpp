#include "simple_svc.h"
#include "io_multiplex_svc.h"
#include "select_pool.h"

int main(int argc, char *argv[])
{
    // simple_svc svc;
    // svc.run_event_loop();
    io_multiplex_svc svc(UNIQUE_PTR<io_pool>(new select_pool(10)));
    
    svc.initialize();
    svc.run_event_loop();
    
    return 0;
}
