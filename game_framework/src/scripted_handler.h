#ifndef _SCRIPTED_HANDLER_H_
#define _SCRIPTED_HANDLER_H_

#include "net_event_handle.h"

class scripted_handler : public i_net_event_handler
{
public:
    explicit scripted_handler(HANDLE fd);

    ~scripted_handler();
    
    virtual HANDLE id() const;

    virtual int handle_input(HANDLE fd);

    virtual int handle_output(HANDLE fd);

private:
    HANDLE fd_;
};

#endif
