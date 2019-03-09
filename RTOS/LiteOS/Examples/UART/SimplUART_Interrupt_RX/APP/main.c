#include "SWM320.h"

#include <string.h>

void SerialInit(void);

#define UART_RX_LEN	 128

uint32_t UART_GetChars(char *data);

int main(void)
{
	uint32_t len, i;
	char buffer[UART_RX_LEN] = {0};
	
	SystemInit();
	
	SerialInit();
   	
	while(1==1)
	{
		if((len = UART_GetChars(buffer)) != 0)
		{
			for(i = 0; i < len; i++)
				printf("%c", buffer[i]);
		}
	}
}

char UART_RXBuffer[UART_RX_LEN] = {0};
uint32_t UART_RXIndex = 0;

uint32_t UART_GetChars(char *data)
{
	uint32_t len = 0;
	
	if(UART_RXIndex != 0)
	{
		NVIC_DisableIRQ(UART0_IRQn);		//从UART_RXBuffer读取数据过程中要关闭中断，防止读写混乱
		memcpy(data, UART_RXBuffer, UART_RX_LEN);
		len = UART_RXIndex;
		UART_RXIndex = 0;
		NVIC_EnableIRQ(UART0_IRQn);
	}
	
	return len;
}

void UART0_Handler(void)
{
	uint32_t chr;
	
	if(UART_INTRXThresholdStat(UART0) || UART_INTTimeoutStat(UART0))
	{
		while(UART_IsRXFIFOEmpty(UART0) == 0)
		{
			if(UART_ReadByte(UART0, &chr) == 0)
			{
				if(UART_RXIndex < UART_RX_LEN)
				{
					UART_RXBuffer[UART_RXIndex] = chr;
					
					UART_RXIndex++;
				}
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
	UART_initStruct.RXThreshold = 3;
	UART_initStruct.RXThresholdIEn = 1;
	UART_initStruct.TXThresholdIEn = 0;
	UART_initStruct.TimeoutTime = 10;		//10个字符时间内未接收到新的数据则触发超时中断
	UART_initStruct.TimeoutIEn = 1;
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
