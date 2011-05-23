#ifndef _NET_EVENT_HANDLE_H_
#define _NET_EVENT_HANDLE_H_

#include "define.h"

class i_net_event_handler
{
public:
    virtual ~i_net_event_handler()
        {}

    virtual HANDLE id() const = 0;

    virtual int handle_input(HANDLE fd) = 0;

    virtual int handle_output(HANDLE fd) = 0;
};


#endif
