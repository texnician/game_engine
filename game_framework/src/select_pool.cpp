#include "define.h"
#include "select_pool.h"

select_pool::select_pool(size_t max_client)
    : maxfd_(-1), maxi_(-1), fd_vec_(max_client, -1)
{
    FD_ZERO(&read_set_);
}

int select_pool::on_add_handler(HANDLE id)
{
    fd_vec::iterator it = std::find(fd_vec_.begin(), fd_vec_.end(), -1);
    if (it != fd_vec_.end()) {
        *it = id;
        maxfd_ = std::max(id, maxfd_);
        maxi_ = std::max(int(std::distance(fd_vec_.begin(), it)), maxi_);
        FD_SET(id, &read_set_);
        return 0;
    }
    else {
        assert(false && "fd overflow");
        return -1;
    }
}

int select_pool::on_remove_handler(HANDLE id)
{
    fd_vec::iterator it = std::find(fd_vec_.begin(),
                                    fd_vec_.begin()+maxi_+1,
                                    id);
    assert(it != fd_vec_.end() && "can't find handler");
    *it = -1;
    FD_CLR(id, &read_set_);
    if (std::distance(fd_vec_.begin(), it) == maxi_) {
        --maxi_;
    }
    return 0;
}

select_pool::io_event_list select_pool::get_io_events()
{
    fd_set ready_set = read_set_;
    int nready = select(maxfd_+1, &ready_set, 0, 0, 0);
    if (nready < 0) {
        handle_error("select");
    }

    io_event_list events;
    for (fd_vec::iterator it = fd_vec_.begin();
         it != fd_vec_.begin() + maxi_ + 1 && nready > 0; ++it)
    {
        if (FD_ISSET(*it, &ready_set)) {
            events.push_back(io_pool_event(READ_EVENT, *it));
            --nready;
        }
    }
    return events;
}
