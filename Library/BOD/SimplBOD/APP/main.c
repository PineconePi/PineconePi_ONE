#include "SWM320.h"


int main(void)
{	
	SystemInit();
	
	GPIO_Init(GPIOA, PIN5, 1, 0, 0);			//Êä³ö£¬½ÓLED
	
	SYS->BODIE = (1 << SYS_BODIE_2V2_Pos);
	SYS->BODIF = (1 << SYS_BODIF_2V2_Pos);
	NVIC_EnableIRQ(BOD_IRQn);
		
	while(1==1)
	{
		GPIO_ClrBit(GPIOA, PIN5);
	}
}

void BOD_Handler(void)
{	
	SYS->BODIF = (1 << SYS_BODIF_2V2_Pos);
	
	GPIO_SetBit(GPIOA, PIN5);
}
