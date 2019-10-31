#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SERVER_FIFO_NAME "/tmp/serv_fifo"
// #define CLIENT_FIFO_NAME "/tmp/client_fifo"
#define BUFFER_SIZE 20
char *ctrl_word[11]={"pow","auto","cold_mode","hot_mode",\
"lesswet_mode","air_supy","windsped","temp","left_right","up_down","sleep"};
char *status[2]={"0","1"};
#include <ctype.h>
#include "cJSON.h"
char buffer[BUFSIZ];

int main(int argc ,char *argv[])
{
    int server_fifo_fd;
    int res;
    cJSON *root;
    
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
    server_fifo_fd=open(SERVER_FIFO_NAME,O_WRONLY);
    if(server_fifo_fd == -1){
        fprintf(stderr,"Server fifo failure\n");
        exit(EXIT_FAILURE);
    }
    // for(int i=0;i<22;i++)
    // {   
        root=cJSON_CreateObject();
        // if(i==11) {mode_i=0;status_i=1;}
        // cjson_test(mode_i++,status_i);
        cJSON_AddItemToObject(root,"t",cJSON_CreateString(argv[1]));
        cJSON_AddItemToObject(root,"p",cJSON_CreateBool(atoi(argv[2])));
        strcpy(buffer,cJSON_PrintUnformatted(root));
        res = write(server_fifo_fd,buffer,sizeof(buffer));  
        if(res == -1){
            fprintf(stderr,"Write error on pipe\n");
            exit(EXIT_FAILURE);
        }
        cJSON_Delete(root);
        // sleep(1);
    // }
    // close(server_fifo_fd);
    // unlink(SERVER_FIFO_NAME);
    exit(EXIT_SUCCESS);
}


// struct data_to_pass_st{
//     pid_t client_pid;
//     char some_data[BUFFER_SIZE -1];
// };

// #include "client.h"
// #include <ctype.h>
// #include "cJSON.h"


// int main()
// {
//     int server_fifo_fd,client_fifo_fd;
//     struct data_to_pass_st my_data;
//     int read_res;
//     char client_fifo[256];
//     char *tmp_char_ptr;

//     mkfifo(SERVER_FIFO_NAME,0777);
//     server_fifo_fd=open(SERVER_FIFO_NAME,O_RDONLY);
//     if(server_fifo_fd == -1){
//         fprintf(stderr,"Server fifo failure\n");
//         exit(EXIT_FAILURE);
//     }
//     sleep(10);  /*lets clients queue for demo pu*/
//     do{
//         read_res = read(server_fifo_fd,&my_data,sizeof(my_data));
//         if(read_res>0){
//             tmp_char_ptr=my_data.some_data;
//             while(*tmp_char_ptr){
//                 *tmp_char_ptr = toupper(*tmp_char_ptr);
//                 tmp_char_ptr++;
//             }
//             sprintf(client_fifo,CLIENT_FIFO_NAME,my_data.client_pid);
//             client_fifo_fd=open(client_fifo,O_WRONLY);
//             if(client_fifo_fd != -1){
//                 write(client_fifo_fd,&my_data,sizeof(my_data));
//                 close(client_fifo_fd);
//             }
//         }
//     }while(read_res>0);
//     unlink(SERVER_FIFO_NAME);
//     exit(EXIT_SUCCESS);
// }