#include "SWM320.h"

#define NOR_TEST_ADDR  0x10000

uint16_t WrBuff[20] = {0x1414, 0x1515, 0x1616, 0x1717, 0x1818, 0x1919, 0x1A1A, 0x1B1B, 0x1C1C, 0x1D1D,
				       0x1E1E, 0x1F1F, 0x2020, 0x2121, 0x2222, 0x2323, 0x2424, 0x2525, 0x2626, 0x2727};

void SerialInit(void);

int main(void)
{		
	uint32_t i;
	NORFL_InitStructure NORFL_InitStruct;
		
	SystemInit();
		
	SerialInit();
	
	PORT->PORTP_SEL0 = 0xAAAAAAAA;		//PP0-23 => ADDR0-23
	PORT->PORTP_SEL1 = 0x0000AAAA;
	
	PORT->PORTM_SEL0 = 0xAAAAAAAA;		//PM0-15 => DATA15-0
	PORT->PORTM_INEN = 0xFFFF;
	
	PORT->PORTM_SEL1 = 0xAAA;			//PM16 => OEN、PM17 => WEN、PM18 => NORFL_CSN、PM19 => SDRAM_CSN、PM20 => SRAM_CSN、PM21 => SDRAM_CKE
	
	NORFL_InitStruct.DataWidth = 16;
	NORFL_InitStruct.WELowPulseTime = 5;
	NORFL_InitStruct.OEPreValidTime = 12;
	NORFL_InitStruct.OperFinishIEn  = 0;
	NORFL_InitStruct.OperTimeoutIEn = 0;
	NORFL_Init(&NORFL_InitStruct);
		
	NORFL_SectorErase(NOR_TEST_ADDR);
	
	printf("\r\nAfter Erase: \r\n");
	for(i = 0; i < 20; i++)	printf("0x%04X, ",  NORFL_Read(NOR_TEST_ADDR + i*2));
	
	for(i = 0; i < 20; i++) NORFL_Write(NOR_TEST_ADDR + i*2, WrBuff[i]);	// 数据连线16位，半字写入
	
	printf("\r\nAfter Write: \r\n");
	for(i = 0; i < 20; i++)	printf("0x%04X, ",  NORFL_Read(NOR_TEST_ADDR + i*2));
	
	printf("\r\nDirect Read: \r\n");
	for(i = 0; i < 10; i++) printf("0x%08X, ", *(((volatile uint32_t *) (NORFLM_BASE + NOR_TEST_ADDR)) + i));		// 直接读只支持字读
	
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
