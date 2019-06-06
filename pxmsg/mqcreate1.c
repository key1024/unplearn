#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define MAXLINE 1024

struct mq_attr attr;

int main(int argc, char **argv)
{
    int flags = O_RDWR | O_CREAT;
    
    int c = 0;
    while((c = getopt(argc, argv, "em:z:")) != -1)
    {
        switch (c)
        {
        case 'e':
            flags |= O_EXCL;
            break;
        case 'm':
            attr.mq_maxmsg = atol(optarg);
            break;
        case 'z':
            attr.mq_msgsize = atol(optarg);
            break;
        }
    }
    if(optind != argc - 1)
    {
        printf("usage: mqcreate [ -e ] [ -m maxmsg -z msgsize ] <name>\n");
        return -1;
    }

    if((attr.mq_maxmsg != 0 && attr.mq_msgsize == 0) || 
        (attr.mq_maxmsg == 0 && attr.mq_msgsize != 0))
    {
        printf("must specify both -m maxmsg and -z msgsize\n");
        return -1;
    }

    mqd_t mqd = mq_open(argv[optind], flags, FILE_MODE, (attr.mq_maxmsg != 0)?&attr:NULL);
    if(mqd < 0)
    {
        printf("mq_open error %s: %s\n", argv[optind], strerror(errno));
        return -1;
    }

    mq_close(mqd);

    return 0;
}