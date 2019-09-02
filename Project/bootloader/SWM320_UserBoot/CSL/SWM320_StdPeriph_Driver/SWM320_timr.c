/****************************************************************************************************************************************** 
* �ļ�����: SWM320_timr.c
* ����˵��:	SWM320��Ƭ���ļ�����/��ʱ������������
* ����֧��:	http://www.synwit.com.cn/e/tool/gbook/?bid=1
* ע������:
* �汾����:	V1.1.0		2017��10��25��
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
#include "SWM320_timr.h"


/****************************************************************************************************************************************** 
* ��������: TIMR_Init()
* ����˵��:	TIMR��ʱ��/��������ʼ��
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������Чֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��TIMR5
*			uint32_t mode			TIMR_MODE_TIMER ��ʱ��ģʽ    TIMR_MODE_COUNTER ������ģʽ
*			uint32_t period			��ʱ/��������
*			uint32_t int_en			�ж�ʹ��
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_Init(TIMR_TypeDef * TIMRx, uint32_t mode, uint32_t period, uint32_t int_en)
{
	SYS->CLKEN |= (0x01 << SYS_CLKEN_TIMR_Pos);
	
	TIMR_Stop(TIMRx);	//һЩ�ؼ��Ĵ���ֻ���ڶ�ʱ��ֹͣʱ����
	
	TIMRx->CTRL &= ~TIMR_CTRL_CLKSRC_Msk;
	TIMRx->CTRL |= mode << TIMR_CTRL_CLKSRC_Pos;
	
	TIMRx->LDVAL = period;
	
	switch((uint32_t)TIMRx)
	{
	case ((uint32_t)TIMR0):
		TIMRG->IF = (1 << TIMRG_IF_TIMR0_Pos);		//ʹ���ж�ǰ����жϱ�־
		TIMRG->IE &= ~TIMRG_IE_TIMR0_Msk;
		TIMRG->IE |= (int_en << TIMRG_IE_TIMR0_Pos);
	
		if(int_en) NVIC_EnableIRQ(TIMR0_IRQn);
		break;
	
	case ((uint32_t)TIMR1):
		TIMRG->IF = (1 << TIMRG_IF_TIMR1_Pos);
		TIMRG->IE &= ~TIMRG_IE_TIMR1_Msk;
		TIMRG->IE |= (int_en << TIMRG_IE_TIMR1_Pos);
	
		if(int_en) NVIC_EnableIRQ(TIMR1_IRQn);
		break;
	
	case ((uint32_t)TIMR2):
		TIMRG->IF = (1 << TIMRG_IF_TIMR2_Pos);
		TIMRG->IE &= ~TIMRG_IE_TIMR2_Msk;
		TIMRG->IE |= (int_en << TIMRG_IE_TIMR2_Pos);
	
		if(int_en) NVIC_EnableIRQ(TIMR2_IRQn);
		break;
	
	case ((uint32_t)TIMR3):
		TIMRG->IF = (1 << TIMRG_IF_TIMR3_Pos);
		TIMRG->IE &= ~TIMRG_IE_TIMR3_Msk;
		TIMRG->IE |= (int_en << TIMRG_IE_TIMR3_Pos);
	
		if(int_en) NVIC_EnableIRQ(TIMR3_IRQn);
		break;
	
	case ((uint32_t)TIMR4):
		TIMRG->IF = (1 << TIMRG_IF_TIMR4_Pos);
		TIMRG->IE &= ~TIMRG_IE_TIMR4_Msk;
		TIMRG->IE |= (int_en << TIMRG_IE_TIMR4_Pos);
	
		if(int_en) NVIC_EnableIRQ(TIMR4_IRQn);
		break;
	
	case ((uint32_t)TIMR5):
		TIMRG->IF = (1 << TIMRG_IF_TIMR5_Pos);
		TIMRG->IE &= ~TIMRG_IE_TIMR5_Msk;
		TIMRG->IE |= (int_en << TIMRG_IE_TIMR5_Pos);
	
		if(int_en) NVIC_EnableIRQ(TIMR5_IRQn);
		break;
	}
}

/****************************************************************************************************************************************** 
* ��������:	TIMR_Start()
* ����˵��:	������ʱ�����ӳ�ʼֵ��ʼ��ʱ/����
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��TIMR5
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_Start(TIMR_TypeDef * TIMRx)
{
	TIMRx->CTRL |= TIMR_CTRL_EN_Msk;
}

/****************************************************************************************************************************************** 
* ��������:	TIMR_Stop()
* ����˵��:	ֹͣ��ʱ��
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��TIMR5
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_Stop(TIMR_TypeDef * TIMRx)
{
	TIMRx->CTRL &= ~TIMR_CTRL_EN_Msk;
}

/****************************************************************************************************************************************** 
* ��������:	TIMR_Halt()
* ����˵��:	��ͣ��ʱ��������ֵ���ֲ���
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��TIMR5
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_Halt(TIMR_TypeDef * TIMRx)
{
	switch((uint32_t)TIMRx)
	{
	case ((uint32_t)TIMR0):
		TIMRG->HALT |= (0x01 << TIMRG_HALT_TIMR0_Pos);
		break;
	
	case ((uint32_t)TIMR1):
		TIMRG->HALT |= (0x01 << TIMRG_HALT_TIMR1_Pos);
		break;
	
	case ((uint32_t)TIMR2):
		TIMRG->HALT |= (0x01 << TIMRG_HALT_TIMR2_Pos);
		break;
	
	case ((uint32_t)TIMR3):
		TIMRG->HALT |= (0x01 << TIMRG_HALT_TIMR3_Pos);
		break;
	
	case ((uint32_t)TIMR4):
		TIMRG->HALT |= (0x01 << TIMRG_HALT_TIMR4_Pos);
		break;
	
	case ((uint32_t)TIMR5):
		TIMRG->HALT |= (0x01 << TIMRG_HALT_TIMR5_Pos);
		break;
	}
}

/****************************************************************************************************************************************** 
* ��������:	TIMR_Resume()
* ����˵��:	�ָ���ʱ��������ͣ����������
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��TIMR5
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_Resume(TIMR_TypeDef * TIMRx)
{
	switch((uint32_t)TIMRx)
	{
	case ((uint32_t)TIMR0):
		TIMRG->HALT &= ~(0x01 << TIMRG_HALT_TIMR0_Pos);
		break;
	
	case ((uint32_t)TIMR1):
		TIMRG->HALT &= ~(0x01 << TIMRG_HALT_TIMR1_Pos);
		break;
	
	case ((uint32_t)TIMR2):
		TIMRG->HALT &= ~(0x01 << TIMRG_HALT_TIMR2_Pos);
		break;
	
	case ((uint32_t)TIMR3):
		TIMRG->HALT &= ~(0x01 << TIMRG_HALT_TIMR3_Pos);
		break;
	
	case ((uint32_t)TIMR4):
		TIMRG->HALT &= ~(0x01 << TIMRG_HALT_TIMR4_Pos);
		break;
	
	case ((uint32_t)TIMR5):
		TIMRG->HALT &= ~(0x01 << TIMRG_HALT_TIMR5_Pos);
		break;
	}
}

/****************************************************************************************************************************************** 
* ��������: TIMR_SetPeriod()
* ����˵��:	���ö�ʱ/��������
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��TIMR5
*			uint32_t period			��ʱ/��������
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_SetPeriod(TIMR_TypeDef * TIMRx, uint32_t period)
{
	TIMRx->LDVAL = period;
}

/****************************************************************************************************************************************** 
* ��������: TIMR_GetPeriod()
* ����˵��:	��ȡ��ʱ/��������
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��TIMR5
* ��    ��: uint32_t				��ǰ��ʱ/��������
* ע������: ��
******************************************************************************************************************************************/
uint32_t TIMR_GetPeriod(TIMR_TypeDef * TIMRx)
{
	return TIMRx->LDVAL; 
}

/****************************************************************************************************************************************** 
* ��������:	TIMR_GetCurValue()
* ����˵��:	��ȡ��ǰ����ֵ
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��TIMR5
* ��    ��: uint32_t				��ǰ����ֵ
* ע������: ��
******************************************************************************************************************************************/
uint32_t TIMR_GetCurValue(TIMR_TypeDef * TIMRx)
{
	return TIMRx->CVAL;
}

/****************************************************************************************************************************************** 
* ��������:	TIMR_INTEn()
* ����˵��:	ʹ���ж�
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��TIMR5
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_INTEn(TIMR_TypeDef * TIMRx)
{
	switch((uint32_t)TIMRx)
	{
	case ((uint32_t)TIMR0):
		TIMRG->IE |= (0x01 << TIMRG_IE_TIMR0_Pos);
		NVIC_EnableIRQ(TIMR0_IRQn);
		break;
	
	case ((uint32_t)TIMR1):
		TIMRG->IE |= (0x01 << TIMRG_IE_TIMR1_Pos);
		NVIC_EnableIRQ(TIMR1_IRQn);
		break;
	
	case ((uint32_t)TIMR2):
		TIMRG->IE |= (0x01 << TIMRG_IE_TIMR2_Pos);
		NVIC_EnableIRQ(TIMR2_IRQn);
		break;
	
	case ((uint32_t)TIMR3):
		TIMRG->IE |= (0x01 << TIMRG_IE_TIMR3_Pos);
		NVIC_EnableIRQ(TIMR3_IRQn);
		break;
	
	case ((uint32_t)TIMR4):
		TIMRG->IE |= (0x01 << TIMRG_IE_TIMR4_Pos);
		NVIC_EnableIRQ(TIMR4_IRQn);
		break;
	
	case ((uint32_t)TIMR5):
		TIMRG->IE |= (0x01 << TIMRG_IE_TIMR5_Pos);
		NVIC_EnableIRQ(TIMR5_IRQn);
		break;
	}
}

/****************************************************************************************************************************************** 
* ��������: TIMR_INTDis()
* ����˵��:	�����ж�
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��TIMR5
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_INTDis(TIMR_TypeDef * TIMRx)
{
	switch((uint32_t)TIMRx)
	{
	case ((uint32_t)TIMR0):
		TIMRG->IE &= ~(0x01 << TIMRG_IE_TIMR0_Pos);
		break;
	
	case ((uint32_t)TIMR1):
		TIMRG->IE &= ~(0x01 << TIMRG_IE_TIMR1_Pos);
		break;
	
	case ((uint32_t)TIMR2):
		TIMRG->IE &= ~(0x01 << TIMRG_IE_TIMR2_Pos);
		break;
	
	case ((uint32_t)TIMR3):
		TIMRG->IE &= ~(0x01 << TIMRG_IE_TIMR3_Pos);
		break;
	
	case ((uint32_t)TIMR4):
		TIMRG->IE &= ~(0x01 << TIMRG_IE_TIMR4_Pos);
		break;
	
	case ((uint32_t)TIMR5):
		TIMRG->IE &= ~(0x01 << TIMRG_IE_TIMR5_Pos);
		break;
	}
}

/****************************************************************************************************************************************** 
* ��������:	TIMR_INTClr()
* ����˵��:	����жϱ�־
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��TIMR5
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_INTClr(TIMR_TypeDef * TIMRx)
{
	switch((uint32_t)TIMRx)
	{
	case ((uint32_t)TIMR0):
		TIMRG->IF = (0x01 << TIMRG_IF_TIMR0_Pos);
		break;
	
	case ((uint32_t)TIMR1):
		TIMRG->IF = (0x01 << TIMRG_IF_TIMR1_Pos);
		break;
	
	case ((uint32_t)TIMR2):
		TIMRG->IF = (0x01 << TIMRG_IF_TIMR2_Pos);
		break;
	
	case ((uint32_t)TIMR3):
		TIMRG->IF = (0x01 << TIMRG_IF_TIMR3_Pos);
		break;
	
	case ((uint32_t)TIMR4):
		TIMRG->IF = (0x01 << TIMRG_IF_TIMR4_Pos);
		break;
	
	case ((uint32_t)TIMR5):
		TIMRG->IF = (0x01 << TIMRG_IF_TIMR5_Pos);
		break;
	}
}

/****************************************************************************************************************************************** 
* ��������: TIMR_INTStat()
* ����˵��:	��ȡ�ж�״̬
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��TIMR5
* ��    ��: uint32_t 				0 TIMRxδ�����ж�    1 TIMRx�������ж�
* ע������: ��
******************************************************************************************************************************************/
uint32_t TIMR_INTStat(TIMR_TypeDef * TIMRx)
{
	switch((uint32_t)TIMRx)
	{
	case ((uint32_t)TIMR0):
		return (TIMRG->IF & TIMRG_IF_TIMR0_Msk) ? 1 : 0;
	
	case ((uint32_t)TIMR1):
		return (TIMRG->IF & TIMRG_IF_TIMR1_Msk) ? 1 : 0;
	
	case ((uint32_t)TIMR2):
		return (TIMRG->IF & TIMRG_IF_TIMR2_Msk) ? 1 : 0;
	
	case ((uint32_t)TIMR3):
		return (TIMRG->IF & TIMRG_IF_TIMR3_Msk) ? 1 : 0;
	
	case ((uint32_t)TIMR4):
		return (TIMRG->IF & TIMRG_IF_TIMR4_Msk) ? 1 : 0;
	
	case ((uint32_t)TIMR5):
		return (TIMRG->IF & TIMRG_IF_TIMR5_Msk) ? 1 : 0;
	}
	
	return 0;
}
