#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

#include "svc_actor.h"

class i_svc;

class acceptor : public i_svc_actor
{
public:
    acceptor(i_svc* svc, int port);
    ~acceptor();
    
    virtual int initialize();

    virtual svc_actor_t id() const;

    virtual actor_type type() const;

    virtual int run();

private:
    int open_listenfd();

    i_svc* svc_;
    int listen_port_;
    int listen_fd_;
    svc_actor_t id_;
};

#endif
