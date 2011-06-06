#ifndef _MUTEX_H_
#define _MUTEX_H_

// With cxx0x support
#if defined(_CXX0X_)
#include <mutex>

typedef std::mutex g_mutex;
typedef std::lock_guard<g_mutex> g_scoped_lock;

#else
#include <boost/thread/mutex.hpp>

typedef boost::mutex g_mutex;
typedef boost::mutex::scoped_lock g_scoped_lock;

#endif  // _CXX0X_

#endif
