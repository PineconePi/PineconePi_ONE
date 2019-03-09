#include "SWM320.h"


int main(void)
{	
	SystemInit();
	
	GPIO_Init(GPIOA, PIN5, 1, 0, 0);
		
	SYS->LRCCR &= ~(1 << SYS_LRCCR_OFF_Pos);	// 开启32K时钟
	
	TIMR_Init(TIMR4, TIMR_MODE_COUNTER, 32, 1);	// 每计32个边沿进入中断
	
	TIMR_Start(TIMR4);
	
	while(1==1)
	{
	}
}

void TIMR4_Handler(void)
{
	TIMR_INTClr(TIMR4);
	
	GPIO_InvBit(GPIOA, PIN5);
}
