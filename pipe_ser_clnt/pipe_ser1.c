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

char *online_words[64]={
    "打开空调","关闭空调","调高温度",\
    "调低温度","调高风速","降低风速",\
    "上下扫风","关闭上下扫风","左右扫风",\
    "关闭左右扫风","打开睡眠","关闭睡眠",\
    "查一下近两日的电量","查一下本月的电量","查一下近两个小时的电量",\
    "推荐一款空调","打开灯光","关闭灯光",\
    "关闭健康","打开健康","打开换气",\
    "关闭换气","打开节能","关闭节能",\
    "打开干燥","关闭干燥"                                                                                                                                                                                                                                                        
};   

#include <ctype.h>
#include "cJSON.h"
#define JSON_BUF 1024
char jsonBuffer[JSON_BUF];

// rcgcc cJSON.c pipe_ser1.c  -o voice_online_one -lm && adb push voice_online_one /userdata
int main(int argc ,char *argv[])
{
    int server_fifo_fd;
    int res;
    cJSON *root;
    int client_fifo_fd;
    
    if(argc<2){
        fprintf(stderr,"Please input one params\n");
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
    int i=atoi(argv[1]);
    cJSON_AddItemToObject(root,"query",cJSON_CreateString(online_words[i]));
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
