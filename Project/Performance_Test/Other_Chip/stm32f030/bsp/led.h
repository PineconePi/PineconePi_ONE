/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LED_H
#define __LED_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_tim.h"

	 /* LED operation functions *****************************************************/
	 
	 void Green_Led(uint16_t on);
	 void Blue_Led(uint16_t light,TIM_HandleTypeDef mytim);
	 #ifdef __cplusplus
}
#endif

#endif 
