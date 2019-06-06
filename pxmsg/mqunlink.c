#include <stdio.h>
#include <mqueue.h>

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        printf("usage: mqunlink <name>\n");
        return 0;
    }

    mq_unlink(argv[1]);

    return 0;
}