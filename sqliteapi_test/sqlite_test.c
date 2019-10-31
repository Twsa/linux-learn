#include <stdio.h>
#include <stdlib.h>
#include "./sqlite-amalgamation-3300100/sqlite3.h"
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

int showTableInfo ( void *para,int n_column,char **column_value,char **column_name );
int showCount ( void *para,int n_column,char **column_value,char **column_name );
void set_5mins_timmer(void);

sqlite3 *db=NULL;
char *errMsg;
int rc;

//gcc  sqlite_test.c sqlite-amalgamation-3300100/sqlite3.c -o sqlite_test -pthread -ldl
int main ( int argc, char **argv ) {

    //打开指定的数据库文件,如果不存在将创建一个同名的数据库文件
    rc = sqlite3_open ( "liu.db", &db );


    printf ("%s\n",sqlite3_version );
    if ( rc ==SQLITE_OK ) {
        //如果打开成功
        printf ( "打开数据库成功！" );
        rc = sqlite3_exec ( db,"create table if not exists twsa(time varchar(32), engyinfo float)",NULL,NULL,&errMsg );
        if ( rc != SQLITE_OK ) {
            printf ( "创建表失败，错误码：%d,错误原因:%sn",rc,errMsg );
            printf ( "创建表user失败!" );

        }
    }
    set_5mins_timmer();
    while(1);
    return 0;
}


int showTableInfo ( void *para,int n_column,char **column_value,char **column_name ) {
    int i;
    printf ( "记录包含%d个字段\n",n_column );
    for ( i=0; i<n_column; i++ ) {
        printf ( "字段名：%s  >> 字段值：%s\n",column_name[i],column_value[i] );
    }
    printf ( "--------------------------------------\n" );
    return 0;
}

int showCount ( void *para,int n_column,char **column_value,char **column_name ) {
    printf ( "一共%s条记录%s\n",column_value[0],column_name[0] );
    return 0;
}

void save_engy_info_5mins(int parameter)
{

    struct  timeval stamp={0}; 
    char buffer[128]={0};
    struct tm * timeinfo={NULL};
    sqlite3_stmt *stmt;
    float engyTest=1.12;
    char *sql = "insert into twsa (time, engyinfo) values(?,?)";

     /*********获取UTC时间 start*************/
    gettimeofday(&stamp, NULL); 
    timeinfo = localtime (&stamp.tv_sec);
    strftime (buffer,sizeof(buffer),"%Y/%m/%d %H:%M:%S",timeinfo);
    /*********获取UTC时间 end*************/
    rc = sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL);
    sqlite3_bind_text(stmt, 1, buffer, strlen(buffer), SQLITE_STATIC);
    sqlite3_bind_double(stmt,2,engyTest);

    // rc=sqlite3_exec ( db,"insert into twsa values(buffer,'测试')",NULL,NULL,&errMsg );
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if ( rc != SQLITE_OK ) {
        printf ( "插入数据失败!" );
    }
    rc=sqlite3_exec ( db,"select * from twsa",showTableInfo,NULL,&errMsg );
    rc=sqlite3_exec ( db,"select count(*) from twsa",showCount,NULL,&errMsg );
    if ( rc != SQLITE_OK ) {
        printf ( "查询失败!" );
    }
}

void set_5mins_timmer(void)
{
    struct itimerval value, ovalue, value2;
      
    printf("process id is %d\n", getpid());
    //定时器服务程序绑定
    signal(SIGALRM, save_engy_info_5mins);

    //基础定时器配置
    value.it_value.tv_sec = 1;
    value.it_value.tv_usec = 0;
    value.it_interval.tv_sec = ;
    value.it_interval.tv_usec = 0;
   setitimer(ITIMER_REAL, &value, &ovalue);  
}

