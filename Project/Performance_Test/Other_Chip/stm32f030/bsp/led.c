#include "led.h"
void Green_Led(uint16_t on)
{
	if(on==0)
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	else if(on==1)
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
}
void Blue_Led(uint16_t light,TIM_HandleTypeDef mytim)
{
	__HAL_TIM_SET_COMPARE(&mytim,TIM_CHANNEL_1,light);
}
