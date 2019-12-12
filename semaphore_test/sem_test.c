#include<semaphore.h>
#include<unistd.h>
#include<stdio.h>
#include<pthread.h>

static int buff[1]={0};
sem_t put,get;
void signal_init()
{
    sem_init(&put,0,1);
    sem_init(&get,0,0);

}

void signal_destroy()
{
    sem_destroy(&put);
    sem_destroy(&get);
}

void *producer_signal(void *arg)
{
    int i;
    for(i=1;i<=10;i++)
    {
        sem_wait(&put);
        buff[1]=i;
        printf("value put int the buffer=%d\n",i);
        sem_post(&get);
    }
    pthread_exit((void *)"pthread producer exist\n");
}

void *customer_signal(void *arg)
{
    int i;
    int value;
    for(i=1;i<=10;i++)
    {
        sem_wait(&get);
        value=buff[1];
        printf("value get from the buffer=%d\n",value);
        sem_post(&put);
    }
    pthread_exit((void *)"thread customer exist\n");
}
int main()
{
    void *ret;
    int ret_value;
    signal_init();
    pthread_t tid_producer,tid_customer;
    ret_value=pthread_create(&tid_producer,NULL,producer_signal,NULL);
    pthread_create(&tid_customer,NULL,customer_signal,NULL);
    pthread_join(tid_producer,&ret);
    pthread_join(tid_customer,&ret);
    printf("tid_producer=%d\n,ret_value=%d",tid_producer,ret_value);
    signal_destroy();
    return 0;
}
