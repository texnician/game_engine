#include "define.h"
#include "acceptor.h"
#include "svc.h"

acceptor::acceptor(i_svc* svc, svc_actor_t id, int port)
    : svc_(svc), listen_port_(port), listen_fd_(0), id_(id)
{}

acceptor::~acceptor()
{
    if (listen_fd_ > 0) {
        close(listen_fd_);
    }
}

int acceptor::run()
{
    int listenfd = open_listenfd();
    
    while(1) {
        struct sockaddr_in clientaddr;
        socklen_t clientlen = sizeof(clientaddr);
        
        int connfd = accept(listenfd, reinterpret_cast<SA*>(&clientaddr), &clientlen);
        if (connfd < 0) {
            handle_error("accept");
        }

        // void freeaddrinfo(struct addrinfo *res);
        // struct addrinfo hints;
        // struct addrinfo *hp, *rp;

        // memset(&hints, 0, sizeof(struct addrinfo));
        // hints.ai_family = AF_INET;    /* Allow IPv4 or IPv6 */
        // hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
        // hints.ai_flags = AI_CANONNAME;
        // hints.ai_protocol = 0;          /* Any protocol */
        // hints.ai_canonname = NULL;
        // hints.ai_addr = NULL;
        // hints.ai_next = NULL;

        // int s = 0;
        // if (s = getaddrinfo(reinterpret_cast<const char*>(&clientaddr.sin_addr.s_addr), 0,
        //                     &hints, &hp) != 0) {
        //     fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        //     exit(EXIT_FAILURE);
        // }
        // for (rp = hp; rp != 0; rp = rp->ai_next) {
        //     char *haddrp = inet_ntoa(clientaddr.sin_addr);
        //     printf("server connect to %s (%s).\n", rp->ai_canonname, haddrp);
        // }

        char *haddrp = inet_ntoa(clientaddr.sin_addr);
        printf("server connect to %s.\n", haddrp);
            
        if (svc_->handle_actor(*this, connfd) < 0) {
            close(connfd);
        }
    }

    return 0;
}

int acceptor::open_listenfd()
{
    int listenfd, optval = 1;
    
    // Create a socket descriptor.
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        handle_error("socket");
    }

    // Eliminates "Address already in use" error from bind.
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                   (const void*)&optval, sizeof(int)) < 0)
    {
        handle_error("setsockopt");
    }

    struct sockaddr_in serveraddr;
    // Clear struct
    memset(reinterpret_cast<char*>(&serveraddr), 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(static_cast<unsigned short>(listen_port_));
    if (bind(listenfd, reinterpret_cast<SA*>(&serveraddr), sizeof(serveraddr)) < 0) {
        handle_error("bind");
    }

    if (listen(listenfd, LISTEN_BACKLOG) < 0) {
        handle_error("listen");
    }

    return listenfd;
}

svc_actor_t acceptor::id() const
{
    return id_;
}

actor_type acceptor::type() const
{
    return actor_type("NET_ACCEPTOR");
}
