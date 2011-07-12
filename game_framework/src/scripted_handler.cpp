#include "define.h"
#if defined(OS_LINUX)
#include <libguile.h>
#if defined(OS_LINUX)
#include <sys/syscall.h>
#endif
#include "rio.h"
#include "scripted_handler.h"
#include "log.h"

scripted_handler::scripted_handler(GHANDLE fd)
    : fd_(fd)
{
}

scripted_handler::~scripted_handler()
{
    close(fd_);
}


GHANDLE scripted_handler::id() const
{
    return fd_;
}

static SCM catch_body(void *data)
{
    SCM val = scm_current_thread();
    scm_i_thread *thptr = SCM_I_THREAD_DATA(val);
    LOG(L_INFO, "thread %s in guile mode", (thptr->guile_mode ? "is" : "is NOT"));

    char buf[MAXLINE];
    char *s = reinterpret_cast<char*>(data);
    snprintf(buf, MAXLINE, "(mote-carlo-pi %s)", s);
    struct timeval a;
    gettimeofday(&a, 0);
    SCM ret_val = scm_eval_string(scm_from_locale_string(buf));
    LOG_INDENT(L_NOTICE, 1, "%f", scm_to_double(ret_val));
    struct timeval b;
    gettimeofday(&b, 0);
    LOG(L_DEBUG, "elapsed %f", double(b.tv_sec - a.tv_sec) +
        double(b.tv_usec - a.tv_usec) / 1000000.0);
    return ret_val;
}

static SCM catch_handler(void *data, SCM tag, SCM throw_args)
{
    char *stag = scm_to_locale_string(scm_symbol_to_string(tag));
    LOG(L_ERROR, "--- catch %s: %s", stag, (char*)data);
    free(stag);
    return SCM_BOOL_T;
}

static SCM preunwind_handler(void *data, SCM key, SCM parameters)
{
    /* Capture the stack here: */
    *(SCM *)data = scm_make_stack (SCM_BOOL_T, SCM_EOL);
    return SCM_BOOL_T;
}

int scripted_handler::handle_input(GHANDLE fd)
{
    size_t n;
    char buf[MAXLINE];
    rio_t rio;
    rio_readinitb(&rio, fd);
    
    if ((n = rio_readlineb(&rio, buf, MAXLINE)) != 0) {
        std::string dbg(buf, buf + n - strlen("\r\n"));
        dbg += "\\r\\n";
        LOG(L_INFO, "server received %ld bytes: %s", n, dbg.c_str());
        LOG_INDENT(L_DEBUG, 1, "eval %s", dbg.c_str());

        SCM captured_stack = SCM_BOOL_F;

        scm_c_catch (SCM_BOOL_T, catch_body, buf, catch_handler, buf,
                     preunwind_handler, &captured_stack);

        SCM port = scm_open_output_string ();
        if (captured_stack != SCM_BOOL_F) {
            scm_display_backtrace (captured_stack, port, SCM_BOOL_F, SCM_BOOL_F);
            SCM scm_str = scm_get_output_string (port);
            char *stack_str = scm_to_locale_string(scm_str);
            LOG(L_ERROR, "STACK:\n%s", stack_str);
            scm_close_output_port(port);
        }
        
        return 0;
    }
    else {
        return -1;
    }
}

int scripted_handler::handle_output(GHANDLE fd)
{
    return 0;
}
#endif
