#include <stdio.h>
#include <sys/types.h>
#include <regex.h>
//https://www.cnblogs.com/zhidongjian/p/10097856.html
/*
* C语言中使用正则表达式一般分为三步：
* 编译正则表达式 regcomp()
* 匹配正则表达式 regexec()
* 释放正则表达式 regfree()
*/
int main(void)
{
int status = 0, i = 0;
int flag = REG_EXTENDED;
regmatch_t pmatch;
const size_t nmatch = 1;
regex_t reg;
//^\\w+([-+.]\\w+)*@\\w+([-.]\\w+)*\\.\\w+([-.]\\w+)*$
//^([0-9]{3}-|\([0-9]{3}\) )[0-9]{3}-([0-9]{4})$
const char *pattern = "^([0-9]{3}-|\\([0-9]{3}\\) )[0-9]{3}-[0-9]{4}";
char *buf = "987-123-4567\n123 456 7890\n(123) 456-7890";//success
// char *buf = "###270401@163.com";//error

regcomp(&reg, pattern, flag);
status = regexec(&reg, buf, nmatch, &pmatch, 0);
if(status == REG_NOMATCH){
printf("no match\n");
}else if(status == 0){
printf("match success\n");
for(i = pmatch.rm_so; i < pmatch.rm_eo; i++){
putchar(buf[i]);
}
putchar('\n');
}
regfree(&reg);

return 0;
}
