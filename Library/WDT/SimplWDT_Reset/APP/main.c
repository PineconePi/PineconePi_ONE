#include "SWM320.h"


int main(void)
{
	uint32_t i;

	SystemInit();

	for(i=0;i<SystemCoreClock/10;i++);			//复位后等一会儿再点亮LED
	
	GPIO_Init(GPIOA, PIN5, 1, 0, 0);			//输出，接LED
	GPIO_SetBit(GPIOA, PIN5);					//点亮LED
	
	WDT_Init(WDT, SystemCoreClock/2, WDT_MODE_RESET);	//每0.5秒需要喂狗一次
	WDT_Start(WDT);								//启动WDT
	
	while(1==1)
	{
		
	}
}
