#include <stdio.h>
#include "ping.h"

int main(int argc, char* argv[])
{
   if(ping_test() == 0){
       printf("connect net succeed\n");
   }else {
       printf("connect net failed\n");
   }
}