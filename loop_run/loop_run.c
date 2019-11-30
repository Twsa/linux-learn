/*
*  File Name   : loop_run.c
*  description : 周期运行某个程序 
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>	
#include <sys/time.h>
#include <signal.h>
#include <string.h>

static int time_run = 0;
static int time_tmp = 0;
static unsigned char cmd_buf[128] = {0};

static void set_timer(void);
// rcgcc loop_run.c -o loop_run && adb push loop_run /userdata
int main (int argc,char *argv[])
{
    if(argc<3){
        fprintf(stderr,"Please input <sec>,<command>\n");
        exit(EXIT_FAILURE);
        
    }
    /*设置定时时间*/
    time_run = atoi(argv[1]);
    time_tmp=time_run;
    /*设置运行命令*/
    memset(cmd_buf,'\0',sizeof(cmd_buf));
    strcpy(cmd_buf,argv[2]);

    set_timer();
    while(1);
}

static void timer_handle(int sign)
{
    if(time_tmp--==0){
        time_tmp = time_run;
        system(cmd_buf);
    }
}
static void set_timer(void)
{
    signal(SIGALRM,timer_handle);

    struct itimerval sectime;
    sectime.it_interval.tv_sec = 1;
    sectime.it_interval.tv_usec = 0;
    sectime.it_value.tv_sec = 0;
    sectime.it_value.tv_usec = 1;
    setitimer(ITIMER_REAL,&sectime,NULL);

}
