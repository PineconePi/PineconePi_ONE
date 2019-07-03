/* Define to prevent recursive inclusion 这个定义是为了防止递归包含-------------------------------------*/
#ifndef __SWM320_DELAY_H
#define __SWM320_DELAY_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "SWM320.h"

void Delay_Init(void); //对计数值初始化0 设置并开启5号定时器
void IncTick(void);//用5号定时器把计数值+1
uint32_t GetTick(void);//返回计数值
void delay(__IO uint32_t Delay);//毫秒延时
 
#ifdef __cplusplus
}
#endif

#endif
