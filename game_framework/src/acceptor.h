#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

#include "svc_actor.h"

class i_svc;

class acceptor : public i_svc_actor
{
public:
    acceptor(i_svc* svc, svc_actor_t id, int port);
    ~acceptor();
    
    virtual int run();

    virtual svc_actor_t id() const;

    virtual actor_type type() const;

private:
    int open_listenfd();

    i_svc* svc_;
    int listen_port_;
    int listen_fd_;
    svc_actor_t id_;
};

#endif
