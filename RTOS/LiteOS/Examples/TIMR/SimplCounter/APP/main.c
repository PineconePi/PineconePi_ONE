#include "SWM320.h"


int main(void)
{	
	SystemInit();
	
	GPIO_Init(GPIOA, PIN5, 1, 0, 0);
	
	PORT_Init(PORTA, PIN4, FUNMUX0_TIMR2_IN, 1);
	
	TIMR_Init(TIMR2, TIMR_MODE_COUNTER, 3, 1);	//每计3个边沿进入中断
	
	TIMR_Start(TIMR2);
	
	while(1==1)
	{
	}
}

void TIMR2_Handler(void)
{
	TIMR_INTClr(TIMR2);
	
	GPIO_InvBit(GPIOA, PIN5);
}
