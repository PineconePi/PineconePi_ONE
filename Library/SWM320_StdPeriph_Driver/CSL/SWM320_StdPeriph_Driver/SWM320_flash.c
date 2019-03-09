/****************************************************************************************************************************************** 
* 文件名称:	SWM320_flash.c
* 功能说明:	使用芯片的IAP功能将片上Flash模拟成EEPROM来保存数据，掉电后不丢失
* 技术支持:	http://www.synwit.com.cn/e/tool/gbook/?bid=1
* 注意事项:
* 版本日期: V1.1.0		2017年10月25日
* 升级记录: 
*******************************************************************************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION 
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, SYNWIT SHALL NOT BE HELD LIABLE 
* FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT 
* OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONN-
* -ECTION WITH THEIR PRODUCTS.
*
* COPYRIGHT 2012 Synwit Technology  
*******************************************************************************************************************************************/
#include "SWM320.h"
#include "SWM320_flash.h"


/****************************************************************************************************************************************** 
* 函数名称: FLASH_Erase()
* 功能说明:	片内Flash擦除
* 输    入: uint32_t addr			擦除地址
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void FLASH_Erase(uint32_t addr)
{	
	FLASH->ERASE = addr | ((uint32_t)1 << FLASH_ERASE_REQ_Pos);
	while((FLASH->STAT & FLASH_STAT_ERASE_GOING_Msk) == 0);
	while((FLASH->STAT & FLASH_STAT_ERASE_GOING_Msk) == 1);
	
	FLASH->ERASE = 0;
}

/****************************************************************************************************************************************** 
* 函数名称: FLASH_Write()
* 功能说明:	片内Flash写入
* 输    入: uint32_t addr			写入地址
*			uint32_t buff[]			要写入的数据
*			uint32_t size			要写入数据的个数，字为单位
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void FLASH_Write(uint32_t addr, uint32_t buff[], uint32_t size)
{
	uint32_t i, j;
		
	FLASH->CACHE |= (1 << FLASH_CACHE_PROG_Pos);
	
	for(i = 0; i < size/4; i++)
	{
		FLASH->ADDR = addr + i * 4 * 4;
		
		for(j = 0; j < 4; j++)
			FLASH->DATA = buff[i*4 + j];
		while((FLASH->STAT & FLASH_STAT_FIFO_EMPTY_Msk) == 0) __NOP();
	}
	if((size % 4) != 0)
	{
		FLASH->ADDR = addr + i * 4 * 4;
		
		for(j = 0; j < size%4; j++)
			FLASH->DATA = buff[i*4 + j];
		while((FLASH->STAT & FLASH_STAT_FIFO_EMPTY_Msk) == 0) __NOP();
	}
	while(FLASH->STAT & FLASH_STAT_PROG_GOING_Msk);
	
	FLASH->CACHE |= (1 << FLASH_CACHE_CLEAR_Pos);
	FLASH->CACHE = 0;
}


#if   defined ( __CC_ARM )

/* 将Flash参数设置成120MHz主频下运行时所需的参数的代码指令，生成这段儿指令的C代码是：
void Flash_Param_at_120MHz(void)
{
	uint32_t i;
	
	__disable_irq();
	for(i = 0; i < 5; i++) __NOP();
	
	FLASH->CFG0 = 0x48a;
	FLASH->CFG1 = 0xabfc7a6e;
	
	for(i = 0; i < 5; i++) __NOP();
	__enable_irq();
}

C代码编译生成库文件的反汇编是：
Flash_Param_at_120MHz
    0x00000000:    b672        r.      CPSID    i
    0x00000002:    2000        .       MOVS     r0,#0
    0x00000004:    bf00        ..      NOP      
    0x00000006:    1c40        @.      ADDS     r0,r0,#1
    0x00000008:    2805        .(      CMP      r0,#5
    0x0000000a:    d3fb        ..      BCC      {pc}-0x6 ; 0x4
    0x0000000c:    4806        .H      LDR      r0,[pc,#24] ; [0x28] = 0x4001f000
    0x0000000e:    f240418a    @..A    MOV      r1,#0x48a
    0x00000012:    6101        .a      STR      r1,[r0,#0x10]
    0x00000014:    4905        .I      LDR      r1,[pc,#20] ; [0x2c] = 0xabfc7a6e
    0x00000016:    6141        Aa      STR      r1,[r0,#0x14]
    0x00000018:    2000        .       MOVS     r0,#0
    0x0000001a:    bf00        ..      NOP      
    0x0000001c:    1c40        @.      ADDS     r0,r0,#1
    0x0000001e:    2805        .(      CMP      r0,#5
    0x00000020:    d3fb        ..      BCC      {pc}-0x6 ; 0x1a
    0x00000022:    b662        b.      CPSIE    i
    0x00000024:    4770        pG      BX       lr
$d
    0x00000026:    0000        ..      DCW    0
    0x00000028:    4001f000    ...@    DCD    1073868800
    0x0000002c:    abfc7a6e    nz..    DCD    2885450350
*/
uint16_t Code_Flash_Param_at_120MHz[] = {
	0xb672, 0x2000, 0xbf00, 0x1c40, 0x2805, 0xd3fb, 0x4806, 0xf240,
	0x418a, 0x6101, 0x4905, 0x6141, 0x2000, 0xbf00, 0x1c40, 0x2805, 
	0xd3fb, 0xb662, 0x4770, 0x0000, 0xf000, 0x4001, 0x7a6e, 0xabfc
};

#elif defined ( __ICCARM__ )

__ramfunc void Flash_Param_at_120MHz(void)
{
	uint32_t i;
	
	__ISB();
	__NOP();__NOP();__NOP();__NOP();
	
	FLASH->CFG0 = 0x48a;
	FLASH->CFG1 = 0xabfc7a6e;
	
	for(i = 0; i < 7; i++)  __NOP();
}

#endif



