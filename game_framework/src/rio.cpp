#include "define.h"
#include "rio.h"

ssize_t rio_readn(int fd, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nread;
    char *bufp = static_cast<char*>(usrbuf);

    while (nleft > 0) {
        if ((nread = read(fd, bufp, nleft)) < 0) {
            if (errno == EINTR) {
                nread = 0;
            }
            else {
                handle_error_print("read");
                return -1;
            }
        }
        else if (nread == 0) {
            break;
        }
        nleft -= nread;
        bufp += nread;
    }
    return n - nleft;
}

ssize_t rio_writen(int fd, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nwritten;
    char *bufp = static_cast<char*>(usrbuf);

    while (nleft > 0) {
        if ((nwritten =  write(fd, bufp, nleft)) <= 0) {
            if (errno == EINTR) {
                nwritten = 0;
            }
            else {
                handle_error_print("write");
                return -1;
            }
        }
        nleft -= nwritten;
        bufp += nwritten;
    }
    return n;
}

void rio_readinitb(rio_t *rp, int fd)
{
    rp->rio_fd = fd;
    rp->rio_cnt = 0;
    rp->rio_bufptr = rp->rio_buf;
}

static ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n)
{
    while (rp->rio_cnt <= 0) {
        rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));
        if (rp->rio_cnt < 0) {
            if (errno != EINTR) {
                handle_error_print("rio_read");
                return -1;
            }
        }
        else if (rp->rio_cnt == 0) { // EOF
            fprintf(stderr, "Connection <%d> reset by peer.\n", rp->rio_fd);
            return 0;
        }
        else {
            rp->rio_bufptr = rp->rio_buf;
        }
    }

    size_t cnt = n;
    if (rp->rio_cnt < ssize_t(n)) {
        cnt = rp->rio_cnt;
    }
    memcpy(usrbuf, rp->rio_bufptr, cnt);
    rp->rio_bufptr += cnt;
    rp->rio_cnt -= cnt;
    return cnt;
}

ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen)
{
    ssize_t rc;
    char c, *bufp = (char *)usrbuf;

    size_t n;
    for (n = 1; n < maxlen; n++) {
        if ((rc = rio_read(rp, &c, 1)) == 1) {
            *bufp++ = c;
            if (c == '\n')
                break;
        }
        else if (rc == 0) {
            if (n == 1) {
                return 0;
            }
            else {
                break;
            }
        }
        else {
            return -1;
        }
    }
    *bufp = 0;
    return n;
}

ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nread;
    char *bufp = (char *)usrbuf;

    while (nleft > 0) {
        if ((nread = rio_read(rp, bufp, nleft)) < 0) {
            if (errno == EINTR) {
                nread = 0;
            }
            else {
                return -1;
            }
        }
        else if (nread == 0) {
            break;
        }
        nleft -= nread;
        bufp += nread;
    }
    return n - nleft;
}
