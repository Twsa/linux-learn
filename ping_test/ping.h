#ifndef __PING_H_
#define __PING_H_
#define PACKET_SEND_MAX_NUM 3
#define PINGTEST_DOMAIN "www.baidu.com"

void icmp_sigint(int signo);
void ping_stats_show(void);
int ping_test(void);
#endif