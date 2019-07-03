/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SWM320_DELAY_H
#define __SWM320_DELAY_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "SWM320.h"

void Delay_Init(void); 
void IncTick(void);
uint32_t GetTick(void);
void delay(__IO uint32_t Delay);
 
#ifdef __cplusplus
}
#endif

#endif
