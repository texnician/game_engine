#ifndef _IO_POOL_H_
#define _IO_POOL_H_

class io_pool
{
public:
    enum {
        READ_EVENT,
        WRITE_EVENT
    };
    
    struct io_pool_event
    {
        io_pool_event(int ev, GHANDLE d)
            : event(ev), fd(d)
            {}
        int event;
        GHANDLE fd;
    };

    typedef std::list<io_pool_event> io_event_list;

    virtual ~io_pool()
        {}
    
    virtual int on_add_handler(GHANDLE id) = 0;

    virtual int on_remove_handler(GHANDLE id) = 0;

    virtual io_event_list get_io_events() = 0;
};

#endif
