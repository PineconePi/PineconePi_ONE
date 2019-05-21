#include "SWM320.h"
uint32_t WrBuff[48] = {0xAAAAAAAA,0x02020202,0x03030303,0x04040404,0x05050505,0x06060606,0x07070707,0x08080808,
											 0x11111111,0x12121212,0x13131313,0x14141414,0x15151515,0x16161616,0x17171717,0x18181818,
											 0x19191919,0x20202020,0x21212121,0x22222222,0x23232323,0x24242424,0x25252525,0x26262626,
											 0x27272727,0x28282828,0x29292929,0x30303030,0x31313131,0x32323232,0x33333333,0x34343434,
											 0x35353535,0x36363636,0x37373737,0x38383838,0x39393939,0x40404040,0x41414141,0x42424242,
                       0x43434343,0x44444444,0x45454545,0x46464646,0x47474747,0x48484848,0x49494949,0x50505050};

void SerialInit(void);
//sdram 一个地址存储8位；
//#define Interval_address 32//间隔32个地址也就是256bit的数据进行写入
int main(void)
{		
	uint32_t i=0;
//uint32_t j=1;
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
	SDRAM_InitStruct.TimeTRP  = SDRAM_TRP_3;
	SDRAM_Init(&SDRAM_InitStruct);

		for(i = 0; i < 48; i++) *((volatile uint32_t *) (SDRAMM_BASE + i*4))= 0x00000000; 	// 只支持字操作
	printf("\r\nAfter Erase: \r\n");
	for(i = 0; i < 48; i++)	 printf("0x%08X, ",  *((volatile uint32_t *) (SDRAMM_BASE + i*4)));
	for(i = 0; i < 48; i++)  *((volatile uint32_t *) (SDRAMM_BASE + i*4)) = WrBuff[i];
	printf("\r\nAfter Write: \r\n");
	for(i = 0; i < 48; i++)
	{
	printf("0x%08X, ",  *((volatile uint32_t *) (SDRAMM_BASE +i*4)));  
	if((i<=7)&&(i%7)==0)
	{
	printf("\r\n");
	}
	if((i>7)&&((i-7)%8)==0)
	{
	printf("\r\n");
	}
	}
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

/********************* Write data routines at 256 bits intervals|间隔256bit写入数据例程 *************************/
/*	
	for(i = 0; i < 48; i++){ if((i+1)%8==0){j++;}*((volatile uint32_t *) (SDRAMM_BASE + i*4+Interval_address*j))= 0x00000000; }	// 只支持字操作
	
	printf("\r\nAfter Erase: \r\n");
	j=1;
	for(i = 0; i < 48; i++)	{if((i+1)%8==0){j++;} printf("0x%08X, ",  *((volatile uint32_t *) (SDRAMM_BASE + i*4+Interval_address*j)));}
	j=1;
	for(i = 0; i < 48; i++) { if((i+1)%8==0){j++;} *((volatile uint32_t *) (SDRAMM_BASE + i*4+Interval_address*j)) = WrBuff[i];}
	
	printf("\r\nAfter Write: \r\n");
	j=1;
	for(i = 0; i < 48; i++)
	{
		if((i+1)%8==0)
		{
			j++;
		}
	printf("0x%08X, ",  *((volatile uint32_t *) (SDRAMM_BASE +i*4+Interval_address*j)));  
	if((i<=7)&&(i%7)==0)
	{
	printf("\r\n");
	}
	if((i>7)&&((i-7)%8)==0)
	{
	printf("\r\n");
	}
	}
	
	*/
