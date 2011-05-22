#ifndef _SIMPLE_SVC_H_
#define _SIMPLE_SVC_H_

#include "svc.h"

class simple_svc : public i_svc
{
public:
    virtual int initialize();

    virtual int run_event_loop();

    virtual int handle_actor(const i_svc_actor& actor, HANDLE fd);
};

#endif
