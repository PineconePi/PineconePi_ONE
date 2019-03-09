#include "SWM320.h"

#include "GD25Q21.h"

#define EEPROM_ADDR	  0x8000

uint8_t WrBuff[20] = {0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,
				      0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
uint8_t RdBuff[20] = {0};

void SerialInit(void);

int main(void)
{	
	uint32_t i;
	
	SystemInit();
	
	SerialInit();
	
	GD25Q21_Init();
	
	GD25Q21_EraseSector(EEPROM_ADDR);
 	while(GD25Q21_IsBusy()) __NOP();	// 等待擦除操作完成
	
	GD25Q21_ReadData(EEPROM_ADDR, RdBuff, 20);
	
	printf("\r\nAfter Erase: \r\n");
	for(i = 0; i < 20; i++) printf("0x%X, ", RdBuff[i]);
	
	GD25Q21_WritePage(EEPROM_ADDR, WrBuff, 20);
 	while(GD25Q21_IsBusy()) __NOP();	// 等待写入操作完成
	
	GD25Q21_ReadData(EEPROM_ADDR, RdBuff, 20);
	
	printf("\r\nAfter Write: \r\n");
	for(i = 0; i < 20; i++) printf("0x%X, ", RdBuff[i]);
	
	while(1==1)
	{
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
