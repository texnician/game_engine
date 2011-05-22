#include "define.h"
#include "simple_svc.h"
#include "acceptor.h"

int simple_svc::initialize()
{
    return 0;
}

int simple_svc::run_event_loop()
{
    acceptor ac(this, 0, 1055);
    ac.run();
    return 0;
}

int simple_svc::handle_actor(const i_svc_actor& actor, HANDLE fd)
{
    char buf[1024];
    ssize_t n = read(fd, buf, sizeof(buf));
    if (n < 0) {
        handle_error("read");
    }
    else {
        ssize_t end = std::min(n, (ssize_t(1024 - 1)));
        buf[end] = '\0';
    }
    printf("read %ld bytes: %s\n", n, buf);
    return -1;
}
