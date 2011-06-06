#include "echo_handler.h"
#include "rio.h"
#include "log.h"

echo_handle::echo_handle(i_svc *svc, HANDLE fd)
    : svc_(svc), fd_(fd)
{}

echo_handle::~echo_handle()
{
    close(fd_);
}

HANDLE echo_handle::id() const
{
    return fd_;
}

int echo_handle::handle_input(HANDLE fd)
{
    size_t n;
    char buf[MAXLINE];
    rio_t rio;
    rio_readinitb(&rio, fd);
    
    if ((n = rio_readlineb(&rio, buf, MAXLINE)) != 0) {
        LOG(L_INFO, "server received %ld bytes: %s", n, buf);
        return 0;
    }
    else {
        return -1;
    }
}

int echo_handle::handle_output(HANDLE fd)
{
    return 0;
}
