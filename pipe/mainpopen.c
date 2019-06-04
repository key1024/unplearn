#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    char buff[1024];
    if(fgets(buff, sizeof(buff), stdin) == NULL)
    {
        printf("fgets error\n");
        return -1;
    }

    int n = strlen(buff);
    if(buff[n-1] == '\n')
        n--;

    char command[1024];
    snprintf(command, sizeof(command), "cat %s", buff);

    FILE *fp = popen(command, "r");

    while (fgets(buff, 1024, fp) != NULL)
    {
        fputs(buff, stdout);
    }
    pclose(fp);

    return 0;
}