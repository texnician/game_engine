#ifndef _SCRIPTED_HANDLER_H_
#define _SCRIPTED_HANDLER_H_

#include "net_event_handle.h"

class scripted_handler : public i_net_event_handler
{
public:
    explicit scripted_handler(GHANDLE fd);

    ~scripted_handler();
    
    virtual GHANDLE id() const;

    virtual int handle_input(GHANDLE fd);

    virtual int handle_output(GHANDLE fd);

private:
    GHANDLE fd_;
};

#endif
