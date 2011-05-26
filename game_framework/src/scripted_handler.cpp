#include <libguile.h>
#include "define.h"
#include "rio.h"
#include "scripted_handler.h"

scripted_handler::scripted_handler(HANDLE fd)
    : fd_(fd)
{
}

scripted_handler::~scripted_handler()
{
    close(fd_);
}


HANDLE scripted_handler::id() const
{
    return fd_;
}

static SCM catch_body(void *data)
{
    char *s = reinterpret_cast<char*>(data);
    SCM ret_val = scm_c_eval_string(s);
    printf("%f\n", scm_to_double(ret_val));
    return ret_val;
}

static SCM catch_handler(void *data, SCM tag, SCM throw_args)
{
    char *stag = scm_to_locale_string(scm_symbol_to_string(tag));
    printf("--- catch %s: %s", stag, (char *)data);
    free(stag);
    return SCM_BOOL_T;
}

int scripted_handler::handle_input(HANDLE fd)
{
    size_t n;
    char buf[MAXLINE];
    rio_t rio;
    rio_readinitb(&rio, fd);
    
    if ((n = rio_readlineb(&rio, buf, MAXLINE)) != 0) {
        printf("server received %ld bytes: %s", n, buf);
        printf("eval %s", buf);

        scm_c_catch (SCM_BOOL_T, catch_body, buf, catch_handler, buf, 0, 0);
        
        return 0;
    }
    else {
        return -1;
    }
}

int scripted_handler::handle_output(HANDLE fd)
{
    return 0;
}
