#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main()
{
    int data_processed;
    int file_pipes[2];
    const char some_data[]="123";
    char buffer[BUFSIZ+1];
    pid_t fork_result;

    memset(buffer,'\0',sizeof(buffer));
    if(pipe(file_pipes)==）   //创建管道
    {

    }
}