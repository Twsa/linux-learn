#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#include <unistd.h>

int  tickets = 10;

pthread_mutex_t mut;//互斥锁
 
/**************线程1的服务**************/
void* tprocess1(void* args)
{
   int a = 0;
   while(1)
   {
      pthread_mutex_lock(&mut);              //互斥锁 上锁
      printf("After thread1 lock\n");
      if(tickets>0)
      {
         sleep(1);
         tickets--;                         //访问的公共数据
         printf("窗口1----------------还剩%d张票\n",tickets);      
      }
      else
         a = 1;
      pthread_mutex_unlock(&mut);             //互斥锁 解锁
      sleep(1);
      if(a == 1) 
         break;
   }
       pthread_mutex_unlock(&mut);
       return NULL;
}

/**************线程2的服务**************/
void* tprocess2(void* args)
{
   int a = 0;
   while(1){
   pthread_mutex_lock(&mut);
   printf("After thread2 lock\n");
   if(tickets>0)
   {
      sleep(1);
      tickets--;
      printf("窗口2----------------还剩%d张票\n",tickets);
     }else{
      a = 1;
     }
    pthread_mutex_unlock(&mut);
    sleep(1);
     if(a == 1) {
      break;
     }
  }
       pthread_mutex_unlock(&mut);
       return NULL;
}

/**************线程3的服务**************/
void* tprocess3(void* args)
{
   int a = 0;
   while(1)
   {
      pthread_mutex_lock(&mut);
      printf("After thread3 lock\n");
      if(tickets>0)
      {
         sleep(1);
         tickets--;
          printf("窗口3----------------还剩%d张票\n",tickets);
     }else{
      a = 1;
     }
      pthread_mutex_unlock(&mut);
      sleep(1);                  //等待线程之间的争夺
      if(a == 1) {
      break;
     }
  }
    
       pthread_mutex_unlock(&mut);
       return NULL;
}
/**************线程4的服务**************/
void* tprocess4(void* args)
{
   int a = 0;
   while(1){
   pthread_mutex_lock(&mut);//获得锁则继续进行，否则阻塞等待
   printf("After thread4 lock\n");
   if(tickets>0)
   {
      sleep(1);
      tickets--;
      printf("窗口4----------------还剩%d张票\n",tickets);
     }else{
         a = 1;
     }
    pthread_mutex_unlock(&mut);
    sleep(1);
     if(a == 1) {
      break;
     }
  }
    pthread_mutex_unlock(&mut);
    return NULL;
}

int main(){
  pthread_mutex_init(&mut,NULL);          // 初始化互斥锁
  pthread_t t1;
  pthread_t t2;
  pthread_t t3;
  pthread_t t4;                           //线程标识
  pthread_create(&t4,NULL,tprocess4,NULL);
  pthread_create(&t1,NULL,tprocess1,NULL);
  pthread_create(&t2,NULL,tprocess2,NULL);
  pthread_create(&t3,NULL,tprocess3,NULL);//创建四个线程
  //sleep(50);//能够让所有的县城执行完毕
 pthread_join(t1,NULL);                   //能够让所有的线程执行完毕
 pthread_join(t2,NULL);
 pthread_join(t3,NULL);  
 pthread_join(t4,NULL);
 pthread_mutex_destroy(&mut);              //销锁   
  return 0;
} 
