#include "simple_svc.h"

int main(int argc, char *argv[])
{
    simple_svc svc;
    svc.run_event_loop();
    return 0;
}
