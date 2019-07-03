#include "SWM320.h"
#include "swm320_delay.h"

/* Private variables ---------------------------------------------------------*/
/** @defgroup HAL_Private_Variables HAL Private Variables
  * @{
  */
__IO uint32_t uwTick;	 
	 
void Delay_Init(void)
{
	uwTick=0;
	TIMR_Init(TIMR5, TIMR_MODE_TIMER, SystemCoreClock/1000, 1);
	TIMR_Start(TIMR5);
}

void IncTick(void)
{
	uwTick++;
}
uint32_t GetTick(void)
{
	return uwTick;
}
void TIMR5_Handler(void)
{
	TIMR_INTClr(TIMR5);
	IncTick();
}
void delay(__IO uint32_t Delay)
{
  uint32_t tickstart = GetTick();
  uint32_t wait = Delay;
  while((GetTick() - tickstart) < wait)
  {
  }
}



