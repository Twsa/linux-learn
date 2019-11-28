#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
int main(int argc,char *argv[])
{
    int fd,n;
    char buf[4];
    float data=3.1415;
    char *path="/tmp/engy_data.txt";
    sprintf(buf,"%0.2f",data);  
    if((access("test.c",F_OK))!=-1)   
    {   
      ;
    }   
    else  
    {   
        if(creat(path,00777)<0){            
            fprintf(stderr,"create file fail <erron:%d>\n",errno);
            exit(1);
        }           //创建文件
    }   
    fd=open(path,O_WRONLY);
    if(fd<0){
        fprintf(stderr,"open file fail <erron:%d>\n",errno);
        exit(1); 
    }else{
        if(write(fd,buf,sizeof(buf))<0){
            fprintf(stderr,"write file fail <erron:%d>\n",errno);
            exit(1);      
        }else{
            printf("hello\n");
        }
    }
    close(fd);
return 0;
}