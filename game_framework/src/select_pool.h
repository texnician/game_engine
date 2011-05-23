#ifndef _SELECT_POOL_H_
#define _SELECT_POOL_H_

#include <vector>
#include <sys/select.h>
#include "io_pool.h"

class select_pool : public io_pool
{
public:
    explicit select_pool(size_t max_client = 1024);
    
    virtual int on_add_handler(HANDLE id);

    virtual int on_remove_handler(HANDLE id);

    virtual io_event_list get_io_events();

private:
    typedef std::vector<int> fd_vec;
    int maxfd_;
    fd_set read_set_;
    int maxi_;
    fd_vec fd_vec_;
};

#endif
