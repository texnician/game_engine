#ifndef _NET_EVENT_HANDLE_H_
#define _NET_EVENT_HANDLE_H_

class i_net_event_handle
{
public:
    virtual ~i_net_event_handle()
        {}
    
    virtual int handle_input(HANDLE fd) = 0;

    virtual int handle_output(HANDLE fd) = 0;
};


#endif
