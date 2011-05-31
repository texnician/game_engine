#include <sys/syscall.h>
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
    SCM val = scm_current_thread();
    scm_i_thread *thptr = SCM_I_THREAD_DATA(val);
    printf("thread %s in guile mode.\n",
           (thptr->guile_mode ? "is" : "is NOT"));
    
    char buf[MAXLINE];
    char *s = reinterpret_cast<char*>(data);
    snprintf(buf, MAXLINE, "(mote-carlo-pi %s)", s);
    struct timeval a;
    gettimeofday(&a, 0);
    SCM ret_val = scm_eval_string(scm_from_locale_string(buf));
    printf("%f\n", scm_to_double(ret_val));
    struct timeval b;
    gettimeofday(&b, 0);
    printf("elapsed %f\n", double(b.tv_sec - a.tv_sec) +
           double(b.tv_usec - a.tv_usec) / 1000000.0);
    return ret_val;
}

static SCM catch_handler(void *data, SCM tag, SCM throw_args)
{
    char *stag = scm_to_locale_string(scm_symbol_to_string(tag));
    pid_t tid = syscall(SYS_gettid);
    printf("%d --- catch %s: %s\n", tid, stag, (char*)data);
    free(stag);
    return SCM_BOOL_T;
}

static SCM preunwind_handler(void *data, SCM key, SCM parameters)
{
    /* Capture the stack here: */
    *(SCM *)data = scm_make_stack (SCM_BOOL_T, SCM_EOL);
    return SCM_BOOL_T;
}

int scripted_handler::handle_input(HANDLE fd)
{
    size_t n;
    char buf[MAXLINE];
    rio_t rio;
    rio_readinitb(&rio, fd);
    
    if ((n = rio_readlineb(&rio, buf, MAXLINE)) != 0) {
        pid_t tid = syscall(SYS_gettid);
        printf("[TID:%d] server received %ld bytes: %s", tid, n, buf);
        printf("[TID:%d] eval %s", tid, buf);

        SCM captured_stack = SCM_BOOL_F;

        scm_c_catch (SCM_BOOL_T, catch_body, buf, catch_handler, buf,
                     preunwind_handler, &captured_stack);

        SCM port = scm_open_output_string ();
        if (captured_stack != SCM_BOOL_F) {
            scm_display_backtrace (captured_stack, port, SCM_BOOL_F, SCM_BOOL_F);
            SCM scm_str = scm_get_output_string (port);
            char *stack_str = scm_to_locale_string(scm_str);
            printf("STACK:\n%s", stack_str);
            scm_close_output_port(port);
        }
        
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
