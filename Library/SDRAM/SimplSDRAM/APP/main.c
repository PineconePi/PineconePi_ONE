#include "SWM320.h"

uint32_t WrBuff[] = {0x1C1C1C1C, 0x1D1D1D1D,
				       0x1E1E1E1E, 0x1F1F1F1F, 0x20202020, 0x21212121, 0x22222222, 0x23232323, 0x24242424, 0x25252525, 0x26262626, 0x28282828};

void SerialInit(void);

int main(void)
{		
	uint32_t i;
	SDRAM_InitStructure SDRAM_InitStruct;
		
	SystemInit();
		
	SerialInit();
	
	PORT->PORTP_SEL0 = 0xAAAAAAAA;		//PP0-23 => ADDR0-23
	PORT->PORTP_SEL1 = 0x0000AAAA;
	
	PORT->PORTM_SEL0 = 0xAAAAAAAA;		//PM0-15 => DATA15-0
	PORT->PORTM_INEN = 0xFFFF;
	
	PORT->PORTM_SEL1 = 0xAAA;			//PM16 => OEN、PM17 => WEN、PM18 => NORFL_CSN、PM19 => SDRAM_CSN、PM20 => SRAM_CSN、PM21 => SDRAM_CKE
	
	SDRAM_InitStruct.DataWidth = SDRAM_DATAWIDTH_16;
	SDRAM_InitStruct.CellSize = SDRAM_CELLSIZE_256Mb;
	SDRAM_InitStruct.CellBank = SDRAM_CELLBANK_4;
	SDRAM_InitStruct.CellWidth = SDRAM_CELLWIDTH_16;
	SDRAM_InitStruct.TimeTMRD = SDRAM_TMRD_3;
	SDRAM_InitStruct.TimeTRRD = SDRAM_TRRD_2;
	SDRAM_InitStruct.TimeTRAS = SDRAM_TRAS_6;
	SDRAM_InitStruct.TimeTRC  = SDRAM_TRC_8;
	SDRAM_InitStruct.TimeTRCD = SDRAM_TRCD_7;
	SDRAM_InitStruct.TimeTRP  = SDRAM_TRP_7;
	SDRAM_Init(&SDRAM_InitStruct);
	
	for(i = 0; i < 20; i++)  *((volatile uint32_t *) (SDRAMM_BASE + i*4)) = 0x00000000;		// 只支持字操作
	
	printf("\r\nAfter Erase: \r\n");
	for(i = 0; i < 20; i++)	printf("0x%08X, ",  *((volatile uint32_t *) (SDRAMM_BASE + i*4)));
	
	for(i = 0; i < 20; i++)  *((volatile uint32_t *) (SDRAMM_BASE + i*4)) = WrBuff[i];
	
	printf("\r\nAfter Write: \r\n");
	for(i = 0; i < 20; i++)	printf("0x%08X, ",  *((volatile uint32_t *) (SDRAMM_BASE + i*4)));
	
	while(1==1)
	{
	}
}


void SerialInit(void)
{
	UART_InitStructure UART_initStruct;
	
	PORT_Init(PORTA, PIN2, FUNMUX0_UART0_RXD, 1);	//GPIOA.2配置为UART0输入引脚
	PORT_Init(PORTA, PIN3, FUNMUX1_UART0_TXD, 0);	//GPIOA.3配置为UART0输出引脚
 	
 	UART_initStruct.Baudrate = 9600;
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
