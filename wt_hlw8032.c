/*----------------------------------------------------------------------------
 * Filename: wt_hlw8032.c
 *
 * Function: Deal HLW8032 Uart Data.
 *
 * Author  : tangfeng@wingtech.com.
 *
 * Date    : 20190322.
 *
 * Version : 1.0.2
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/types.h>

#define DEBUG

#define FALSE  -1
#define TRUE   0

#define DATA_PACKAGE 24
#define SAMPLE_RESISTANCE_MR	1	//使用的采样锰铜电阻mR值

#define	EPERM		 1	/* Operation not permitted */
#define	ENODEV		19	/* No such device */

#define UART_IND_HD			0	//State REG
#define UART_IND_5A			1	//Check REG
#define UART_IND_VK			2	//Voltage Parameter REG
#define UART_IND_VT			5	//Voltage REG
#define UART_IND_IK			8	//Current Parameter REG
#define UART_IND_IT			11	//Current REG
#define UART_IND_PK			14	//Power parameter REG
#define UART_IND_PT			17	//Power REG
#define UART_IND_FG			20	//Data Update
#define UART_IND_EN			21	//PF REG
#define UART_IND_SM			23	//CheckSum REG

#define ARRAY_LEN   	3	//平滑滤波buf长度
#define COUNT_NUM   	1	//超时更新数据次数

//8032电能计数脉冲溢出时的数据
#define ENERGY_FLOW_NUM			65536	//电量采集，电能溢出时的脉冲计数值

static int wait_flag = 0;
static volatile int signal_stop = FALSE;
static unsigned char hlw_buff[24] = { 0 };

typedef struct RuningInf_s {
    unsigned short voltage;	//当前电压值，单位为0.1V
    unsigned short electricity;	//当前电流值,单位为0.01A
    unsigned short power;	//当前功率,单位为0.1W

    unsigned long energy;	//当前消耗电能值对应的脉冲个数
    unsigned short energyCurrent;	//电能脉冲当前统计值
    unsigned short energyLast;	//电能脉冲上次统计值
    unsigned char energyFlowFlag;	//电能脉冲溢出标致

    unsigned long energyUnit;	//0.001度点对应的脉冲个数 
} RuningInf_t;

RuningInf_t runingInf;

//获取电压、电流、功率的有限数据
unsigned long getVIPvalue(unsigned long *arr)	//更新电压、电流、功率的列表
{
    int maxIndex = 0;
    int minIndex = 0;
    unsigned long sum = 0;
    int j = 0;
    for (j = 1; j < ARRAY_LEN; j++) {
	if (arr[maxIndex] <= arr[j]) {	//避免所有数据一样时minIndex等于maxIndex
	    maxIndex = j;
	}
	if (arr[minIndex] > arr[j]) {
	    minIndex = j;
	}
    }

    for (j = 0; j < ARRAY_LEN; j++) {
	if ((maxIndex == j) || (minIndex == j)) {
	    continue;
	} else {
	    return arr[j];
	}
    }
}

int isUpdataNewData(unsigned long *arr, unsigned long dat)	//检测电压电流功率是否需要更新
{
    if (arr[0] == dat) {
	return 0;
    } else {
	return 1;
    }
}

static inline void updataVIPvalue(unsigned long *arr, unsigned long dat)	//更新电压、电流、功率的列表
{
    int ii = ARRAY_LEN - 1;
    for (ii = ARRAY_LEN - 1; ii > 0; ii--) {
	arr[ii] = arr[ii - 1];
    }
    arr[0] = dat;
}

static inline void resetVIPvalue(unsigned long *arr, unsigned long dat)	//更新所有电压、电流、功率的列表
{
    int ii = ARRAY_LEN - 1;
    for (ii = ARRAY_LEN - 1; ii >= 0; ii--) {
	arr[ii] = dat;
    }
}

int DealUartInf(unsigned char *inDataBuffer, int recvlen)
{
    unsigned char startFlag = 0;
    unsigned int checksum = 0;
    unsigned long voltage_k = 0;
    unsigned long voltage_t = 0;
    unsigned long voltage = 0;
    static unsigned long voltage_a[ARRAY_LEN] = { 0 };
    static unsigned int voltageCnt = 0;

    unsigned long electricity_k = 0;
    unsigned long electricity_t = 0;
    unsigned long electricity = 0;
    static unsigned long electricity_a[ARRAY_LEN] = { 0 };
    static unsigned int electricityCnt = 0;

    unsigned long power_k = 0;
    unsigned long power_t = 0;
    unsigned long power = 0;
    static unsigned long power_a[ARRAY_LEN] = { 0 };
    static unsigned int powerCnt = 0;
    static unsigned long powerNewFlag = 1;

    unsigned int energy_cnt = 0;
    unsigned char energyFlowFlag = 0;

    if (recvlen != 24) {
	printf
	    ("ERROR:WT_HLW8032 Need A Complete Packet == 24 Bytes!!! \n");
	return -1;
    }
#ifdef DEBUG
    printf("print input data\n");
    for (int j = 0; j < 24; j++) {
	printf("%#x;", inDataBuffer[j]);
    }
    printf("\n");
#endif

    /* checksum */
    for (int j = 0; j < 21; j++) {
	checksum += inDataBuffer[j + 2];
    }
    checksum = checksum & 0xFF;
#ifdef DEBUG
    printf("Checksum data :%#x=%#x?\n", checksum, inDataBuffer[23]);
#endif
    if (checksum != inDataBuffer[23]) {
	printf("Error:Checksum data \n");
	return -2;
    }

    startFlag = inDataBuffer[UART_IND_HD];
    switch (startFlag) {
    case 0x55:
	if ((inDataBuffer[UART_IND_FG] & 0x40) == 0x40) {	//获取当前电压标致，为1时说明电压检测OK
	    voltage_k = ((inDataBuffer[UART_IND_VK] << 16) | (inDataBuffer[UART_IND_VK + 1] << 8) | (inDataBuffer[UART_IND_VK + 2]));	//电压系数
	    voltage_t = ((inDataBuffer[UART_IND_VT] << 16) | (inDataBuffer[UART_IND_VT + 1] << 8) | (inDataBuffer[UART_IND_VT + 2]));	//电压周期

	    if (isUpdataNewData(voltage_a, voltage_t)) {
		updataVIPvalue(voltage_a, voltage_t);
		voltageCnt = 0;
	    } else {
		voltageCnt++;
		if (voltageCnt >= COUNT_NUM) {
		    voltageCnt = 0;
		    updataVIPvalue(voltage_a, voltage_t);
		}
	    }
	    printf("voltage:%ld,%ld,%ld\r\n", voltage_a[0], voltage_a[1],
		   voltage_a[2]);
	    voltage_t = getVIPvalue(voltage_a);

	    if (voltage_t == 0) {
		voltage = 0;
	    } else {
		voltage = voltage_k * 100 / voltage_t;	//电压10mV值，避免溢出
		voltage = voltage * 10;	//电压mV值
	    }
	    printf("Vk = %ld,Vt = %ld,v = %ld\r\n", voltage_k, voltage_t,
		   voltage);
	} else {
	    printf("%s(%d):V Flag Error\r\n", __func__, __LINE__);
	}

	if ((inDataBuffer[UART_IND_FG] & 0x20) == 0x20) {
	    electricity_k = ((inDataBuffer[UART_IND_IK] << 16) | (inDataBuffer[UART_IND_IK + 1] << 8) | (inDataBuffer[UART_IND_IK + 2]));	//电流系数
	    electricity_t = ((inDataBuffer[UART_IND_IT] << 16) | (inDataBuffer[UART_IND_IT + 1] << 8) | (inDataBuffer[UART_IND_IT + 2]));	//电流周期

	    if (isUpdataNewData(electricity_a, electricity_t)) {
		updataVIPvalue(electricity_a, electricity_t);
		electricityCnt = 0;
	    } else {
		electricityCnt++;
		if (electricityCnt >= COUNT_NUM) {
		    electricityCnt = 0;
		    updataVIPvalue(electricity_a, electricity_t);
		}
	    }
	    printf("electricity:%ld,%ld,%ld\r\n", electricity_a[0],
		   electricity_a[1], electricity_a[2]);
	    electricity_t = getVIPvalue(electricity_a);

	    if (electricity_t == 0) {
		electricity = 0;
	    } else {
		electricity = electricity_k * 100 / electricity_t;	//电流10mA值，避免溢出
		electricity = electricity * 10;	//电流mA值
#if(SAMPLE_RESISTANCE_MR == 1)
		//由于使用1mR的电阻，电流和功率需要不除以2
#elif(SAMPLE_RESISTANCE_MR == 2)
		//由于使用2mR的电阻，电流和功率需要除以2
		electricity >>= 1;
#endif
	    }
	    printf("Ik = %ld,It = %ld,I = %ld\r\n", electricity_k,
		   electricity_t, electricity);
	} else {
	    printf("%s(%d):I Flag Error\r\n", __func__, __LINE__);
	}

	if ((inDataBuffer[UART_IND_FG] & 0x10) == 0x10) {
	    powerNewFlag = 0;
	    power_k = ((inDataBuffer[UART_IND_PK] << 16) | (inDataBuffer[UART_IND_PK + 1] << 8) | (inDataBuffer[UART_IND_PK + 2]));	//功率系数
	    power_t = ((inDataBuffer[UART_IND_PT] << 16) | (inDataBuffer[UART_IND_PT + 1] << 8) | (inDataBuffer[UART_IND_PT + 2]));	//功率周期

	    if (isUpdataNewData(power_a, power_t)) {
		updataVIPvalue(power_a, power_t);
		powerCnt = 0;
	    } else {
		powerCnt++;
		if (powerCnt >= COUNT_NUM) {
		    powerCnt = 0;
		    updataVIPvalue(power_a, power_t);
		}
	    }
	    printf("power:%ld,%ld,%ld\r\n", power_a[0], power_a[1],
		   power_a[2]);
	    power_t = getVIPvalue(power_a);

	    if (power_t == 0) {
		power = 0;
	    } else {
		power = power_k * 100 / power_t;	//功率10mw值，避免溢出
		power = power * 10;	//功率mw值
#if(SAMPLE_RESISTANCE_MR == 1)
		//由于使用1mR的电阻，电流和功率需要不除以2
#elif(SAMPLE_RESISTANCE_MR == 2)
		//由于使用2mR的电阻，电流和功率需要除以2
		power >>= 1;
#endif
	    }
	    printf("Pk = %ld,Pt = %ld,P = %ld\r\n", power_k, power_t,
		   power);
	} else if (powerNewFlag == 0) {
	    power_k = ((inDataBuffer[UART_IND_PK] << 16) | (inDataBuffer[UART_IND_PK + 1] << 8) | (inDataBuffer[UART_IND_PK + 2]));	//功率系数
	    power_t = ((inDataBuffer[UART_IND_PT] << 16) | (inDataBuffer[UART_IND_PT + 1] << 8) | (inDataBuffer[UART_IND_PT + 2]));	//功率周期

	    if (isUpdataNewData(power_a, power_t)) {
		unsigned long powerData = getVIPvalue(power_a);
		if (power_t > powerData) {
		    if ((power_t - powerData) > (powerData >> 2)) {
			resetVIPvalue(power_a, power_t);
		    }
		}
	    }
	    printf("power:%ld,%ld,%ld\r\n", power_a[0], power_a[1],
		   power_a[2]);
	    power_t = getVIPvalue(power_a);

	    if (power_t == 0) {
		power = 0;
	    } else {
		power = power_k * 100 / power_t;	//功率10mw值，避免溢出
		power = power * 10;	//功率mw值
#if(SAMPLE_RESISTANCE_MR == 1)
		//由于使用1mR的电阻，电流和功率需要不除以2
#elif(SAMPLE_RESISTANCE_MR == 2)
		//由于使用2mR的电阻，电流和功率需要除以2
		power >>= 1;
#endif
	    }
	    printf("Pk = %ld,Pt = %ld,P = %ld\r\n", power_k, power_t,
		   power);
	}

	energyFlowFlag = (inDataBuffer[UART_IND_FG] >> 7);	//获取当前电能计数溢出标致
	runingInf.energyCurrent = ((inDataBuffer[UART_IND_EN] << 8) | (inDataBuffer[UART_IND_EN + 1]));	//更新当前的脉冲计数值
	if (runingInf.energyFlowFlag != energyFlowFlag) {	//每次计数溢出时更新当前脉冲计数值
	    runingInf.energyFlowFlag = energyFlowFlag;
	    if (runingInf.energyCurrent > runingInf.energyLast) {
		runingInf.energyCurrent = 0;
	    }
	    energy_cnt =
		runingInf.energyCurrent + ENERGY_FLOW_NUM -
		runingInf.energyLast;
	} else {
	    energy_cnt = runingInf.energyCurrent - runingInf.energyLast;
	}
	runingInf.energyLast = runingInf.energyCurrent;
	runingInf.energy += (energy_cnt * 10);	//电能个数累加时扩大10倍，计算电能是除数扩大10倍，保证计算精度

	runingInf.energyUnit = 0xD693A400 >> 1;
	runingInf.energyUnit /= (power_k >> 1);	//1mR采样电阻0.001度电对应的脉冲个数
#if(SAMPLE_RESISTANCE_MR == 1)
	//1mR锰铜电阻对应的脉冲个数
#elif(SAMPLE_RESISTANCE_MR == 2)
	//2mR锰铜电阻对应的脉冲个数
	runingInf.energyUnit = (runingInf.energyUnit << 1);	//2mR采样电阻0.001度电对应的脉冲个数
#endif
	runingInf.energyUnit = runingInf.energyUnit * 10;	//0.001度电对应的脉冲个数(计算个数时放大了10倍，所以在这里也要放大10倍)

	//电能使用量=runingInf.energy/runingInf.energyUnit;//单位是0.001度
	break;

    case 0xAA:
	//芯片未校准
	printf("WT_HLW8032 not check\r\n");
	break;

    default:
	if ((startFlag & 0xF1) == 0xF1) {	//存储区异常，芯片坏了
	    //芯片坏掉，反馈服务器
	    printf("ERROR:WT_HLW8032 broken\r\n");
	}

	if ((startFlag & 0xF2) == 0xF2) {	//功率异常
	    runingInf.power = 0;	//获取到的功率是以0.1W为单位
	    power = 0;
	    printf("ERROR:Power Error\r\n");
	} else {
	    if ((inDataBuffer[UART_IND_FG] & 0x10) == 0x10) {
		powerNewFlag = 0;
		power_k = ((inDataBuffer[UART_IND_PK] << 16) | (inDataBuffer[UART_IND_PK + 1] << 8) | (inDataBuffer[UART_IND_PK + 2]));	//功率系数
		power_t = ((inDataBuffer[UART_IND_PT] << 16) | (inDataBuffer[UART_IND_PT + 1] << 8) | (inDataBuffer[UART_IND_PT + 2]));	//功率周期

		if (isUpdataNewData(power_a, power_t)) {
		    updataVIPvalue(power_a, power_t);
		    powerCnt = 0;
		} else {
		    powerCnt++;
		    if (powerCnt >= COUNT_NUM) {
			powerCnt = 0;
			updataVIPvalue(power_a, power_t);
		    }
		}
		//printf("power:%d,%d,%d\r\n",power_a[0],power_a[1],power_a[2]);
		power_t = getVIPvalue(power_a);

		if (power_t == 0) {
		    power = 0;
		} else {
		    power = power_k * 100 / power_t;	//功率10mw值，避免溢出
		    power = power * 10;	//功率mw值
#if(SAMPLE_RESISTANCE_MR == 1)
		    //由于使用1mR的电阻，电流和功率需要不除以2
#elif(SAMPLE_RESISTANCE_MR == 2)
		    //由于使用2mR的电阻，电流和功率需要除以2
		    power >>= 1;
#endif
		}
		printf("Pk = %ld,Pt = %ld,P = %ld\r\n", power_k, power_t,
		       power);
	    } else if (powerNewFlag == 0) {
		power_k = ((inDataBuffer[UART_IND_PK] << 16) | (inDataBuffer[UART_IND_PK + 1] << 8) | (inDataBuffer[UART_IND_PK + 2]));	//功率系数
		power_t = ((inDataBuffer[UART_IND_PT] << 16) | (inDataBuffer[UART_IND_PT + 1] << 8) | (inDataBuffer[UART_IND_PT + 2]));	//功率周期

		if (isUpdataNewData(power_a, power_t)) {
		    unsigned long powerData = getVIPvalue(power_a);
		    if (power_t > powerData) {
			if ((power_t - powerData) > (powerData >> 2)) {
			    resetVIPvalue(power_a, power_t);
			}
		    }
		}
		//printf("power:%d,%d,%d\r\n",power_a[0],power_a[1],power_a[2]);
		power_t = getVIPvalue(power_a);

		if (power_t == 0) {
		    power = 0;
		} else {
		    power = power_k * 100 / power_t;	//功率10mw值，避免溢出
		    power = power * 10;	//功率mw值
#if(SAMPLE_RESISTANCE_MR == 1)
		    //由于使用1mR的电阻，电流和功率需要不除以2
#elif(SAMPLE_RESISTANCE_MR == 2)
		    //由于使用2mR的电阻，电流和功率需要除以2
		    power >>= 1;
#endif
		}
		printf("Pk = %ld,Pt = %ld,P = %ld\r\n", power_k, power_t,
		       power);
	    }
	}

	if ((startFlag & 0xF4) == 0xF4) {	//电流异常
	    runingInf.electricity = 0;	//获取到的电流以0.01A为单位
	    electricity = 0;
	} else {
	    if ((inDataBuffer[UART_IND_FG] & 0x20) == 0x20) {
		electricity_k = ((inDataBuffer[UART_IND_IK] << 16) | (inDataBuffer[UART_IND_IK + 1] << 8) | (inDataBuffer[UART_IND_IK + 2]));	//电流系数
		electricity_t = ((inDataBuffer[UART_IND_IT] << 16) | (inDataBuffer[UART_IND_IT + 1] << 8) | (inDataBuffer[UART_IND_IT + 2]));	//电流周期

		if (isUpdataNewData(electricity_a, electricity_t)) {
		    updataVIPvalue(electricity_a, electricity_t);
		    electricityCnt = 0;
		} else {
		    electricityCnt++;
		    if (electricityCnt >= COUNT_NUM) {
			electricityCnt = 0;
			updataVIPvalue(electricity_a, electricity_t);
		    }
		}
		//printf("electricity:%d,%d,%d\r\n",electricity_a[0],electricity_a[1],electricity_a[2]);
		electricity_t = getVIPvalue(electricity_a);

		if (electricity_t == 0) {
		    electricity = 0;
		} else {
		    electricity = electricity_k * 100 / electricity_t;	//电流10mA值，避免溢出
		    electricity = electricity * 10;	//电流mA值
#if(SAMPLE_RESISTANCE_MR == 1)
		    //由于使用1mR的电阻，电流和功率需要不除以2
#elif(SAMPLE_RESISTANCE_MR == 2)
		    //由于使用2mR的电阻，电流和功率需要除以2
		    electricity >>= 1;
#endif
		}
		printf("Ik = %ld,It = %ld,I = %ld\r\n", electricity_k,
		       electricity_t, electricity);
	    } else {
		printf("%s(%d):I Flag Error\r\n", __func__, __LINE__);
	    }
	}

	if ((startFlag & 0xF8) == 0xF8) {	//电压异常
	    runingInf.voltage = 0;	//获取到的电压是以0.1V为单位
	    voltage = 0;
	} else {
	    if ((inDataBuffer[UART_IND_FG] & 0x40) == 0x40) {	//获取当前电压标致，为1时说明电压检测OK
		voltage_k = ((inDataBuffer[UART_IND_VK] << 16) | (inDataBuffer[UART_IND_VK + 1] << 8) | (inDataBuffer[UART_IND_VK + 2]));	//电压系数
		voltage_t = ((inDataBuffer[UART_IND_VT] << 16) | (inDataBuffer[UART_IND_VT + 1] << 8) | (inDataBuffer[UART_IND_VT + 2]));	//电压周期

		if (isUpdataNewData(voltage_a, voltage_t)) {
		    updataVIPvalue(voltage_a, voltage_t);
		    voltageCnt = 0;
		} else {
		    voltageCnt++;
		    if (voltageCnt >= COUNT_NUM) {
			voltageCnt = 0;
			updataVIPvalue(voltage_a, voltage_t);
		    }
		}
		//printf("voltage:%d,%d,%d\r\n",voltage_a[0],voltage_a[1],voltage_a[2]);
		voltage_t = getVIPvalue(voltage_a);

		if (voltage_t == 0) {
		    voltage = 0;
		} else {
		    voltage = voltage_k * 100 / voltage_t;	//电压10mV值，避免溢出
		    voltage = voltage * 10;	//电压mV值
		}
		printf("Vk = %ld,Vt = %ld,v = %ld\r\n", voltage_k,
		       voltage_t, voltage);
	    } else {
		printf("%s(%d):V Flag Error\r\n", __func__, __LINE__);
	    }
	}
	printf("0x%x:V = %ld;I = %ld;P = %ld;\r\n", startFlag, voltage,
	       electricity, power);
	break;
    }
    return 1;
}

void print_version(void)
{
    printf("#### wt_hlw8032 Version: 1.0.2 ####\n\n\n");
}

int set_parity(int *file, int databits, int stopbits, int parity)
{
    struct termios options;

    /* allow the process to receive SIGIO */
    fcntl(*file, F_SETOWN, getpid());

    /* Make the file descriptor asynchronous */
    fcntl(*file, F_SETFL, FASYNC);

    if (tcgetattr(*file, &options) != 0) {
	printf("Error:Cannot set serial %d!\n", __LINE__);
	return FALSE;
    }

    options.c_cflag = B4800 | CLOCAL | CREAD;
    switch (databits) {		/*设置数据位数 */
    case 7:
	options.c_cflag |= CS7;
	break;
    case 8:
	options.c_cflag |= CS8;
	break;
    default:
	printf("Error:Unsupported data size!\n");
	return FALSE;
    }

    switch (parity) {
    case 'n':
    case 'N':
	options.c_cflag &= ~PARENB;	/* Clear parity enable */
	options.c_iflag &= ~INPCK;	/* Enable parity checking */
	break;
    case 'o':
    case 'O':
	options.c_cflag |= (PARODD | PARENB);	/* 设置为奇效验 */
	options.c_iflag |= INPCK;	/* Disnable parity checking */
	break;
    case 'e':
    case 'E':
	options.c_cflag |= PARENB;	/* Enable parity */
	options.c_cflag &= ~PARODD;	/* 转换为偶效验 */
	options.c_iflag |= INPCK;	/* Disnable parity checking */
	break;
    case 'S':
    case 's':			/*as no parity */
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	break;
    default:
	printf("Error:Unsupported parity\n");
	return FALSE;
    }

    switch (stopbits) {
    case 1:
	options.c_cflag &= ~CSTOPB;
	break;
    case 2:
	options.c_cflag |= CSTOPB;
	break;
    default:
	printf("Error:Unsupported stop bits\n");
	return FALSE;
    }

    /* Set input parity option */
    if (parity != 'n')
	options.c_iflag |= INPCK;

    tcflush(*file, TCIFLUSH);
    options.c_cc[VTIME] = 0;	/* 设置超时 */
    options.c_cc[VMIN] = 24;	/* Update the options and do it NOW */

    options.c_iflag &= ~ICRNL;	//禁止将输入中的回车翻译为新行

    options.c_lflag = 0;
    options.c_oflag = 0;

    if (tcsetattr(*file, TCSANOW, &options) != 0) {
	printf("Error:Cannot set serial %d!\n", __LINE__);
	return FALSE;
    }

    return TRUE;
}

static inline void cpy(unsigned char *p_src, unsigned char *p_dest,
		       int size)
{
    for (int num = 0; num <= size - 1; num++) {
	p_dest[num] = p_src[num];
    }
}

static inline void signal_handler_IO(int status)
{
    wait_flag = FALSE;
}

/*  parameter：1.device fd 2.databits 3.stopbits 4.parity
 *  return: -EPERM 0
 *  func: 设置串口属性
 */
int set_serial(int *file, int databits, int stopbits, int parity)
{
    struct sigaction signal;

    /* install the signal handler before making the device asynchronous */
    signal.sa_handler = signal_handler_IO;
    signal.sa_flags = 0;
    signal.sa_restorer = NULL;
    sigaction(SIGIO, &signal, NULL);

    /* set new port settings */
    if (set_parity(file, databits, stopbits, parity) == FALSE) {
	printf("Set parity Error\n");
	return -EPERM;
    }
    return 0;
}

/*  parameter：1.device fd
 *  return: void
 *  func: deal_data (endless loop!)
 */
void deal_data(int *fd)
{
    int res = 0;
    unsigned char buf[48] = { 0 };

    printf("%s start!\n", __func__);

    while (signal_stop == FALSE) {
	sleep(1);		// test! sleep for test

	if (wait_flag == FALSE) {
	    res = read(*fd, buf, 48);
#ifdef DEBUG
	    printf("Read OK!!!%d \n", res);
#endif
	    for (int i = 0; i < DATA_PACKAGE; i++) {
		if (buf[i] == 0x5A)
		    cpy(buf, hlw_buff, DATA_PACKAGE);
	    }

#ifdef DEBUG
	    printf("Deal data......\n");
#endif
	    if (hlw_buff[1] == 0x5A)
		DealUartInf(hlw_buff, res);	//Deal

	    wait_flag = TRUE;	// 等待新的输入
	}
    }

    return;
}

int main(int argc, char **argv)
{
    int fd, res;

    print_version();

    fd = open("/dev/ttyS1", O_RDWR | O_NOCTTY);
    if (fd < 0) {
	printf("Error:%s %d\n", "/dev/ttyS1", -ENODEV);
	return -ENODEV;
    }

    res = set_serial(&fd, 8, 1, 'E');
    if (res != 0) {
	printf("Error:set_serial %d\n", -EPERM);
	return -EPERM;
    }

    deal_data(&fd);		// loop

    printf("Close...\n");
    close(fd);

    return 0;
}
