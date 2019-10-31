#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//队列的结点结构
typedef struct Node
{
    int data;
    struct Node *next;
}Node,Queue;

//队列的结构，嵌套
typedef struct
{
    Queue *front;
    Queue * rear;
}LinkQueue;

//初始化
//开始必然是空队列，队尾指针和队头指针都指向头节点
void init_queue(LinkQueue *queue)
{
    //初始化头节点
    queue->front=queue->rear=(Queue *)malloc(sizeof(Node));
    if(queue->front == NULL)
    {
        exit(0);
    }
    queue->front->next = NULL;
    queue->front->data = 0;
}

//判断队列是否为空
bool is_empty(LinkQueue queue)
{
    return queue.rear == queue.front ? true:false;
}

//插入队列，只在一端入队，另一端出队，同样入队不需要判满
void insert_queue(LinkQueue *queue,int temp)
{
    Queue *q =(Queue *)malloc(sizeof(Node));

    //给新的节点插入数据
    q->data = temp;
    q->next = NULL;     //插入的节点的下一个节点指针指向NULL

    //rear总是指向队尾元素
    queue->rear->next = q;
    queue->rear = q;
}

//出队，需要判空
void delete_queue(LinkQueue *queue)
{
    Queue *q=NULL;
    if(!is_empty(*queue))
    {
        q = queue->front->next;
        queue->front->next = q->next;

        if(queue->rear == q)
        {
            queue->rear = queue->front;
        }
    }
    free(q);
}
//遍历队列的所有数据
void traversal(LinkQueue queue)
{
    int i = 1; 
    Queue *q = queue.front->next;
    
    while (q != NULL) {
        printf("%d,", q->data);
        q = q->next;
        i++;
    }
    printf("\n");
}

LinkQueue queue_myself;
void create_room_50ps(void)
{
    init_queue(&queue_myself);
    is_empty(queue_myself);
    int i=1;
    
    while(i++<51)
    {
        insert_queue(&queue_myself,i-1);
    }
}
//空间生存游戏
void room_survive(int data)
{
    delete_queue(&queue_myself);
    insert_queue(&queue_myself,data);
}

//销毁

//测试程序
int main()
{
    create_room_50ps();
    traversal(queue_myself);
    printf("开始生存\n");
    room_survive(51);
    traversal(queue_myself);
     printf("开始生存\n");
    room_survive(52);
    traversal(queue_myself);
        printf("开始生存\n");
    room_survive(53);
    traversal(queue_myself);
        printf("开始生存\n");
    room_survive(54);
    traversal(queue_myself);
    return 0;
}