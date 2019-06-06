#include <stdio.h>
#include <mqueue.h>
#include <string.h>
#include <errno.h>

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        printf("usage: mqgetattr <name>\n");
        return -1;
    }

    mqd_t mqd = mq_open(argv[1], O_RDONLY);
    if(mqd < 0)
    {
        printf("mq_open error %s: %s\n", argv[1], strerror(errno));
        return -1;
    }

    struct mq_attr attr;
    if(mq_getattr(mqd, &attr) < 0)
    {
        printf("mq_getattr error: %s\n", strerror(errno));
        return -1;
    }

    printf("max #msgs = %ld, max #bytes/msg = %ld, #currently on queue = %ld\n",
            attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);

    return 0;
}