#include "SWM320.h"

void SerialInit(void);
void RTC_Config(void);

int main(void)
{	
	uint32_t i;
	
	SystemInit();
	
	SerialInit();
	
	SYS->CLKEN |= (1 << SYS_CLKEN_RTCBKP_Pos);		//唤醒必须开启此时钟
	
	SYS->LRCCR &= ~(1 << SYS_LRCCR_OFF_Pos);		//开启32K低频振荡器
	
	GPIO_Init(GPIOA, PIN5, 1, 0, 0);				//输出， 接LED
	
	RTC_Config();
	SYS->RTCWKEN |= 1;								//开启RTC唤醒
	
	while(1==1)
	{
		GPIO_SetBit(GPIOA, PIN5);					//点亮LED
		for(i = 0; i < 10000000; i++);
		GPIO_ClrBit(GPIOA, PIN5);					//熄灭LED
		
		switchCLK_32KHz();
		
		SYS->HRCCR = (1 << SYS_HRCCR_OFF_Pos);		//关闭HRC、降低功耗
		
		SYS->SLEEP |= (1 << SYS_SLEEP_SLEEP_Pos);	//进入睡眠模式
	
		for(i = 0; i < 1000; i++);
		
		switchCLK_20MHz();
	}
}


RTC_DateTime dateTime;
RTC_AlarmStructure alarmStruct;

void RTC_Config(void)
{
	RTC_InitStructure RTC_initStruct;
	
	RTC_initStruct.Year = 2018;
	RTC_initStruct.Month = 3;
	RTC_initStruct.Date = 23;
	RTC_initStruct.Hour = 10;
	RTC_initStruct.Minute = 5;
	RTC_initStruct.Second = 5;
	RTC_initStruct.SecondIEn = 0;
	RTC_initStruct.MinuteIEn = 0;
	RTC_Init(RTC, &RTC_initStruct);
	
	RTC_Start(RTC);
	
	alarmStruct.Days = RTC_SUN | RTC_MON | RTC_TUE | RTC_WED | RTC_THU | RTC_FRI | RTC_SAT;
	alarmStruct.Hour = 10;
	alarmStruct.Minute = 5;
	alarmStruct.Second = 10;
	alarmStruct.AlarmIEn = 1;
	
	RTC_AlarmSetup(RTC, &alarmStruct);
}

void RTC_Handler(void)
{
	if(RTC_IntAlarmStat(RTC))
	{
		RTC_IntAlarmClr(RTC);
		
		RTC_GetDateTime(RTC, &dateTime);
		printf("Now Time: %02d : %02d\r\n", dateTime.Minute, dateTime.Second);
		
		if(dateTime.Second >= 55)
		{
			alarmStruct.Second = 0;
			alarmStruct.Minute = dateTime.Minute + 1;
		}
		else
		{
			alarmStruct.Second = dateTime.Second + 5;
			alarmStruct.Minute = dateTime.Minute;
		}
		RTC_AlarmSetup(RTC, &alarmStruct);
	}
}


void SerialInit(void)
{
	UART_InitStructure UART_initStruct;
	
	PORT_Init(PORTA, PIN2, FUNMUX0_UART0_RXD, 1);	//GPIOA.2配置为UART0输入引脚
	PORT_Init(PORTA, PIN3, FUNMUX1_UART0_TXD, 0);	//GPIOA.3配置为UART0输出引脚
 	
 	UART_initStruct.Baudrate = 57600;
	UART_initStruct.DataBits = UART_DATA_8BIT;
	UART_initStruct.Parity = UART_PARITY_NONE;
	UART_initStruct.StopBits = UART_STOP_1BIT;
	UART_initStruct.RXThresholdIEn = 0;
	UART_initStruct.TXThresholdIEn = 0;
	UART_initStruct.TimeoutIEn = 0;
 	UART_Init(UART0, &UART_initStruct);
	UART_Open(UART0);
}

/****************************************************************************************************************************************** 
* 函数名称: fputc()
* 功能说明: printf()使用此函数完成实际的串口打印动作
* 输    入: int ch		要打印的字符
*			FILE *f		文件句柄
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
int fputc(int ch, FILE *f)
{
	UART_WriteByte(UART0, ch);
	
	while(UART_IsTXBusy(UART0));
 	
	return ch;
}
