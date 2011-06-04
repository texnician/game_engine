#ifndef _DEFINE_H_
#define _DEFINE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <assert.h>
#include <list>
#include <algorithm>
#include <memory>

#if defined(_GXX_EXPERIMENTAL_CXX0X_)
  #define _CXX0X_ 1
#else
  #undef _CXX0X_
#endif

#if defined(_CXX0X_)
  #include <unordered_map>
  #define UNORDERED_MAP std::unordered_map
#else
  #include "boost/unordered_map.hpp"
  #define UNORDERED_MAP boost::unordered_map
#endif

typedef int HANDLE;

#define handle_error_print(msg) \
  do { fprintf(stderr, "%s:%d ", __FILE__, __LINE__); perror(msg); } while (0)

#define handle_error(msg) \
  do { fprintf(stderr, "%s:%d ", __FILE__, __LINE__); perror(msg); exit(EXIT_FAILURE); } while (0)

typedef struct sockaddr SA;

#define LISTEN_BACKLOG 50

#define MAXLINE 1024

#if defined(__GXX_EXPERIMENTAL_CXX0X__)
  #define UNIQUE_PTR std::unique_ptr
  #define MOVE_PTR(p) std::move(p)
  #define SHARED_PTR std::shared_ptr
  #define WEAK_PTR std::weak_ptr
#else
  #define UNIQUE_PTR  std::auto_ptr
  #define MOVE_PTR(p) p.release()
  #include "boost/shared_ptr.hpp"
  #define SHARED_PTR boost::shared_ptr
  #define WEAK_PTR boost::weak_ptr
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
# define OS_WINDOWS
#elif defined(__CYGWIN__) || defined(__CYGWIN32__)
# define OS_CYGWIN
#elif defined(linux) || defined(__linux) || defined(__linux__)
# define OS_LINUX
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
# define OS_MACOSX
#elif defined(__FreeBSD__)
# define OS_FREEBSD
#elif defined(__NetBSD__)
# define OS_NETBSD
#elif defined(__OpenBSD__)
# define OS_OPENBSD
#else
// TODO(hamaji): Add other platforms.
#endif

#endif
