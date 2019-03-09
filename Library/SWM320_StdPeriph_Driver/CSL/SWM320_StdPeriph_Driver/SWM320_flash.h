#ifndef __SWM320_FLASH_H__
#define __SWM320_FLASH_H__


void FLASH_Erase(uint32_t addr);
void FLASH_Write(uint32_t addr, uint32_t buff[], uint32_t size);


#if   defined ( __CC_ARM )

extern uint16_t Code_Flash_Param_at_120MHz[];

#define Flash_Param_at_120MHz	((Func_void_void) ((uint32_t)Code_Flash_Param_at_120MHz + 1))

#elif defined ( __ICCARM__ )

__ramfunc void Flash_Param_at_120MHz(void);

#endif


#endif //__SWM320_FLASH_H__
