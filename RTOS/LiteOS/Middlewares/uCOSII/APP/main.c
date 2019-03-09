#include "SWM320.h"

#include "ucos_ii.h"

static  OS_STK TaskLEDStk[TASK_LED_STK_SIZE];
static  OS_STK TaskCOMStk[TASK_COM_STK_SIZE];

void TaskLED(void *arg);
void TaskCOM(void *arg);

int main(void)
{ 	
 	SystemInit();
	
	NVIC_SetPriority(SysTick_IRQn, OS_CPU_CFG_SYSTICK_PRIO);
	SysTick_Config(SystemCoreClock / OS_TICKS_PER_SEC);
	
	OSInit();
	
	OSTaskCreate((void (*)(void *))TaskLED, (void *)0, &TaskLEDStk[TASK_LED_STK_SIZE - 1], TASK_LED_PRIO);
	OSTaskCreate((void (*)(void *))TaskCOM, (void *)0, &TaskCOMStk[TASK_COM_STK_SIZE - 1], TASK_COM_PRIO);
	
	OSStart();
}

/****************************************************************************************************************************************** 
* 函数名称:	TaskLED()
* 功能说明: LED灯闪烁任务
* 输    入: void *arg
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void TaskLED(void *arg)
{
	GPIO_Init(GPIOA, PIN4, 1, 0, 0);	//GPIOA.4配置为输出引脚，推挽输出
	GPIO_Init(GPIOA, PIN5, 1, 0, 0);	//GPIOA.5配置为输出引脚，推挽输出
	GPIO_ClrBit(GPIOA, PIN4);
 	GPIO_SetBit(GPIOA, PIN5);
	
	while(1)
	{
		GPIO_InvBit(GPIOA, PIN4);
 		GPIO_InvBit(GPIOA, PIN5);
		
		OSTimeDly(OS_TICKS_PER_SEC);
	}
}

void SerialInit(void);
/****************************************************************************************************************************************** 
* 函数名称:	TaskCOM()
* 功能说明: 串口打印任务
* 输    入: void *arg
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void TaskCOM(void *arg)
{
	SerialInit();
	
	while(1)
	{
		printf("Hi, World!\r\n");
		
		OSTimeDly(OS_TICKS_PER_SEC);
	}
}


void SerialInit(void)
{
	UART_InitStructure UART_initStruct;
	
	PORT_Init(PORTA, PIN2, FUNMUX0_UART0_RXD, 1);	//GPIOA.2配置为UART0输入引脚
	PORT_Init(PORTA, PIN3, FUNMUX1_UART0_TXD, 0);	//GPIOA.3配置为UART0输出引脚
 	
 	UART_initStruct.Baudrate = 57600;
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
 	while(UART_IsTXFIFOFull(UART0));
	
	UART_WriteByte(UART0, ch);
 	
	return ch;
}
