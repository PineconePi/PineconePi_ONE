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


IAP_Cache_Reset_t IAP_Cache_Reset = (IAP_Cache_Reset_t)0x11000601;
IAP_Flash_Param_t IAP_Flash_Param = (IAP_Flash_Param_t)0x11000681;
IAP_Flash_Erase_t IAP_Flash_Erase = (IAP_Flash_Erase_t)0x11000781;
IAP_Flash_Write_t IAP_Flash_Write = (IAP_Flash_Write_t)0x11000801;


/****************************************************************************************************************************************** 
* ��������: FLASH_Erase()
* ����˵��:	Ƭ��Flash����
* ��    ��: uint32_t addr			������ַ��������СΪ4K Byte
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void FLASH_Erase(uint32_t addr)
{	
	__disable_irq();
	
	IAP_Flash_Erase(addr / 0x1000);
	
	IAP_Cache_Reset();
	
	__enable_irq();
}

/****************************************************************************************************************************************** 
* ��������: FLASH_Write()
* ����˵��:	Ƭ��Flashд��
* ��    ��: uint32_t addr			д���ַ
*			uint32_t buff[]			Ҫд�������
*			uint32_t count			Ҫд�����ݵĸ���������Ϊ��λ���ұ�����4����������������д��4����
* ��    ��: ��
* ע������: д�����ݸ���������4����������������д��4����
******************************************************************************************************************************************/
void FLASH_Write(uint32_t addr, uint32_t buff[], uint32_t count)
{
	__disable_irq();
	
	IAP_Flash_Write(addr, (uint32_t)buff, count);
	
	IAP_Cache_Reset();
	
	__enable_irq();
}

/****************************************************************************************************************************************** 
* ��������: Flash_Param_at_120MHz()
* ����˵��:	��Flash�������ó�120MHz��Ƶ������ʱ����Ĳ���
* ��    ��: ��
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void Flash_Param_at_120MHz(void)
{
	__disable_irq();
	
	IAP_Flash_Param(0x48a, 0xabfc7a6e);
	
	__enable_irq();
}
