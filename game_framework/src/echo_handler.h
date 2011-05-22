#ifndef _ECHO_HANDLE_H_
#define _ECHO_HANDLE_H_

#include "net_event_handle.h"

class i_svc;

class echo_handle : public i_net_event_handle
{
public:
    echo_handle(i_svc *svc, HANDLE fd);
    ~echo_handle();
    
    virtual HANDLE id() const;
    
    virtual int handle_input(HANDLE fd);

    virtual int handle_output(HANDLE fd);

private:
    i_svc *svc_;
    HANDLE fd_;
};

#endif
