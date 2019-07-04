#include "square.h"
#include <string.h>
#include <stdlib.h>
#include <rpc/rpc.h>

int main(int argc, char **argv)
{
    CLIENT *cl;
    square_in in;
    square_out *outp;

    if(argc != 3)
    {
        printf("usage: client <hostname> <integer-value>\n");
        return -1;
    }

    cl =  clnt_create(argv[1], SQUARE_PROG, SQUARE_VERS, "tcp");
    in.arg1 = atol(argv[2]);
    if((outp = squareproc_1(&in, cl)) == NULL)
    {
        printf("%s", clnt_sperror(cl, argv[1]));
        return -1;
    }

    printf("result: %ld\n", outp->res1);

    return 0;
}