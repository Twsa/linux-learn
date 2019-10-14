#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

int usSleep(long us) {

    struct timeval tv;

    tv.tv_sec = 0;

    tv.tv_usec = us;

    return select(0, NULL, NULL, NULL, &tv);

 }

int secSleep(long sec) {

    struct timeval tv;

    tv.tv_sec = sec;

    tv.tv_usec = 0;

    return select(0, NULL, NULL, NULL, &tv);

 }

void get_time_stamp(void)
{
    struct  timeval stamp={0}; 
    gettimeofday(&stamp, NULL); 

    char buffer[128]={0};
    struct tm * timeinfo={NULL};

    printf("tv_sec : %ld \n",stamp.tv_sec); 
    printf("tv_usec : %ld \n",stamp.tv_usec);

    //时间戳转化成日期格式
    timeinfo = localtime (&stamp.tv_sec);
    strftime (buffer,sizeof(buffer),"Now is %Y/%m/%d %H:%M:%S",timeinfo);
    printf("%s\n", buffer);
}

// int settimeofday(const struct timeval *tv , const struct timezone *tz);
// struct timeval {
// time_t tv_sec;     /* seconds   since   Jan.   1,   1970  */
// suseconds_t tv_usec;    /* 微妙 */ 
// };


// struct timezone {
// int tz_minuteswest;     /* minutes west of Greenwich */
// int tz_dsttime;         /* type of DST correction */
// };


// 用于修改当前系统时间（只改变系统时间，不改变硬件时钟时间） 
// tv表示需要设置的时间，tz表示时区。如果tv或tz某一项为NULL，表示对相关的信息不感兴趣。
// struct tm {
// int tm_sec;  /*  秒 –  取值区间为[0,59] */
// int tm_min;  /*  分 -  取值区间为[0,59] */
// int tm_hour; /*  时 -  取值区间为[0,23] */
// int tm_mday;  /* 一个月中的日期 -  取值区间为[1,31] */
// int tm_mon;  /*  月份（从一月开始，0 代表一月） -  取值区间为[0,11] */
// int tm_year;  /*  年份，其值等于实际年份减去 1900 */ 
// int tm_wday;  /*  星期 – 取值区间为[0,6]，其中0代表星期天，1 代表星期一，以此类推 */
// int tm_yday;  /*  从每年的1 月1 日开始的天数 –  取值区间为[0,365]，其中0代表 1 月1日，1 代表1 月2日，以此类推 */
// int tm_isdst;/*  夏令时标识符，实行夏令时的时候，tm_isdst为正。不实行夏令时的进候，tm_isdst 为0；不了解情况时，tm_isdst()为负。*/
// };


// 把我们熟悉的时间格式存到该结构体中，
// 然后再用mktime()转成time_t. 
// time_t mktime(struct tm * timeptr); 
// 然后把mktime的返回值存入tv->tv_sec即可。
// 再用settimeofday就可以设置时间了

void set_time_stamp(void)
{
    struct tm timeISet={
        0,  //0秒
        0,  //0分
        0,  //0时
        7,  //7日
        9, //10月
        119,//2019年
        1,  //星期1
        279,
        0,
    };
    time_t stampIset=mktime(&timeISet);  //转换成秒
    struct timeval tv;                
    tv.tv_sec=stampIset;                //设置时间
    tv.tv_usec=0;
    int i=settimeofday(&tv,NULL);
    printf("%d\n",i);
}
int main()
{
    set_time_stamp();
    while(1)
    {
        // usSleep(500000);
        // system("echo 1 >/sys/class/leds/blueled/brightness");
        // usSleep(500000);
        // system("echo 0 >/sys/class/leds/blueled/brightness");
        secSleep(1);
        get_time_stamp();
    }
}