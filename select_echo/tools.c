#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

ssize_t writen(int fd, const char *buff, size_t n)
{
    size_t nleft = n;
    ssize_t nwritten;
    const char *ptr = buff;

    while (nleft > 0)
    {
        nwritten = write(fd, ptr, nleft);
        if(nwritten < 0)
        {
            if(errno == EINTR)
                nwritten = 0;
            else
                return -1;
        }

        nleft -= nwritten;
        ptr += nwritten;
    }
    
    return n;
}