#include "stdio.h"
#define FLOAT_PRINTF
int float2hex(__uint8_t *data_hex,float engy_data)
{
	if(sizeof(float)==4)
	{
		data_hex[0] = (__uint8_t)(*(__uint32_t *)&engy_data>>24)&0x000000FF;
		data_hex[1] = (__uint8_t)(*(__uint32_t *)&engy_data>>16)&0x000000FF;
		data_hex[2] = (__uint8_t)(*(__uint32_t *)&engy_data>>8)&0x000000FF;
		data_hex[3] = (__uint8_t)(*(__uint32_t *)&engy_data)&0x000000FF;
	#ifdef FLOAT_PRINTF
		printf("data_float:%x\n",*(int *)&engy_data);
		printf("data_engy:%x\n",data_hex[0]);
		printf("data_engy:%x\n",data_hex[1]);
		printf("data_engy:%x\n",data_hex[2]);
		printf("data_engy:%x\n",data_hex[3]);
		return 0;
	#endif
	}
	else {
	#ifdef FLOAT_PRINTF
		printf("error,length of float is not 4\n");
	#endif
	}
	return 0;
}
int main()
{
	__uint8_t buf[4]={0};
	float engy_data=3.12;

	float2hex(buf,engy_data);
	return 0;
}
