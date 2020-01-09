#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SERVER_FIFO_NAME "/tmp/serv_fifo"
#define CLIENT_FIFO_NAME "/tmp/client_fifo"
#define CTRL_FIFO_NAME "/tmp/ctrl_fifo"
#define BUFFER_SIZE 20
#include <ctype.h>
#include "cJSON.h"
#define JSON_BUF 1024
char jsonBuffer[JSON_BUF];

// rcgcc cJSON.c pipe_ctrl.c  -o voice_ctrl -lm && cp voice_ctrl ~/Desktop/my_test/busybox_twsa/ && adb push voice_ctrl /data
int main(int argc ,char *argv[])
{
    int ctrl_fifo_fd;
    int res;
    cJSON *root;
    int client_fifo_fd;
    
   if(access(CLIENT_FIFO_NAME,F_OK)==-1){
        res=mkfifo(CLIENT_FIFO_NAME,0777);
        if(res!=0){
            fprintf(stderr,"Cound not create fifo %s\n",CLIENT_FIFO_NAME);
        }
    }           //如果有名管道不存在就创建一个有名管道
    ctrl_fifo_fd=open(CLIENT_FIFO_NAME,O_WRONLY);
    if(ctrl_fifo_fd == -1){
        fprintf(stderr,"Server fifo failure\n");
        exit(EXIT_FAILURE);
    }
	for(;;) {
        memset(jsonBuffer,'\0',sizeof(jsonBuffer));
        client_fifo_fd=open(CLIENT_FIFO_NAME,O_RDONLY);
        if(client_fifo_fd == -1){
            fprintf(stderr,"Ctrl fifo failure\n");
            return -1;
        } 
        res=read(client_fifo_fd,jsonBuffer,sizeof(jsonBuffer));  
        if(res == -1){
            fprintf(stderr,"Read error on client pipe\n");
            exit(EXIT_FAILURE);
        }
        else if(res >0)
        {
            printf("[client] jsonBuf: %s\n",jsonBuffer);
            close(client_fifo_fd);
        }
    }
}
