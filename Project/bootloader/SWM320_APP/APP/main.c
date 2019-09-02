#include "SWM320.h"

void SerialInit(void);
void JumpToUserBoot(void);

int main(void)
{ 	
	uint32_t i;
	
	__enable_irq();
	
 	SystemInit();
	
	SerialInit();
	
	GPIO_Init(GPIOA, PIN4, 0, 1, 0);	// ���룬������
	GPIO_Init(GPIOA, PIN5, 1, 0, 0);	// ���
	
	TIMR_Init(TIMR0, TIMR_MODE_TIMER, SystemCoreClock*1.5, 1);
	TIMR_Start(TIMR0);
	
	while(1==1)
 	{
		printf("Hi from App");
		
		for(i = 0; i < SystemCoreClock/4; i++) __NOP();
		
		
		if(GPIO_GetBit(GPIOA, PIN4) == 0)	// ��⵽�������£�����UserBoot
		{
			/* ��ת��UserBootǰ��Ҫ��APPʹ�õ�����ص�����λ��*/
			TIMR_Stop(TIMR0);
			TIMR_INTDis(TIMR0);
			
			JumpToUserBoot();
		}
 	}
}


void TIMR0_Handler(void)
{
	TIMR_INTClr(TIMR0);
	
	GPIO_InvBit(GPIOA, PIN5);
}


void JumpToUserBoot(void)
{	
	__disable_irq();
	
	uint32_t sp = *((volatile uint32_t *)0);
	uint32_t pc = *((volatile uint32_t *)4);
	
	typedef void (*Func_void_void)(void);
	Func_void_void ResetHandler = (Func_void_void)pc;
	
	SCB->VTOR = 0;
	
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
