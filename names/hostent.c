#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
    struct hostent *hptr;

    if((hptr = gethostbyname(argv[1])) == NULL)
    {
        printf("gethostbyname error for host: %s: %s\n", argv[1], hstrerror(h_errno));
        return -1;
    }

    printf("official hostname: %s\n", hptr->h_name);

    char **pptr = hptr->h_aliases;
    for(; *pptr != NULL; pptr++)
        printf("\talias: %s\n", *pptr);

    char str[16];
    switch (hptr->h_addrtype)
    {
    case AF_INET:
        pptr = hptr->h_addr_list;
        for(; *pptr != NULL; pptr++)
            printf("\taddress: %s\n", inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
        break;
    
    default:
        printf("unknown address type: %d\n", hptr->h_addrtype);
        break;
    }

    return 0;
}