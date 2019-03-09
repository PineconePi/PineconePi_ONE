#include "SWM320.h"

void SerialInit(void);

int main(void)
{	
	uint32_t n, i;
	uint32_t rxdata, txdata = 0x23;
	SPI_InitStructure SPI_initStruct;
	
	SystemInit();
	
	SerialInit();
	
	GPIO_Init(GPIOA, PIN4, 1, 0, 0);
#define SPI1_CS_Low()	GPIO_ClrBit(GPIOA, PIN4)
#define SPI1_CS_High()	GPIO_SetBit(GPIOA, PIN4)
	
	PORT_Init(PORTA, PIN5, FUNMUX1_SPI1_SCLK, 0);
	PORT_Init(PORTA, PIN6, FUNMUX0_SPI1_MOSI, 0);
	PORT_Init(PORTA, PIN7, FUNMUX1_SPI1_MISO, 1);
	
	SPI_initStruct.clkDiv = SPI_CLKDIV_32;
	SPI_initStruct.FrameFormat = SPI_FORMAT_SPI;
	SPI_initStruct.SampleEdge = SPI_SECOND_EDGE;
	SPI_initStruct.IdleLevel = SPI_HIGH_LEVEL;
	SPI_initStruct.WordSize = 8;
	SPI_initStruct.Master = 1;
	SPI_initStruct.RXHFullIEn = 0;
	SPI_initStruct.TXEmptyIEn = 0;
	SPI_initStruct.TXCompleteIEn = 0;
	SPI_Init(SPI1, &SPI_initStruct);
	SPI_Open(SPI1);
	
	while(1==1)
	{
		SPI1_CS_Low();
		for(n = 0; n < 10; n++)
		{
			rxdata = SPI_ReadWrite(SPI1, txdata);
			txdata = rxdata + 1;		//将SPI0_MOSI与SPI0_MISO短接，则可看到打印数值加一递增
			
			printf("rxdata: 0x%X\r\n", rxdata);
			
			for(i = 0; i < SystemCoreClock/50; i++);
		}
		SPI1_CS_High();
		
		for(i = 0; i < SystemCoreClock/10; i++);
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
