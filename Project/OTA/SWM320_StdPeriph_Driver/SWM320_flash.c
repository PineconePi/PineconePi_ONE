/****************************************************************************************************************************************** 
* �ļ�����:	SWM320_flash.c
* ����˵��:	ʹ��оƬ��IAP���ܽ�Ƭ��Flashģ���EEPROM���������ݣ�����󲻶�ʧ
* ����֧��:	http://www.synwit.com.cn/e/tool/gbook/?bid=1
* ע������:
* �汾����: V1.1.0		2017��10��25��
* ������¼: 
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


__attribute__((section("PlaceInRAM")))
static void switchTo80M(void)
{
	uint32_t i;
	
	for(i = 0; i < 50; i++) __NOP();
	
	FLASH->CFG0 = 0x4bf;
	FLASH->CFG1 = 0xabfc7a6e;
	
	for(i = 0; i < 50; i++) __NOP();
}

__attribute__((section("PlaceInRAM")))
static void switchTo40M(void)
{
	uint32_t i;
	
	for(i = 0; i < 50; i++) __NOP();
	
	FLASH->CFG0 = 0x4b9;
	FLASH->CFG1 = 0xABF41F25;
	
	for(i = 0; i < 50; i++) __NOP();
}

/****************************************************************************************************************************************** 
* ��������: FLASH_Erase()
* ����˵��:	Ƭ��Flash����
* ��    ��: uint32_t addr			������ַ
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void FLASH_Erase(uint32_t addr)
{
// 	switchTo80M();
	
	FLASH->ERASE = addr | ((uint32_t)1 << FLASH_ERASE_REQ_Pos);
	while((FLASH->STAT & FLASH_STAT_ERASE_GOING_Msk) == 0);
	while((FLASH->STAT & FLASH_STAT_ERASE_GOING_Msk) == 1);
	
	FLASH->ERASE = 0;
	
// 	switchTo40M();
}

/****************************************************************************************************************************************** 
* ��������: FLASH_Write()
* ����˵��:	Ƭ��Flashд��
* ��    ��: uint32_t addr			д���ַ
*			uint32_t buff[]			Ҫд�������
*			uint32_t size			Ҫд�����ݵĸ�������Ϊ��λ
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void FLASH_Write(uint32_t addr, uint32_t buff[], uint32_t size)
{
	uint32_t i, j;
	
	switchTo80M();
	
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
	
// 	switchTo40M();
}
