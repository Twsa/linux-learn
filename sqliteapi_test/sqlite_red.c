#include "./sqlite-amalgamation-3300100/sqlite3.h"
#include <stdio.h>  
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
int get_redcode_buf(void);

#define TABLE  "red_code"
char *red_sql=  "CREATE TABLE IF NOT EXISTS red_table (\
        BTN_NUM INT PRIMARY KEY     NOT NULL,\
        NAME          TEXT    NOT NULL,\
        RED_CODE      TEXT    NOT NULL\
        );";
char *red_insert_sql = "insert into red_table (BTN_NUM, NAME,RED_CODE) values(?,?,?)";    //准备向表中插入变量
char *errMsg;
char redCode[BUFSIZ];        //红外码
char bltName[11]="blt_name";   //按钮的名字
int  bltNum=2;        //按钮的编号

int showTableInfo ( void *para,int n_column,char **column_value,char **column_name ) {
    int i;
    printf ( "记录包含%d个字段\n",n_column );
    for ( i=0; i<n_column; i++ ) {
        printf ( "字段名：%s  >> 字段值：%s\n",column_name[i],column_value[i] );
    }
    printf ( "--------------------------------------\n" );
    return 0;
}

int main(void) {


    sqlite3 *db;
    char *err_msg = NULL;
    sqlite3_stmt *stmt;

    get_redcode_buf();

    /***********创建数据库************/
    int rc = sqlite3_open("test.db", &db);
    if (rc != SQLITE_OK) {
        printf("open database test.db failed\n");
        sqlite3_close(db);
        return 1;
    }


    /***********创建表************/
    rc = sqlite3_exec(db, red_sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        return 1;
    } 

    /*********准备绑定变量的sql*******/
    rc = sqlite3_prepare(db, red_insert_sql, strlen(red_insert_sql), &stmt, NULL);             //准备绑定变量的sql
    sqlite3_bind_int(stmt, 1, bltNum);
    sqlite3_bind_text(stmt, 2, bltName, strlen(bltName), SQLITE_STATIC);   //绑定变量
    sqlite3_bind_text(stmt, 3, redCode, strlen(redCode), SQLITE_STATIC);   //绑定变量

    sqlite3_step(stmt); 
    sqlite3_finalize(stmt);
     rc=sqlite3_exec ( db,"select * from red_table",showTableInfo,NULL,&errMsg );
    /**********关闭数据库**********/
    sqlite3_close(db);
    return 0;
}


int get_redcode_buf(void)
{
    char strCmd[BUFSIZ]="cat /home/twsa/Desktop/my_test/sqliteapi_test/ir_recv";   
    FILE *read_fp;
    int chars_read;
    memset(redCode, '\0', sizeof(redCode));
    read_fp = popen(strCmd, "r");
    if (read_fp != NULL) {
        chars_read = fread(redCode, sizeof(char), sizeof(redCode), read_fp);
        pclose(read_fp);
        printf("redCode:%s\n",redCode);
        return 0;
    }
    return -1;
}

