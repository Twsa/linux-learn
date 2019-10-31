#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <unistd.h> 
#include <errno.h>

int main(int argc,char *argv[])
{
        if(argc!=3)
        {
           printf("./Usage [ip] [port]\n");
           return 1;
        }
        int sock=socket(AF_INET,SOCK_DGRAM,0);  
        if(sock<0)
        {
                perror("socket");
                return 2;

        }

        struct sockaddr_in server;
        server.sin_family=AF_INET;
        server.sin_port=htons(atoi(argv[2]));
        server.sin_addr.s_addr=inet_addr(argv[1]);


        char buf[1024];
        struct sockaddr_in peer;
        while(1)
        {
                socklen_t len=sizeof(peer);
                printf("Please Enter# ");
                fflush(stdout);
                ssize_t s=read(0,buf,sizeof(buf)-1);
                if(s>0)
                {
                        buf[s-1]=0;

                        int on=1;
                        setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
                        int sendto_return=sendto(sock,buf,strlen(buf),0,\
                                        (struct sockaddr*)&server,sizeof(server));
                        int errmsg=errno;
                        printf("errno:%d\n",errno);
			printf("sendto_return:%d\n",sendto_return);
                }
        }
        close(sock);
        return 0;

}
