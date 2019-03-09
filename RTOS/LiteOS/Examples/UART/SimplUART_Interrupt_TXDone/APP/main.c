#include "SWM320.h"

#include <string.h>

void SerialInit(void);

void UART_SendChars(char data[], uint32_t len);

int main(void)
{
	uint32_t i;
	char str_Hi[] = "Hi from Synwit\r\n";
	
	SystemInit();
	
	SerialInit();
   	
	while(1==1)
	{
		UART_SendChars(str_Hi, strlen(str_Hi));
				
		for(i = 0; i < 10000000; i++);
	}
}

char * UART_TXBuffer = 0;
uint32_t UART_TXCount = 0,
         UART_TXIndex = 0;

void UART_SendChars(char data[], uint32_t len)
{
	UART_TXBuffer = data;
	UART_TXCount = len;
	UART_TXIndex = 0;
	
	UART_WriteByte(UART0, UART_TXBuffer[UART_TXIndex++]);
	
	UART_INTTXDoneEn(UART0);
}

void UART0_Handler(void)
{
	if(UART_INTTXDoneStat(UART0))
	{
		while(UART_IsTXFIFOFull(UART0) == 0)
		{
			if(UART_TXIndex < UART_TXCount)
			{
				UART_WriteByte(UART0, UART_TXBuffer[UART_TXIndex]);
				
				UART_TXIndex++;
			}
			else
			{
				UART_INTTXDoneDis(UART0);
				
				break;
			}
		}
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
	UART_initStruct.TXThreshold = 3;
	UART_initStruct.TXThresholdIEn = 0;
	UART_initStruct.TimeoutIEn = 0;
 	UART_Init(UART0, &UART_initStruct);
	UART_Open(UART0);
	
	NVIC_EnableIRQ(UART0_IRQn);
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
