#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>


#define SERVER_FIFO_NAME "/tmp/serv_fifo"
// #define CLIENT_FIFO_NAME "/tmp/client_%d_fifo"
#define CNTL_WORD_NUMS 12
#define BUFFER_SIZE 20

#include "client.h"
#include <ctype.h>
#include "cJSON.h"
char jsonBuffer[BUFSIZ];

int ctl_word_handle(const char *const word,size_t len,bool status);

char *ctrl_word[11]={"pow","auto","cold_mode","hot_mode",\
"lesswet_mode","air_supy","windsped","temp","left_right","up_down","sleep"};

int main()
{
    int server_fifo_fd,client_fifo_fd;
    int res;
    cJSON *root,item;
    char cntl_word[BUFFER_SIZE];

    if(access(SERVER_FIFO_NAME,F_OK)==-1){
        res=mkfifo(SERVER_FIFO_NAME,0777);
        if(res!=0){
            fprintf(stderr,"Cound not create fifo %s\n",SERVER_FIFO_NAME);
        }
    }
       server_fifo_fd=open(SERVER_FIFO_NAME,O_RDONLY);
        if(server_fifo_fd == -1){
            fprintf(stderr,"Sorry.no server\n");
            exit(EXIT_FAILURE);
        }
    printf("Reading pipe\n");     //只有添加这个，才会显示第一条接受的信息？
    while(1)
    {
 
        do{
            res=read(server_fifo_fd,jsonBuffer,sizeof(jsonBuffer));
        }while(res<0);
        root=cJSON_Parse(jsonBuffer);
        strcpy(cntl_word,cJSON_GetObjectItem(root,"t")->valuestring);
        bool status=cJSON_GetObjectItem(root,"p")->type;
        server_fifo_fd=open(SERVER_FIFO_NAME,O_RDONLY);
        // printf("type: %s,",cntl_word);
        // printf("opt: %s\n",status==false?"false":"true"); 
        ctl_word_handle(cntl_word,BUFFER_SIZE,status);
    }
    close(server_fifo_fd);
    unlink(SERVER_FIFO_NAME);
    exit(EXIT_SUCCESS);
}


int ctl_word_handle(const char *const word,size_t len,bool status)
{
    char buffer[BUFFER_SIZE]={0};
    memcpy(buffer,word,BUFFER_SIZE);
    int handleIndex=0;
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
            }
        }
    }
    switch (handleIndex)
    {
    case 0:printf("pow: %s\n",status==false?"false":"true");break;
    case 1:printf("auto: %s\n",status==false?"false":"true");break;
    case 2:printf("cold_mode: %s\n",status==false?"false":"true");break;
    case 3:printf("hot_mode: %s\n",status==false?"false":"true");break;
    case 4:printf("lesswet_mode: %s\n",status==false?"false":"true");break;
    case 5:printf("air_supy: %s\n",status==false?"false":"true");break;
    case 6:printf("windsped: %s\n",status==false?"false":"true");break;
    case 7:printf("temp: %s\n",status==false?"false":"true");break;
    case 8:printf("left_right: %s\n",status==false?"false":"true");break;
    case 9:printf("up_down: %s\n",status==false?"false":"true");break;
    case 10:printf("sleep: %s\n",status==false?"false":"true");break;
    default:
        break;
    }
    return 0;
}
// #define SERVER_FIFO_NAME "/tmp/serv_fifo"
// #define CLIENT_FIFO_NAME "/tmp/client_%d_fifo"
// #define BUFFER_SIZE 20

// struct data_to_pass_st{
//     pid_t client_pid;
//     char some_data[BUFFER_SIZE -1];
// };

// #include "client.h"
// #include <ctype.h>
// #include "cJSON.h"
// struct data_to_pass_st my_data;

// void cjson_test(void)
// {
//     cJSON *root,item;
//     root=cJSON_CreateObject();
//     cJSON_AddItemToObject(root,"Pow",cJSON_CreateString("1"));
//     strcpy(my_data.some_data,cJSON_PrintUnformatted(root));
// }
// int main()
// {
//     int server_fifo_fd,client_fifo_fd;
//     int read_res;
//     char client_fifo[256];

//     server_fifo_fd=open(SERVER_FIFO_NAME,O_WRONLY);
//     if(server_fifo_fd == -1){
//         fprintf(stderr,"Sorry.no server\n");
//         exit(EXIT_FAILURE);
//     }
//     sprintf(client_fifo,CLIENT_FIFO_NAME,my_data.client_pid);
//     if(mkfifo(client_fifo,0777)==-1){
//         fprintf(stderr,"Sorry cant't make %s\n",client_fifo);
//         exit(EXIT_FAILURE);
//     }
//     for (int times_to_send=0;times_to_send<5;times_to_send++){
//         // sprintf(my_data.some_data,"Hello from %d",my_data.client_pid);
//         cjson_test();
//         printf("%d sent %s, ",my_data.client_pid,my_data.some_data);
//         write(server_fifo_fd,&my_data,sizeof(my_data));   //向服务器发送数据
//         client_fifo_fd =open(client_fifo,O_RDONLY);
//         if(client_fifo_fd !=-1){
//             if(read(client_fifo_fd,&my_data,sizeof(my_data))>0){
//                 printf("received: %s\n",my_data.some_data);
//             }
//             close(client_fifo_fd);
//         }
//     }
//             close(server_fifo_fd);
//             unlink(client_fifo); //关闭服务器FIFO并将客户FIFO从文件系统中删除
//             exit(EXIT_SUCCESS);
// }