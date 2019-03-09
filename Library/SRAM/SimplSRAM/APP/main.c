#include "SWM320.h"

uint8_t EXRAM_Buffer[1024] __attribute__((at(SRAMM_BASE))) = {0};

void SerialInit(void);

int main(void)
{		
	uint32_t i;
	SRAM_InitStructure SRAM_InitStruct;
	
	SystemInit();
	
	SerialInit();
	
	PORT->PORTP_SEL0 = 0xAAAAAAAA;		//PP0-23 => ADDR0-23
	PORT->PORTP_SEL1 = 0xAAAA;
	
	PORT->PORTM_SEL0 = 0xAAAAAAAA;		//PM0-15 => DATA15-0
	PORT->PORTM_INEN = 0xFFFF;
	
	PORT->PORTM_SEL1 = 0xAAA;			//PM16 => OEN、PM17 => WEN、PM18 => NORFL_CSN、PM19 => SDRAM_CSN、PM20 => SRAM_CSN、PM21 => SDRAM_CKE
	
	SRAM_InitStruct.ClkDiv = SRAM_CLKDIV_8;
	SRAM_InitStruct.DataWidth = SRAM_DATAWIDTH_16;
	SRAM_Init(&SRAM_InitStruct);
	
	while(1==1)
	{
		for(i = 0; i < 32; i++)
		{
			EXRAM_Buffer[i] = i;
		}
		printf("EXRAM_Buffer:\r\n");
		for(i = 0; i < 32; i++)
		{
			printf("%02X, ", EXRAM_Buffer[i]);
		}
		printf("\r\n\r\n");
		
		*((volatile uint32_t *) (SRAMM_BASE + 0x00)) = 0x12345678;
		*((volatile uint32_t *) (SRAMM_BASE + 0x04)) = 0x87654321;
		if(*((volatile uint32_t *) (SRAMM_BASE + 0x00)) == 0x12345678)
		{
			printf("SRAM Word Read and Write Success!\r\n");
		}
		else
		{
			printf("SRAM Word Read and Write Failure!\r\n");
		}
		printf("\r\n\r\n\r\n");
		
		for(i = 0; i < SystemCoreClock/4; i++) __NOP();
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
