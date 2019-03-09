#include "SWM320.h"

void SerialInit(void);

uint8_t EXRAM_Buffer[500] = {0};	// 在 Options for File 'main.c' 中已设定main中定义未初始化全局变量定位到片外SRAM地址空间

int main(void)
{		
	uint32_t i;
	
	SystemInit();
	
	SerialInit();
	
	PORT->PORTP_SEL0 = 0xAAAAAAAA;		//PP0-23 => ADDR0-23
	PORT->PORTP_SEL1 = 0xAAAA;
	
	PORT->PORTM_SEL0 = 0xAAAAAAAA;		//PM0-15 => DATA15-0
	PORT->PORTM_INEN = 0xFFFF;
	
	PORT->PORTM_SEL1 = 0xAAA;			//PM16 => OEN、PM17 => WEN、PM18 => NORFL_CSN、PM19 => SDRAM_CSN、PM20 => SRAM_CSN、PM21 => SDRAM_CKE
	
	
	// 配置SRAM前需要刷新下SDRAM控制器
	do {
		SYS->CLKEN |=  (1 << SYS_CLKEN_SDRAM_Pos);
		
		while(SDRAMC->REFDONE == 0);
		SDRAMC->REFRESH &= ~(1 << SDRAMC_REFRESH_EN_Pos);
		
		for(i = 0; i < 1000; i++) __NOP();
		SYS->CLKEN &= ~(1 << SYS_CLKEN_SDRAM_Pos);
	} while(0);
	
	
	SYS->CLKEN |= (1 << SYS_CLKEN_RAMC_Pos);
	for(i = 0; i < 10; i++) __NOP();
	
	SRAMC->CR = (9 << SRAMC_CR_RWTIME_Pos) |
				(0 << SRAMC_CR_BYTEIF_Pos) |	// 16位接口
			    (0 << SRAMC_CR_HBLBDIS_Pos);	// 使能字节、半字访问
	
	for(i = 0; i < 500; i++)
	{
		EXRAM_Buffer[i] = i;
	}

	while(1==1)
	{
		*((volatile uint32_t *) (SRAMM_BASE + 0x00)) = 0x12345678;
		*((volatile uint32_t *) (SRAMM_BASE + 0x04)) = 0x87654321;
		if(*((volatile uint32_t *) (SRAMM_BASE + 0x00)) == 0x12345678)
		{
			printf("SRAM Read and Write Success!\r\n");
		}
		else
		{
			printf("SRAM Read and Write Failure!\r\n");
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
