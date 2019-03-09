#include "SWM320.h"

#include <RTL.h>

/* 注意：
*	为了简单，在RX_Conf_CM.c中配置所有Task都运行于特权等级，否则当Task中
* 代码写某些内核寄存器（如调用NVIC_DisableIRQ())时就会触发HardFault
************************************************************************/

OS_TID idLED, idCOM;

__task void TaskINI(void);
__task void TaskLED(void);
__task void TaskCOM(void);

int main(void)
{ 	
 	SystemInit();
	
	os_sys_init(TaskINI);
}

__task void TaskINI(void)
{
	idLED = os_tsk_create(TaskLED, 1);
	idCOM = os_tsk_create(TaskCOM, 2);
	
	os_tsk_delete(os_tsk_self());
}

/****************************************************************************************************************************************** 
* 函数名称:	TaskLED()
* 功能说明: LED灯闪烁任务
* 输    入: 无
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
__task void TaskLED(void)
{
	GPIO_Init(GPIOA, PIN4, 1, 0, 0);	//GPIOA.4配置为输出引脚，推挽输出
	GPIO_Init(GPIOA, PIN5, 1, 0, 0);	//GPIOA.5配置为输出引脚，推挽输出
	GPIO_ClrBit(GPIOA, PIN4);
 	GPIO_SetBit(GPIOA, PIN5);
	
	while(1)
	{
		GPIO_InvBit(GPIOA, PIN4);
 		GPIO_InvBit(GPIOA, PIN5);
		
		os_dly_wait(50);
	}
}

void SerialInit(void);
/****************************************************************************************************************************************** 
* 函数名称:	TaskCOM()
* 功能说明: 串口打印任务
* 输    入: 无
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
__task void TaskCOM(void)
{
	SerialInit();
	
	while(1)
	{
		printf("Hi, World!\r\n");
		
		os_dly_wait(50);
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
