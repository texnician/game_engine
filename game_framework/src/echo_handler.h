#ifndef _ECHO_HANDLE_H_
#define _ECHO_HANDLE_H_

#include "net_event_handle.h"

class i_svc;

class echo_handle : public i_net_event_handler
{
public:
    echo_handle(i_svc *svc, GHANDLE fd);
    ~echo_handle();
    
    virtual GHANDLE id() const;
    
    virtual int handle_input(GHANDLE fd);

    virtual int handle_output(GHANDLE fd);

private:
    i_svc *svc_;
    GHANDLE fd_;
};

#endif
