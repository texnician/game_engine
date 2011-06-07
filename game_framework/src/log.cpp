#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>   // for uintptr_t
#include <map>
#include "define.h"
#include "assert.h"
#include "log.h"
#include "mutex.h"

const struct log_level_entry log_level_table[__MAX_LOG_LEVEL__] = {
    { L_FATA, "FATA", "F" },
    { L_ALERT, "ALERT", "A" },
    { L_CRIT, "CRIT", "C" },
    { L_ERROR, "ERROR", "E" },
    { L_WARNING, "WARNING", "W" },
    { L_NOTICE, "NOTICE", "N" },
    { L_INFO, "INFO", "I" },
    { L_DEBUG, "DEBUG", "D" },
};

const char* level_c_str(const log_level_t& level)
{
    ASSERT(level < __MAX_LOG_LEVEL__ && level >= L_FATA && "level > __MAX_LOG_LEVEL__");
    return log_level_table[level].name;
}

const char* level_abbrev_c_str(const log_level_t& level)
{
    ASSERT(level < __MAX_LOG_LEVEL__ && level >= L_FATA && "level > __MAX_LOG_LEVEL__");
    return log_level_table[level].abbrev;
}

#if defined(__linux__)

#include <sys/time.h>

std::string log_now_time()
{
    char buffer[100];
    time_t t;
    time(&t);
    tm r = {0};
    strftime(buffer, sizeof(buffer), "%m%d %X", localtime_r(&t, &r));
    struct timeval tv;
    gettimeofday(&tv, 0);
    char result[100] = {0};
    std::sprintf(result, "%s.%06ld", buffer, (long)tv.tv_usec); 
    return result;
}

#else

#include <windows.h>

std::string log_now_time()
{
    const int MAX_LEN = 200;
    char buffer[MAX_LEN];
    if (GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0, 
            "HH':'mm':'ss", buffer, MAX_LEN) == 0)
        return "Error in NowTime()";

    char result[100] = {0};
    static DWORD first = GetTickCount();
    std::sprintf(result, "%s.%06ld", buffer, (long)(GetTickCount() - first)); 
    return result;
}

#endif  // __linux__

std::string fmt_log(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    char buf[MAXLINE];
    memset(buf, 0, sizeof(buf));
#if defined(__linux__)
    vsnprintf(buf, sizeof(buf), fmt, ap);
#else
    vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, ap);
#endif
    va_end(ap);

    return std::string(buf);
}

file_ptr& log2file::get_file_stream()
{
    static file_ptr pf = stderr;
    return pf;
}

void log2file::set_file_stream(const file_ptr& ptr)
{
    // need mutex lock
    get_file_stream() = ptr;
}

void log2file::out_put(const std::string& msg)
{
    // need mutex lock
    file_ptr pf = get_file_stream();
    if (0 != pf) {
        fprintf(pf, "%s", msg.c_str());
        fflush(pf);
    }
}

#if defined(__linux__)
#include <sys/syscall.h>
#endif

pid_t get_tid()
{
#if defined(__linux__)
#ifndef __NR_gettid
#define __NR_gettid SYS_gettid
#endif
    static bool lacks_gettid = false;
    if (!lacks_gettid) {
        pid_t tid = syscall(__NR_gettid);
        if (tid != -1) {
            return tid;
        }
        // Technically, this variable has to be volatile, but there is a small
        // performance penalty in accessing volatile variables and there should
        // not be any serious adverse effect if a thread does not immediately see
        // the value change to "true".
        lacks_gettid = true;
    }
#endif  // __linux__

    // If gettid() could not be used, we use one of the following.
#if defined(__linux__)
    return getpid();  // Linux:  getpid returns thread ID when gettid is absent
#elif defined OS_WINDOWS || defined OS_CYGWIN
    return GetCurrentThreadId();
#else
    // If none of the techniques above worked, we use pthread_self().
    return (pid_t)(uintptr_t)pthread_self();
#endif
}

namespace rt_log
{
    typedef std::string file_key;
    typedef int line_key;
    typedef std::list<atomic_rt_flag*> same_pos_flag_list;
    typedef std::pair<same_pos_flag_list, rt_log_flag> rt_log_value;
    typedef std::pair<file_key, line_key> rt_log_key;
    typedef std::map<rt_log_key, rt_log_value> rt_log_map;

    static rt_log_map& get_rt_log_map()
    {
        static rt_log_map m;
        return m;
    }

    static g_mutex& get_mutex()
    {
        static g_mutex m;
        return m;
    }
    
    static std::string rtflag2str(rt_log_flag flag)
    {
        switch (flag)
        {
        case ENABLED:
            return "ENABLED";
            break;
        case DISABLED:
            return "DISABLED";
            break;
        case DEFAULT:
            return "DEFAULT";
            break;
        case NOT_INIT:
            return "NOT_INIT";
            break;
        default:
            ASSERT(false && "Unknown rt_log_flag");
            return "";
        };
    }

    void init(atomic_rt_flag& aflag, const char* file, int line)
    {
        g_scoped_lock l(get_mutex());
        rt_log_key key = std::make_pair(std::string(file), line);
        rt_log_map::iterator it = get_rt_log_map().find(key);
        if (it != get_rt_log_map().end()) {
            // add addr to list
            rt_log_flag &cur_val = it->second.second;
            rt_log_flag prev = rt_log_flag(aflag.load());
            int tmp = NOT_INIT;
            ASSERT(cur_val != NOT_INIT);
            if (aflag.compare_exchange(tmp, cur_val)) {
                same_pos_flag_list &flist = it->second.first;
                flist.push_back(&aflag);
                LOG(L_DEBUG, "Initialized runtime log at %s:%d @%#lx %s -> %s (preset value)",
                    file, line, (uintptr_t)(&aflag), rtflag2str(prev).c_str(),
                    rtflag2str(rt_log_flag(aflag.load())).c_str());
            }
            if (prev == cur_val) {
                LOG(L_DEBUG,
                    "Runtime log at %s:%d @%#lx alreay initialized: %s -> %s (not reset)",
                    file, line, (uintptr_t)(&aflag), rtflag2str(prev).c_str(),
                    rtflag2str(rt_log_flag(aflag.load())).c_str());
            }
        }
        else {
            rt_log_flag prev = rt_log_flag(aflag.load());
            ASSERT(prev == NOT_INIT);
            int tmp = NOT_INIT;
            aflag.compare_exchange(tmp, DEFAULT);
            
            same_pos_flag_list new_list;
            new_list.push_back(&aflag);
            get_rt_log_map()[key] = std::make_pair(new_list, DEFAULT);
            
            LOG(L_DEBUG, "Initialized runtime log at %s:%d @%#lx %s -> %s (first time)",
                file, line, (uintptr_t)(&aflag), rtflag2str(prev).c_str(),
                rtflag2str(rt_log_flag(aflag.load())).c_str());
        }
    }

    static void try_set(atomic_rt_flag& flag, rt_log_flag from, rt_log_flag to)
    {
        // not thread-safe. MUST be called inside `set'.
        if (from != to) {
            rt_log_flag tmp = rt_log_flag(flag.load());
            if (tmp != from) {
                LOG_INDENT(L_ERROR, 2, "%s != %s", rtflag2str(tmp).c_str(),
                           rtflag2str(from).c_str());
                ASSERT(false);
            }
            flag.exchange(atomic_rt_flag::value_type(to));
            LOG_INDENT(L_DEBUG, 2, "Set runtime log @%#lx %s -> %s",
                       (uintptr_t)(&flag),
                       rtflag2str(from).c_str(),
                       rtflag2str(rt_log_flag(flag.load())).c_str());
        }
        else {
            LOG_INDENT(L_ERROR, 2, "Can't reset runtime log @%#lx to same value %s",
                       (uintptr_t)(&flag), rtflag2str(to).c_str());
        }
    }
    
    // Set runtime log flag at file:line
    void set(const char* file, int line, rt_log_flag flag)
    {
        g_scoped_lock lk(get_mutex());
        if (flag != NOT_INIT) {
            rt_log_key key = std::make_pair(std::string(file), line);
            rt_log_map::iterator it = get_rt_log_map().find(key);
            if (it != get_rt_log_map().end()) {
                rt_log_flag& cur_val = it->second.second;
                LOG(L_DEBUG, "Changing runtime log at %s:%d %s -> %s",
                    file, line, rtflag2str(cur_val).c_str(),
                    rtflag2str(flag).c_str());
                rt_log_flag prev_val = cur_val;
                cur_val = flag;
                same_pos_flag_list& alist = it->second.first;
                for (same_pos_flag_list::iterator it = alist.begin();
                     it != alist.end(); ++it) {
                    try_set(**it, prev_val, flag);
                }
            }
            else {
                get_rt_log_map()[key] = std::make_pair(same_pos_flag_list(),
                                                       flag);
                LOG(L_DEBUG, "Save runtime log flag at %s:%d  %s (empty list)",
                    file, line, rtflag2str(flag).c_str());
            }
        }
        else {
            LOG(L_ERROR, "Can't set runtime log to %s state",
                rtflag2str(flag).c_str());
        }
    }
    
    bool log_enabled(atomic_rt_flag& aflag, log_level_t level,
                     const char* file, int line)
    {
        rt_log_flag cur_val = rt_log_flag(aflag.load());
        if (cur_val == NOT_INIT) {
            init(aflag, file, line);
            cur_val = rt_log_flag(aflag.load());
        }
        ASSERT(cur_val != NOT_INIT);
        return (cur_val == DEFAULT && file_log::reporting_level() >= level)
            || cur_val == ENABLED;
    }
}
