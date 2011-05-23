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


typedef int HANDLE;

#define handle_error_print(msg) \
  do { fprintf(stderr, "%s:%d ", __FILE__, __LINE__); perror(msg); } while (0)

#define handle_error(msg) \
  do { fprintf(stderr, "%s:%d ", __FILE__, __LINE__); perror(msg); exit(EXIT_FAILURE); } while (0)

typedef struct sockaddr SA;

#define LISTEN_BACKLOG 50

#define MAXLINE 1024

#endif
