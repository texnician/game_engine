#include "define.h"
#include "acceptor.h"
#include "svc.h"
#include "log.h"

acceptor::acceptor(i_svc* svc, int port)
    : svc_(svc), listen_port_(port), listen_fd_(0)
{}

acceptor::~acceptor()
{
    if (listen_fd_ > 0) {
        close(listen_fd_);
    }
}

int acceptor::initialize()
{
    listen_fd_ = open_listenfd();
    
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
    return listen_fd_;
}

actor_type acceptor::type() const
{
    return actor_type("NET_ACCEPTOR");
}

int acceptor::run()
{
    struct sockaddr_in clientaddr;
    socklen_t clientlen = sizeof(clientaddr);
        
    int connfd = accept(listen_fd_, reinterpret_cast<SA*>(&clientaddr), &clientlen);
    if (connfd < 0) {
        handle_error("accept");
    }
        
    char *haddrp = inet_ntoa(clientaddr.sin_addr);
    LOG(L_INFO, "server connect to %s.", haddrp);
            
    if (svc_->handle_actor(*this, connfd) < 0) {
        close(connfd);
    }
    return 0;
}
