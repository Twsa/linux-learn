#include "stdio.h"
#include "unistd.h"
//linux中线程操作相关的头文件
#include "pthread.h"
#include "sys/time.h"
/*
* 文件名：thread_first.c
* 日期：2019/09/11
* 作者：刘传佳
* 描述：这是一个简单的多线程的程序,
*	创建三个线程任务，task1,task2,task3,并加入到线程中。
*/
//时间戳变量定义
struct timeval stamp={0};
//task1
void *task1(void *ptr)
{
	while(1){
		gettimeofday(&stamp,NULL);
		printf("task1:%ld\n",stamp.tv_sec);
		sleep(3);
	}
}
//task2
void *task2(void *ptr)
{	
	while(1){
		gettimeofday(&stamp,NULL);
		printf("task2:%ld\n",stamp.tv_sec);
		sleep(3);
		}
}
//task3
void *task3(void *ptr)
{
	printf("This is task3\n");
}
//到达主线程入口
int main()
{
	printf("进入主线程\n");
	pthread_t task1_ID=0;
	pthread_t task2_ID=0;
	pthread_t task3_ID=0;
	//创建线程1
	if(pthread_create(&task1_ID,NULL,task1,NULL)!=0){
		printf("Task1 thread created failed!\n");
	}else{
		printf("Task1 created successfully!\n");
	}
	//创建线程2
	if(pthread_create(&task2_ID,NULL,task2,NULL)!=0){
		printf("Task2 thread created failed!\n");
	}else{
		printf("Task2 created successfully!\n");
	}
	//阻塞主进程，等待其他线程运行完成
	pthread_join(task1_ID,NULL);
	pthread_join(task2_ID,NULL);
	return 0;
}
