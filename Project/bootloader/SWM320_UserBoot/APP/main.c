#include "SWM320.h"

void SerialInit(void);
void JumpToApp(uint32_t addr);

int main(void)
{ 	
	uint32_t i, j;
	
	__enable_irq();
	
 	SystemInit();
	
	SerialInit();
	
	GPIO_Init(GPIOA, PIN4, 0, 1, 0);	// ���룬������
	GPIO_Init(GPIOA, PIN5, 1, 0, 0);	// ���
		
	if(GPIO_GetBit(GPIOA, PIN4) == 0)		// ��⵽�ض��źţ����߼�⵽APP����������
	{
		for(i = 0; i < 10; i++)				// ģ��UserBoot����APP����Ĺ���
		{
			GPIO_InvBit(GPIOA, PIN5);
			
			printf("Hi from UserBoot");
			
			for(j = 0; j < SystemCoreClock/24; j++) __NOP();
		}
	}
	
	/* ��ת��APPǰ��Ҫ��UserBootʹ�õ�����ص�����λ��*/
	GPIO_Init(GPIOA, PIN5, 0, 0, 0);
	
	JumpToApp(0x40000);
	
 	while(1==1)
 	{
 	}
}


void JumpToApp(uint32_t addr)
{	
	__disable_irq();
	
	uint32_t sp = *((volatile uint32_t *)(addr));
	uint32_t pc = *((volatile uint32_t *)(addr + 4));
	
	typedef void (*Func_void_void)(void);
	Func_void_void ResetHandler = (Func_void_void)pc;
	
	SCB->VTOR = addr;
	
	__set_MSP(sp);
	
	ResetHandler();
	
	while(1) __NOP();
}


void SerialInit(void)
{
	UART_InitStructure UART_initStruct;
	
	PORT_Init(PORTA, PIN2, FUNMUX0_UART0_RXD, 1);	//GPIOA.2����ΪUART0��������
	PORT_Init(PORTA, PIN3, FUNMUX1_UART0_TXD, 0);	//GPIOA.3����ΪUART0�������
 	
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
* ��������: fputc()
* ����˵��: printf()ʹ�ô˺������ʵ�ʵĴ��ڴ�ӡ����
* ��    ��: int ch		Ҫ��ӡ���ַ�
*			FILE *f		�ļ����
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
int fputc(int ch, FILE *f)
{
	UART_WriteByte(UART0, ch);
	
	while(UART_IsTXBusy(UART0));
 	
	return ch;
}
