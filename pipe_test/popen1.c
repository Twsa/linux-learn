#include "unistd.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 1024
int main()
{
    FILE *read_fp;
    char buffer[BUFFER_SIZE+1];

    int chars_read;
    memset(buffer,'\0',sizeof(buffer));
    read_fp=popen("uname -a","r");
    if (read_fp!=NULL){ 
        chars_read=fread(buffer,sizeof(char),BUFFER_SIZE,read_fp);
        if(chars_read>0)
            printf("Output was:-\n%s\n",buffer);
        pclose(read_fp);
        exit(EXIT_SUCCESS);
    }
    exit(EXIT_FAILURE);
}