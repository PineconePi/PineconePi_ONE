#include "SWM320.h"

CAN_RXMessage CAN_RXMsg;

void SerialInit(void);

uint32_t sameBits(uint32_t std_id[], uint32_t std_n, uint32_t ext_id[], uint32_t ext_n);

int main(void)
{
	uint32_t i;
	CAN_InitStructure CAN_initStruct;
	
	SystemInit();
	
	SerialInit();
	
	PORT_Init(PORTA, PIN4, FUNMUX0_CAN_RX, 1);	//GPIOA.4配置为CAN输入引脚
	PORT_Init(PORTA, PIN5, FUNMUX1_CAN_TX, 0);	//GPIOA.5配置为CAN输出引脚
	
	CAN_initStruct.Mode = CAN_MODE_NORMAL;
	CAN_initStruct.CAN_BS1 = CAN_BS1_5tq;
	CAN_initStruct.CAN_BS2 = CAN_BS2_4tq;
	CAN_initStruct.CAN_SJW = CAN_SJW_2tq;
	CAN_initStruct.Baudrate = 100000;
	CAN_initStruct.FilterMode = CAN_FILTER_32b;
   	switch(5)
	{
	case 0:		// 接收ID为任意值的帧
		CAN_initStruct.FilterMask32b  = 0xFFFFFFFF;				// 0 must match    1 don't care
		CAN_initStruct.FilterCheck32b = 0xFFFFFFFF;
		break;
	
	case 1:		// 接收ID为0x122的标准帧
		CAN_initStruct.FilterMask32b = ~(0x7FFu << 21);
		CAN_initStruct.FilterCheck32b = (0x122  << 21);			// 标准帧ID在第21位到31位，详见手册
		break;
	
	case 2:		// 接收ID为0x122的扩展帧
		CAN_initStruct.FilterMask32b = ~(0x1FFFFFFFu << 3);
		CAN_initStruct.FilterCheck32b = (0x00000122  << 3);		// 扩展帧ID在第 3位到31位，详见手册
		break;
	
	case 3:		// 接收ID为0x12X的标准帧，X表示ID的低4位是什么值无所谓
		CAN_initStruct.FilterMask32b = ~(0x7F0u << 21);
		CAN_initStruct.FilterCheck32b = (0x122  << 21);
		break;
	
	case 4:		// 接收ID为0x122和0x101的标准帧
		CAN_initStruct.FilterMode = CAN_FILTER_16b;
		CAN_initStruct.FilterMask16b1 = (uint16_t)~(0x7FFu << 5);
		CAN_initStruct.FilterCheck16b1 = (0x122  << 5);
		CAN_initStruct.FilterMask16b2 = (uint16_t)~(0x7FFu << 5);
		CAN_initStruct.FilterCheck16b2 = (0x101  << 5);
		break;
	
	case 5:		// 即接收ID为0x122、0x235、0x450的标准帧，也接收ID为0x101, 0x235, 0x1780的扩展帧
		{		// 并不是只接收ID为这6个值的帧，而是接收尽量最少的帧、且能保证ID为要求值的帧都能接收到
		uint32_t stdID[] = {0x122, 0x235, 0x450};
		uint32_t extID[] = {0x101, 0x235, 0x1780};
		
		CAN_initStruct.FilterMask32b = ~(sameBits(stdID, 3, extID, 3) << 3);
		CAN_initStruct.FilterCheck32b = (extID[0]  << 3);
		// 或者：
// 		CAN_initStruct.FilterCheck32b = (stdID[0]  << 21);
		}
		break;
	}
	CAN_initStruct.RXNotEmptyIEn = 1;
	CAN_initStruct.RXOverflowIEn = 0;
	CAN_initStruct.ArbitrLostIEn = 0;
	CAN_initStruct.ErrPassiveIEn = 0;
	CAN_Init(CAN, &CAN_initStruct);
	
	CAN_Open(CAN);
	
	printf("Hi from Synwit\r\n");
	
	while(1==1)
	{
		if(CAN_RXMsg.size > 0)
		{
			printf("\r\nReceive %s: %08X, ", CAN_RXMsg.format == CAN_FRAME_STD ? "STD" : "EXT", CAN_RXMsg.id);
			for(i = 0; i < CAN_RXMsg.size; i++) printf("%02X, ", CAN_RXMsg.data[i]);
			
			CAN_RXMsg.size = 0;
		}
		else if(CAN_RXMsg.remote == 1)	//远程帧
		{
			printf("\r\nReceive %s Remote Request", CAN_RXMsg.format == CAN_FRAME_STD ? "STD" : "EXT");
			
			CAN_RXMsg.remote = 0;
		}
	}
}

void CAN_Handler(void)
{
	if(CAN_INTRXNotEmptyStat(CAN) != 0)
	{
		CAN_Receive(CAN, &CAN_RXMsg);
	}
}

/****************************************************************************************************************************************** 
* 函数名称: sameBits()
* 功能说明: 找到所有ID中相同位置值相同的位，比如所有ID的第10位都是0（或1）则返回值的第10位为1
* 输    入: 
* 输    出: 
* 注意事项: 无
******************************************************************************************************************************************/
uint32_t sameBits(uint32_t std_id[], uint32_t std_n, uint32_t ext_id[], uint32_t ext_n)
{
	uint32_t i, j;

	uint32_t mask = 0;
	
	for(i = 0; i < std_n; i++)
		std_id[i] = std_id[i] << 18;
	
	for(i = 0; i < std_n-1; i++)
	{
		for(j = i+1; j < std_n; j++)
		{
			mask |= std_id[i] ^ std_id[j];
		}
	}
	
	for(i = 0; i < ext_n-1; i++)
	{
		for(j = i+1; j < ext_n; j++)
		{
			mask |= ext_id[i] ^ ext_id[j];
		}
	}
	
	return ~mask;
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
