#ifndef _SOCKET_H_
#define _SOCKET_H_

#include "define.h"

#if defined(OS_LINUX)
#include <sys/select.h>
#elif defined(OS_WINDOWS)
#include <winsock2.h>
#include <ws2tcpip.h>
#endif  // OS_LINUX

typedef struct sockaddr SA;

#endif  // _SOCKET_H_
