/****************************************************************************************************************************************** 
* �ļ�����: SWM320_gpio.c
* ����˵��:	SWM320��Ƭ����ͨ�������������������
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
#include "SWM320_gpio.h"


/****************************************************************************************************************************************** 
* ��������: GPIO_Init()
* ����˵��:	���ų�ʼ�����������ŷ����������衢�������衢��©���
* ��    ��: GPIO_TypeDef * GPIOx	    ָ��GPIO�˿ڣ���Чֵ����GPIOA��GPIOB��GPIOC��GPIOM��GPION��GPIOP	
*			uint32_t n		       ָ��GPIO���ţ���Чֵ����PIN0��PIN1��PIN2��... ... PIN22��PIN23
*			uint32_t dir	       ���ŷ���0 ����        1 ���
*			uint32_t pull_up	   �������裬0 �ر�����    1 ��������
*			uint32_t pull_down	   �������裬0 �ر�����    1 ��������
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void GPIO_Init(GPIO_TypeDef * GPIOx, uint32_t n, uint32_t dir, uint32_t pull_up, uint32_t pull_down)
{
	switch((uint32_t)GPIOx)
	{
	case ((uint32_t)GPIOA):
		SYS->CLKEN |= (0x01 << SYS_CLKEN_GPIOA_Pos);
		
		PORT_Init(PORTA, n, 0, 1);			//PORTA.PINn��������ΪGPIO���ܣ��������뿪��
		if(dir == 1)
		{			
			GPIOA->DIR |= (0x01 << n);
		}
		else
		{
			GPIOA->DIR &= ~(0x01 << n);
		}
		
		if(pull_up == 1)
			PORT->PORTA_PULLU |= (0x01 << n);
		else
			PORT->PORTA_PULLU &= ~(0x01 << n);
		break;
	
	case ((uint32_t)GPIOB):
		SYS->CLKEN |= (0x01 << SYS_CLKEN_GPIOB_Pos);
		
		PORT_Init(PORTB, n, 0, 1);			//PORTB.PINn��������ΪGPIO���ܣ��������뿪��
		if(dir == 1)
		{			
			GPIOB->DIR |= (0x01 << n);
		}
		else
		{
			GPIOB->DIR &= ~(0x01 << n);
		}
		
		if(pull_down == 1)
			PORT->PORTB_PULLD |= (0x01 << n);
		else
			PORT->PORTB_PULLD &= ~(0x01 << n);
		break;

	case ((uint32_t)GPIOC):
		SYS->CLKEN |= (0x01 << SYS_CLKEN_GPIOC_Pos);
		
		PORT_Init(PORTC, n, 0, 1);			//PORTC.PINn��������ΪGPIO���ܣ��������뿪��
		if(dir == 1)
		{			
			GPIOC->DIR |= (0x01 << n);
		}
		else
		{
			GPIOC->DIR &= ~(0x01 << n);
		}
		
		if(pull_up == 1)
			PORT->PORTC_PULLU |= (0x01 << n);
		else
			PORT->PORTC_PULLU &= ~(0x01 << n);
		break;
		
	case ((uint32_t)GPIOM):
		SYS->CLKEN |= (0x01 << SYS_CLKEN_GPIOM_Pos);
		
		PORT_Init(PORTM, n, 0, 1);			//PORTM.PINn��������ΪGPIO���ܣ��������뿪��
		if(dir == 1)
		{			
			GPIOM->DIR |= (0x01 << n);
		}
		else
		{
			GPIOM->DIR &= ~(0x01 << n);
		}
		
		if(pull_up == 1)
			PORT->PORTM_PULLU |= (0x01 << n);
		else
			PORT->PORTM_PULLU &= ~(0x01 << n);
		break;
		
	case ((uint32_t)GPION):
		SYS->CLKEN |= (0x01 << SYS_CLKEN_GPION_Pos);
		
		PORT_Init(PORTN, n, 0, 1);			//PORTN.PINn��������ΪGPIO���ܣ��������뿪��
		if(dir == 1)
		{			
			GPION->DIR |= (0x01 << n);
		}
		else
		{
			GPION->DIR &= ~(0x01 << n);
		}
		
		if(pull_down == 1)
			PORT->PORTN_PULLD |= (0x01 << n);
		else
			PORT->PORTN_PULLD &= ~(0x01 << n);
		break;
	
	case ((uint32_t)GPIOP):
		SYS->CLKEN |= (0x01 << SYS_CLKEN_GPIOP_Pos);
		
		PORT_Init(PORTP, n, 0, 1);			//PORTP.PINn��������ΪGPIO���ܣ��������뿪��
		if(dir == 1)
		{			
			GPIOP->DIR |= (0x01 << n);
		}
		else
		{
			GPIOP->DIR &= ~(0x01 << n);
		}
		
		if(pull_up == 1)
			PORT->PORTP_PULLU |= (0x01 << n);
		else
			PORT->PORTP_PULLU &= ~(0x01 << n);
		break;
	}
}

/****************************************************************************************************************************************** 
* ��������: GPIO_SetBit()
* ����˵��:	������ָ�������ŵ�ƽ�ø�
* ��    ��: GPIO_TypeDef * GPIOx	    ָ��GPIO�˿ڣ���Чֵ����GPIOA��GPIOB��GPIOC��GPIOM��GPION��GPIOP	
*			uint32_t n		       ָ��GPIO���ţ���Чֵ����PIN0��PIN1��PIN2��... ... PIN22��PIN23
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void GPIO_SetBit(GPIO_TypeDef * GPIOx, uint32_t n)
{
	GPIOx->DATA |= (0x01 << n);
}

/****************************************************************************************************************************************** 
* ��������:	GPIO_ClrBit()
* ����˵��:	������ָ�������ŵ�ƽ�õ�
* ��    ��: GPIO_TypeDef * GPIOx	    ָ��GPIO�˿ڣ���Чֵ����GPIOA��GPIOB��GPIOC��GPIOM��GPION��GPIOP	
*			uint32_t n		       ָ��GPIO���ţ���Чֵ����PIN0��PIN1��PIN2��... ... PIN22��PIN23
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void GPIO_ClrBit(GPIO_TypeDef * GPIOx, uint32_t n)
{
	GPIOx->DATA &= ~(0x01 << n);
}

/****************************************************************************************************************************************** 
* ��������: GPIO_InvBit()
* ����˵��:	������ָ�������ŵ�ƽ��ת
* ��    ��: GPIO_TypeDef * GPIOx	    ָ��GPIO�˿ڣ���Чֵ����GPIOA��GPIOB��GPIOC��GPIOM��GPION��GPIOP	
*			uint32_t n		       ָ��GPIO���ţ���Чֵ����PIN0��PIN1��PIN2��... ... PIN22��PIN23
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void GPIO_InvBit(GPIO_TypeDef * GPIOx, uint32_t n)
{
	GPIOx->DATA ^= (0x01 << n);
}

/****************************************************************************************************************************************** 
* ��������:	GPIO_GetBit()
* ����˵��:	��ȡ����ָ�������ŵĵ�ƽ״̬
* ��    ��: GPIO_TypeDef * GPIOx	    ָ��GPIO�˿ڣ���Чֵ����GPIOA��GPIOB��GPIOC��GPIOM��GPION��GPIOP	
*			uint32_t n		       ָ��GPIO���ţ���Чֵ����PIN0��PIN1��PIN2��... ... PIN22��PIN23
* ��    ��: ����ָ�������ŵĵ�ƽ״̬	0 �͵�ƽ	1 �ߵ�ƽ
* ע������: ��
******************************************************************************************************************************************/
uint32_t GPIO_GetBit(GPIO_TypeDef * GPIOx, uint32_t n)
{	
	return ((GPIOx->DATA >> n) & 0x01);
}

/****************************************************************************************************************************************** 
* ��������: GPIO_SetBits()
* ����˵��: ������ָ���Ĵ�n��ʼ��wλ�������ŵĵ�ƽ�ø�
* ��    ��: GPIO_TypeDef * GPIOx	    ָ��GPIO�˿ڣ���Чֵ����GPIOA��GPIOB��GPIOC��GPIOM��GPION��GPIOP	
*			uint32_t n		       ָ��GPIO���ţ���Чֵ����PIN0��PIN1��PIN2��... ... PIN22��PIN23
*			uint32_t w			   ָ��Ҫ�����ŵ�ƽ�øߵ����ŵĸ���
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void GPIO_SetBits(GPIO_TypeDef * GPIOx, uint32_t n, uint32_t w)
{
	uint32_t bits;
	
	bits = 0xFFFFFF >> (24 - w);
	
	GPIOx->DATA |= (bits << n);
}

/****************************************************************************************************************************************** 
* ��������:	GPIO_ClrBits()
* ����˵��: ������ָ���Ĵ�n��ʼ��wλ�������ŵĵ�ƽ�õ�
* ��    ��: GPIO_TypeDef * GPIOx	    ָ��GPIO�˿ڣ���Чֵ����GPIOA��GPIOB��GPIOC��GPIOM��GPION��GPIOP	
*			uint32_t n		       ָ��GPIO���ţ���Чֵ����PIN0��PIN1��PIN2��... ... PIN22��PIN23
*			uint32_t w			   ָ��Ҫ�����ŵ�ƽ�õ͵����ŵĸ���
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void GPIO_ClrBits(GPIO_TypeDef * GPIOx, uint32_t n, uint32_t w)
{
	uint32_t bits;
	
	bits = 0xFFFFFF >> (24 - w);
	
	GPIOx->DATA &= ~(bits << n);
}

/****************************************************************************************************************************************** 
* ��������: GPIO_InvBits()
* ����˵��: ������ָ���Ĵ�n��ʼ��wλ�������ŵĵ�ƽ��ת
* ��    ��: GPIO_TypeDef * GPIOx	    ָ��GPIO�˿ڣ���Чֵ����GPIOA��GPIOB��GPIOC��GPIOM��GPION��GPIOP	
*			uint32_t n		       ָ��GPIO���ţ���Чֵ����PIN0��PIN1��PIN2��... ... PIN22��PIN23
*			uint32_t w			   ָ��Ҫ�����ŵ�ƽ��ת�����ŵĸ���
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void GPIO_InvBits(GPIO_TypeDef * GPIOx, uint32_t n, uint32_t w)
{
	uint32_t bits;
	
	bits = 0xFFFFFF >> (24 - w);
	
	GPIOx->DATA ^= (bits << n);
}

/****************************************************************************************************************************************** 
* ��������:	GPIO_GetBits()
* ����˵��: ��ȡ����ָ���Ĵ�n��ʼ��wλ�������ŵĵ�ƽ״̬
* ��    ��: GPIO_TypeDef * GPIOx	    ָ��GPIO�˿ڣ���Чֵ����GPIOA��GPIOB��GPIOC��GPIOM��GPION��GPIOP	
*			uint32_t n		       ָ��GPIO���ţ���Чֵ����PIN0��PIN1��PIN2��... ... PIN22��PIN23
*			uint32_t w			   ָ��Ҫ�����ŵ�ƽ�øߵ����ŵĸ���
* ��    ��: ����ָ���Ĵ�n��ʼ��wλ�������ŵĵ�ƽ״̬	0 �͵�ƽ	1 �ߵ�ƽ
*			����ֵ�ĵ�0λ��ʾ����n�ĵ�ƽ״̬������ֵ�ĵ�1λ��ʾ����n+1�ĵ�ƽ״̬... ...����ֵ�ĵ�wλ��ʾ����n+w�ĵ�ƽ״̬
* ע������: ��
******************************************************************************************************************************************/
uint32_t GPIO_GetBits(GPIO_TypeDef * GPIOx, uint32_t n, uint32_t w)
{
	uint32_t bits;
	
   	bits = 0xFFFFFF >> (24 - w);
	
	return ((GPIOx->DATA >> n) & bits);
}
