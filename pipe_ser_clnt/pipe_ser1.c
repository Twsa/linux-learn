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
#define BUFFER_SIZE 20
char *ctrl_word[11]={"pow","auto","cold_mode","hot_mode",\
"lesswet_mode","air_supy","windsped","temp","left_right","up_down","sleep"};

#include <ctype.h>
#include "cJSON.h"
#define JSON_BUF 64
char jsonBuffer[JSON_BUF];

int main(int argc ,char *argv[])
{
    int server_fifo_fd;
    int res;
    cJSON *root;
    int client_fifo_fd;
    
    if(argc<3){
        fprintf(stderr,"Please input two params\n");
        exit(EXIT_FAILURE);
    }
   if(access(SERVER_FIFO_NAME,F_OK)==-1){
        res=mkfifo(SERVER_FIFO_NAME,0777);
        if(res!=0){
            fprintf(stderr,"Cound not create fifo %s\n",SERVER_FIFO_NAME);
        }
    }           //如果有名管道不存在就创建一个有名管道
    if(access(CLIENT_FIFO_NAME,F_OK)==-1){
    res=mkfifo(CLIENT_FIFO_NAME,0777);
    if(res!=0){
        fprintf(stderr,"Cound not create client  fifo %s\n",CLIENT_FIFO_NAME);
    }
}//创建客户端管道
    server_fifo_fd=open(SERVER_FIFO_NAME,O_WRONLY);
    if(server_fifo_fd == -1){
        fprintf(stderr,"Server fifo failure\n");
        exit(EXIT_FAILURE);
    }
    // for(int i=0;i<5;i++)
    // {      
    root=cJSON_CreateObject();
    // if(i==11) {mode_i=0;status_i=1;}
    // cjson_test(mode_i++,status_i);
    cJSON_AddItemToObject(root,"query",cJSON_CreateString(argv[1]));
    cJSON_AddItemToObject(root,"p",cJSON_CreateString(argv[2]));
    strcpy(jsonBuffer,cJSON_PrintUnformatted(root));
    cJSON_Delete(root);
    res = write(server_fifo_fd,jsonBuffer,sizeof(jsonBuffer));  
    if(res == -1){
        fprintf(stderr,"Write error on server pipe\n");
        exit(EXIT_FAILURE);
    }
    else if(res>0)
    {
        close(server_fifo_fd);
        memset(jsonBuffer,'\0',sizeof(jsonBuffer));
        client_fifo_fd=open(CLIENT_FIFO_NAME,O_RDONLY);
        if(client_fifo_fd == -1){
            fprintf(stderr,"Client fifo failure\n");
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
    // }

    // unlink(SERVER_FIFO_NAME);
    exit(EXIT_SUCCESS);
}
