#include "square.h"
#include <string.h>
#include <stdlib.h>
#include <rpc/rpc.h>

int main(int argc, char **argv)
{
    CLIENT *cl;
    square_in in;
    square_out out;

    if(argc != 3)
    {
        printf("usage: client <hostname> <integer-value>\n");
        return -1;
    }

    cl =  clnt_create(argv[1], SQUARE_PROG, SQUARE_VERS, "tcp");
    in.arg1 = atol(argv[2]);
    if(squareproc_2(&in, &out, cl) != RPC_SUCCESS)
    {
        printf("%s", clnt_sperror(cl, argv[1]));
        return -1;
    }

    printf("result: %ld\n", out.res1);

    return 0;
}