/****************************************************************************************************************************************** 
* �ļ�����: SWM320_norflash.c
* ����˵��:	SWM320��Ƭ����NOR Flash��������
* ����֧��:	http://www.synwit.com.cn/e/tool/gbook/?bid=1
* ע������:
* �汾����: V1.1.0		2017��10��25��
* ������¼: 
*
*
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
#include "SWM320_norflash.h"


/****************************************************************************************************************************************** 
* ��������:	NORFL_Init()
* ����˵��:	NOR Flash��������ʼ��
* ��    ��: NORFL_InitStructure * initStruct    ����NOR Flash����������趨ֵ�Ľṹ��
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void NORFL_Init(NORFL_InitStructure * initStruct)
{
	uint32_t i;
	
	// ����SRAMǰ��Ҫˢ����SDRAM������
	do {
		SYS->CLKEN |=  (1 << SYS_CLKEN_SDRAM_Pos);
		
		while(SDRAMC->REFDONE == 0);
		SDRAMC->REFRESH &= ~(1 << SDRAMC_REFRESH_EN_Pos);
		
		for(i = 0; i < 1000; i++) __NOP();
		SYS->CLKEN &= ~(1 << SYS_CLKEN_SDRAM_Pos);
	} while(0);
	
	SYS->CLKEN |= (1 << SYS_CLKEN_NORFL_Pos);
	
	NORFLC->CR = ((initStruct->DataWidth == 8 ? 1 : 0) << NORFLC_CR_BYTEIF_Pos) |
				 (initStruct->WELowPulseTime << NORFLC_CR_WRTIME_Pos) |
				 (initStruct->OEPreValidTime << NORFLC_CR_RDTIME_Pos);
	
	NORFLC->IE = 3;
	NORFLC->IF = 3;		// ����жϱ�־
	if(initStruct->OperFinishIEn)  NORFLC->IM &= ~(1 << NORFLC_IM_FINISH_Pos);
	else                           NORFLC->IM |=  (1 << NORFLC_IM_FINISH_Pos);
	if(initStruct->OperTimeoutIEn) NORFLC->IM &= ~(1 << NORFLC_IM_TIMEOUT_Pos);	
	else                           NORFLC->IM |=  (1 << NORFLC_IM_TIMEOUT_Pos);	
}

/****************************************************************************************************************************************** 
* ��������:	NORFL_ChipErase()
* ����˵��:	NOR Flash��Ƭ����
* ��    ��: ��
* ��    ��: uint32_t			0 �����ɹ�    1 ������ʱ
* ע������: ��
******************************************************************************************************************************************/
uint32_t NORFL_ChipErase(void)
{
	uint32_t res;
	
	NORFLC->CMD = (NORFL_CMD_CHIP_ERASE << NORFLC_CMD_CMD_Pos);
	
	while(((NORFLC->IF & NORFLC_IF_FINISH_Msk) == 0) &&
		  ((NORFLC->IF & NORFLC_IF_TIMEOUT_Msk) == 0)) __NOP();
	
	if(NORFLC->IF & NORFLC_IF_FINISH_Msk)  res = 0;
	else                                   res = 1;
	
	NORFLC->IF = NORFLC_IF_FINISH_Msk | NORFLC_IF_TIMEOUT_Msk;
	
	return res;
}

/****************************************************************************************************************************************** 
* ��������:	NORFL_SectorErase()
* ����˵��:	NOR Flash��������
* ��    ��: uint32_t addr		Ҫ������������ʼ��ַ
* ��    ��: uint32_t			0 �����ɹ�    1 ������ʱ
* ע������: MX29LV128DB ǰ8����Ϊ8K����255����Ϊ64K    MX29LV128DT ǰ255����Ϊ64K����8����Ϊ8K
******************************************************************************************************************************************/
uint32_t NORFL_SectorErase(uint32_t addr)
{
	uint32_t res;
	
	NORFLC->ADDR = addr;
	NORFLC->CMD = (NORFL_CMD_SECTOR_ERASE << NORFLC_CMD_CMD_Pos);
	
	while(((NORFLC->IF & NORFLC_IF_FINISH_Msk) == 0) &&
		  ((NORFLC->IF & NORFLC_IF_TIMEOUT_Msk) == 0)) __NOP();
	
	if(NORFLC->IF & NORFLC_IF_FINISH_Msk)  res = 0;
	else                                   res = 1;
	
	NORFLC->IF = NORFLC_IF_FINISH_Msk | NORFLC_IF_TIMEOUT_Msk;
	
	return res;
}

/****************************************************************************************************************************************** 
* ��������:	NORFL_Write()
* ����˵��:	NOR Flashд
* ��    ��: uint32_t addr		����Ҫд��ĵ�ַ
*			uint32_t data		Ҫд�������
* ��    ��: uint32_t			0 д��ɹ�    1 д�볬ʱ
* ע������: Ӳ�����ӣ�������Ϊ16λʱ������д�룻������Ϊ8λʱ���ֽ�д��
******************************************************************************************************************************************/
uint32_t NORFL_Write(uint32_t addr, uint32_t data)
{
	uint32_t res;
	
	NORFLC->ADDR = addr;
	NORFLC->CMD = (NORFL_CMD_PROGRAM << NORFLC_CMD_CMD_Pos) | (data << NORFLC_CMD_DATA_Pos);
	
	while(((NORFLC->IF & NORFLC_IF_FINISH_Msk) == 0) &&
		  ((NORFLC->IF & NORFLC_IF_TIMEOUT_Msk) == 0)) __NOP();
	
	if(NORFLC->IF & NORFLC_IF_FINISH_Msk)  res = 0;
	else                                   res = 1;
	
	NORFLC->IF = NORFLC_IF_FINISH_Msk | NORFLC_IF_TIMEOUT_Msk;
	
	return res;
}

/****************************************************************************************************************************************** 
* ��������:	NORFL_Read()
* ����˵��:	NOR Flash��
* ��    ��: uint32_t addr		����Ҫ�����ĵ�ַ
* ��    ��: uint32_t			����������
* ע������: Ӳ�����ӣ�������Ϊ16λʱ�����ֶ�����������Ϊ8λʱ���ֽڶ���
******************************************************************************************************************************************/
uint32_t NORFL_Read(uint32_t addr)
{
	NORFLC->ADDR = addr;
	NORFLC->CMD = (NORFL_CMD_READ << NORFLC_CMD_CMD_Pos);
	
	return (NORFLC->CMD & NORFLC_CMD_DATA_Msk);
}

/****************************************************************************************************************************************** 
* ��������:	NORFL_ReadID()
* ����˵��:	NOR Flash��ID
* ��    ��: uint32_t id_addr	ID��ַ���˲�����оƬ��صģ�ÿ��оƬ����ͬ
* ��    ��: uint16_t			��ȡ����ID
* ע������: ��
******************************************************************************************************************************************/
uint16_t NORFL_ReadID(uint32_t id_addr)
{
	uint16_t id;
	
	NORFLC->CMD = (NORFL_CMD_AUTO_SELECT << NORFLC_CMD_CMD_Pos);
	
	NORFLC->ADDR = id_addr;
	NORFLC->CMD = (NORFL_CMD_READ << NORFLC_CMD_CMD_Pos);
	
	id = NORFLC->CMD & NORFLC_CMD_DATA_Msk;
	
	NORFLC->CMD = (NORFL_CMD_RESET << NORFLC_CMD_CMD_Pos);	// �˳�ID��ȡģʽ
	
	return id;
}
