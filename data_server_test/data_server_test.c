#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char LOCAL_MAC[13]="cabc63e18304";
#define localMAC LOCAL_MAC
void test_data_ready(void)
{
    /*char类型数据*/
    char datacharBuf[1024]={0};
    char databufDynamic[1024]={0};  
    char loginCid[20]={0};
    char targetCid[20]={0};
    // char localMAC[13]="cabc63e18304";
    unsigned char temp = 0;

    /*__uint16_t类型数据*/
    __uint16_t timeYear=2019;
    __uint16_t timerMonth=10;
    __uint16_t timeDay=19;
    __uint16_t timeHour=8;
    __uint16_t timeMinute=9;
    __uint16_t timeSecond=0;

    /*int类型数据*/
    int length=0;
    int i=0;
    unsigned int  data[6]={0};

    /*__uint32_t类型数据*/
    __uint32_t wifiMid_int32=0;
    __uint32_t venderInt=0;

    memset(datacharBuf,'\0',sizeof(datacharBuf));
    datacharBuf[0] = 0x66;
    datacharBuf[1] = 0x67;

    strcpy(loginCid,localMAC);

    if(length >= 256)
    {
      datacharBuf[2] = (unsigned char)((length&0xFF00)>>8);
      datacharBuf[3] = (unsigned char)(length&0x00FF);
    }
    else
    {
      datacharBuf[2] = temp;
      datacharBuf[3] = (unsigned char)(length&0x00FF);
    }
  
    for(i=0;i<6;i++) data[i] = 0;    
    sscanf(loginCid,"%2X%2X%2X%2X%2X%2X",data,data+1,data+2,data+3,data+4,data+5);
    for(i=4;i<10;i++) datacharBuf[i] = data[i-4];
   
    for(i=0;i<6;i++) data[i] = 0;
    sscanf(targetCid,"%2X%2X%2X%2X%2X%2X",data,data+1,data+2,data+3,data+4,data+5);
    for(i=10;i<16;i++) datacharBuf[i] = data[i-10];
    
    for(i=0;i<6;i++) data[i] = 0;
    sscanf(localMAC,"%2X%2X%2X%2X%2X%2X",data,data+1,data+2,data+3,data+4,data+5);
    for(i=16;i<22;i++) datacharBuf[i] = data[i-16];      
    datacharBuf[22] = (wifiMid_int32>>24)&0x000000FF;
    datacharBuf[23] = (wifiMid_int32>>16)&0x000000FF;
    datacharBuf[24] = (wifiMid_int32>>8)&0x000000FF;
    datacharBuf[25] = wifiMid_int32&0x000000FF;
    datacharBuf[26] = (venderInt>>24)&0x000000FF;
    datacharBuf[27] = (venderInt>>16)&0x000000FF;
    datacharBuf[28] = (venderInt>>8)&0x000000FF;
    datacharBuf[29] = venderInt&0x000000FF;
    datacharBuf[30] = 1;
    datacharBuf[31] = 0;  
    datacharBuf[32] = (timeYear-2000);
    datacharBuf[33] = timerMonth;
    datacharBuf[34] = timeDay;
    datacharBuf[35] = timeHour;
    datacharBuf[36] = timeMinute;
    datacharBuf[37] = timeSecond;  
    datacharBuf[38] = 0x02; 
    for(i=39;i<length+39;i++) datacharBuf[i] = databufDynamic[i-39];
}

int main()
{
    test_data_ready();
    return 0;
}