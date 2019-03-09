#include "SWM320.h"


int main(void)
{	
	uint32_t i;
	
	SystemInit();
	
	SYS->CLKEN |= (1 << SYS_CLKEN_RTCBKP_Pos);		//唤醒必须开启此时钟
	
	SYS->LRCCR &= ~(1 << SYS_LRCCR_OFF_Pos);		//开启32K低频振荡器
	
	GPIO_Init(GPIOA, PIN5, 1, 0, 0);				//输出， 接LED
	
	GPIO_Init(GPIOA, PIN4, 0, 1, 0);				//输入，上拉使能，接KEY
	SYS->PAWKEN |= (1 << PIN4);						//开启PA4引脚低电平唤醒
	
	while(1==1)
	{
		GPIO_SetBit(GPIOA, PIN5);					//点亮LED
		for(i = 0; i < 10000000; i++);
		GPIO_ClrBit(GPIOA, PIN5);					//熄灭LED
		
		switchCLK_32KHz();
		
		SYS->HRCCR = (1 << SYS_HRCCR_OFF_Pos);		//关闭HRC、降低功耗
		
		SYS->SLEEP |= (1 << SYS_SLEEP_SLEEP_Pos);	//进入睡眠模式
	
		while((SYS->PAWKSR & (1 << PIN4)) == 0);	//等待唤醒条件
		SYS->PAWKSR |= (1 << PIN4);					//清除唤醒状态
		
		switchCLK_20MHz();
	}
}
