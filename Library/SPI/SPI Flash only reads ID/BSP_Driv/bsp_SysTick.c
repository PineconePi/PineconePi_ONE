
#include "bsp_SysTick.h"

static __IO uint32_t uwTick;
 
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms中断一次
	 * SystemFrequency / 100000	 10us中断一次
	 * SystemFrequency / 1000000 1us中断一次
	 */
	if (SysTick_Config(SystemCoreClock / 1000))
	{ 
		/* Capture error */ 
		while (1);
	}
}

__weak void SWM_IncTick(void)
{
	uwTick++;
}

__weak uint32_t SWM_GetTick(void)
{
	return uwTick;
}

__weak void SWM_Delay(__IO uint32_t Delay)
{
	uint32_t tickstart = 0;
	tickstart = SWM_GetTick();
	while((SWM_GetTick() - tickstart) < Delay)
	{
	}
}

void SysTick_Handler(void)
{
	SWM_IncTick();
}

/*********************************************END OF FILE**********************/
