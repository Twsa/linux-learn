#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <ctype.h>

#define SERVER_FIFO_NAME "/tmp/serv_fifo"
#define CLIENT_FIFO_NAME "/tmp/client_fifo"
#define CNTL_WORD_NUMS 7
#define BUFFER_SIZE_PIPE 20
#define JSON_BUF 64

#include "../include/gree_fifo_client.h"
// #include "../include/gree_indoor.h"
#include "../include/cJSON.h"
#include "../include/label.h"


// twsa@a:~/Desktop/my_test/pipe_ser_clnt$ gcc pipe_clnt.c cJSON.c -o clnt  -lm 
static int ctl_word_handle(const __uint8_t *const word,const __uint8_t status);
static int client_send_ok(const __uint8_t *const client_buf,const size_t len,const __uint8_t stat);
// static int wifi_2_red_pipe(const __uint8_t handle_handle,const __uint8_t status)   ;
__uint8_t *ctrl_word[11]={"pow","mode","windsped","temp",\
"left_right","up_down","sleep"};
extern char redF0[34];
extern unsigned short int ARRBUF[ARRBUF_SUM];


void gree_fifo_client(void)
{
/*#if INFRARED_PRINTF
    cout<<"[gree_fifo_client]___func___"<<__func__<<endl;
#endif */
    int server_fifo_fd;
    int res;
    __uint8_t status;
    __uint8_t cntl_word[BUFFER_SIZE_PIPE];
    __uint8_t jsonBuffer[JSON_BUF];
    cJSON *root;

    if(access(SERVER_FIFO_NAME,F_OK)==-1){
        res=mkfifo(SERVER_FIFO_NAME,0777);
        if(res!=0){
            fprintf(stderr,"Cound not create fifo %s\n",SERVER_FIFO_NAME);
        }
    }//判断服务器管道是否存在，如果不存在就创建
    server_fifo_fd=open(SERVER_FIFO_NAME,O_RDONLY);
        if(server_fifo_fd == -1){
            fprintf(stderr,"Sorry.no server\n");
            exit(EXIT_FAILURE);
        }  //打开服务器管道文件
    if(access(CLIENT_FIFO_NAME,F_OK)==-1){
        res=mkfifo(CLIENT_FIFO_NAME,0777);
        if(res!=0){
            fprintf(stderr,"Cound not create client  fifo %s\n",CLIENT_FIFO_NAME);
        }
    }//判断客户端管道是否存在，如果存在就创建
    // printf("Reading pipe\n");                        //只有添加这个，才会显示第一条接受的信息？
    while(1)
    {
        memset(jsonBuffer,'\0',sizeof(jsonBuffer));     //每次读数据，清空缓冲区
        res=read(server_fifo_fd,jsonBuffer,sizeof(jsonBuffer));
        if(res == -1)
        {
            fprintf(stderr,"Read error on server pipe\n");
            exit(EXIT_FAILURE);
        }
        else if (res>0)                                  //接受到有效数据                             
        {
            root=cJSON_Parse(jsonBuffer);
            strcpy(cntl_word,cJSON_GetObjectItem(root,"t")->valuestring);
            status=cJSON_GetObjectItem(root,"p")->valueint;
            cJSON_Delete(root);
            printf("[server] jsonBUffer: %s\n",jsonBuffer);
            // printf("type: %s,",cntl_word);
            // printf("opt: %s\n",status); 
            ctl_word_handle(cntl_word,status);
        }
    }
    close(server_fifo_fd);
    // unlink(SERVER_FIFO_NAME);
    exit(EXIT_SUCCESS);
}

/*
* Func Name   : ctl_word_handle
* Author      : liuchuanjia
* Date        : 2019/11/01
* Description : 根据接受到语音的控制包的json的解析出来的字段，进行处理的函数
* Param       : const char *const word，控制字段的名字；const int status,返回的状态
* Return      : -1,没有找到所接受的字段；0,处理成功
* Version     : v1.0
*/
static int ctl_word_handle(const __uint8_t *const word,const __uint8_t status)
{
    __uint8_t buffer[BUFFER_SIZE_PIPE]={0};
    memcpy(buffer,word,BUFFER_SIZE_PIPE);
    __uint8_t handleIndex=0;
    for(int i=0;i<CNTL_WORD_NUMS;i++)
    {
        if(!strcmp(buffer,ctrl_word[i])){
            handleIndex=i;
            printf("found it!\n");
            break;
        }
        else{
            if(i!=CNTL_WORD_NUMS-1) continue;
            else{
                printf("cannot found control word <%s>\n ",buffer);
                return -1;
            }
        }
    }       //查找字段的名字

    __uint8_t wind,tmp;

    switch (handleIndex)
    {
    case 0:
            redF0[0] = status<<3;
            redF0[16] = (redF0[0]+redF0[2]+redF0[4]+redF0[6]+redF0[8]+redF0[10]+redF0[12]+redF0[14]+0x08)&0x0f;//校验位
            //redF0[33] = (redF0[17]+redF0[19]+redF0[21]+redF0[23]+redF0[25]+redF0[27]+redF0[29]+redF0[31]+0x08)&0x0f;
            break;

    case 1:
            redF0[0] = (redF0[0] & 0x08)|(status & 0x07);
            redF0[16] = (redF0[0]+redF0[2]+redF0[4]+redF0[6]+redF0[8]+redF0[10]+redF0[12]+redF0[14]+0x08)&0x0f;//校验位
            break;
    case 2:
        wind = redF0[31] & 0X07;
        if(wind < 4 && status == 1)
        {
            redF0[1] ++;
            redF0[31] ++;
        }
        else if(wind >= 4&& wind < 8 && status == 1)
        {
            redF0[31] ++; 
        }else if(wind <= 4 && status == 0)
        {
            redF0[1] --;
            redF0[31] --;
        }
        else if(wind > 4&& wind < 8 && status == 1)
        {
            redF0[31] --; 
        }

        redF0[16] = (redF0[0]+redF0[2]+redF0[4]+redF0[6]+redF0[8]+redF0[10]+redF0[12]+redF0[14]+0x08)&0x0f;//校验位
        redF0[33] = (redF0[17]+redF0[19]+redF0[21]+redF0[23]+redF0[25]+redF0[27]+redF0[29]+redF0[31]+0x08)&0x0f;
        break;
    case 3:
    if(status == 1)
        redF0[2] ++;
    else if(status == 0)
        redF0[2] --;

        tmp = redF0[2];
        redF0[16] = (redF0[0]+redF0[2]+redF0[4]+redF0[6]+redF0[8]+redF0[10]+redF0[12]+redF0[14]+0x08)&0x0f;//校验位
        break;
        
    case 4:
        redF0[9] = status & 0x01;
        break;

    case 5:
        redF0[10] = status & 0x01;
        redF0[16] = (redF0[0]+redF0[2]+redF0[4]+redF0[6]+redF0[8]+redF0[10]+redF0[12]+redF0[14]+0x08)&0x0f;//校验位
        break;

    case 6:
        redF0[1] = ((status & 0x01)<<3)|(redF0[1]&0x07);
        break;
    default:
        break;
    }
    printf("%s: %d\n",ctrl_word[handleIndex],status);
    usleep(10);  //等待10ms
    client_send_ok(ctrl_word[handleIndex],BUFFER_SIZE_PIPE,status); 
    return 0;
}

/*
* Func Name   : client_send_ok
* Author      : liuchuanjia
* Date        : 2019/11/01
* Description : 接受控制命令的字段，向服务器返回ok
* Param       : const char *const client_buf，控制字段的名字；
* Return      : -1,客户端管道打开失败,处理成功；-2,客户端管道写入失败；0,成功
* Version     : v1.0
*/
static int client_send_ok(const __uint8_t *const client_buf,const size_t len,const __uint8_t stat)
{
    int client_fifo_fd;
    int res;
    cJSON *root;
    __uint8_t buf[BUFFER_SIZE_PIPE]={0};
    __uint8_t jsonBuf[JSON_BUF]={0};
    //准备客户端的返回json
    memcpy(buf,client_buf,len); 
    root=cJSON_CreateObject();
    cJSON_AddItemToObject(root,"t",cJSON_CreateString(buf));
    cJSON_AddItemToObject(root,"s",cJSON_CreateNumber(stat));
    strcpy(jsonBuf,cJSON_PrintUnformatted(root));

    client_fifo_fd=open(CLIENT_FIFO_NAME,O_WRONLY);
    if(client_fifo_fd == -1){
        fprintf(stderr,"Client fifo failure\n");
        return -1;
    }
    res = write(client_fifo_fd,jsonBuf,sizeof(jsonBuf));
    if(res == -1){
        fprintf(stderr,"Write error on client pipe\n");
        return -2;
    }
    cJSON_Delete(root);
    close(client_fifo_fd);
    return 0;
}


/*
* Func Name   : wifi_2_red
* Author      : liuchuanjia
* Date        : 2019/11/01
* Description : 根据更改的空体控制字段，对红外码特定的控制位进行更改
* Param       : None
* Return      : None
* Version     : v1.0
*/
// static int wifi_2_red_pipe(const __uint8_t handle_handle,const __uint8_t status)          
// { 

//     redF0[handle_handle]= status<<3 | 0x08;  //

//     printf("%#X\n",redF0[0]);
//     return 0;
// }   