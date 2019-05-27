#ifndef _TOOLS_H_
#define _TOOLS_H_

#define max(a, b) ((a) > (b) ? (a) : (b))

#include <unistd.h>

ssize_t writen(int fd, const char *buff, size_t n);

#endif