#ifndef __SWM320_GPIO_H__
#define	__SWM320_GPIO_H__


void GPIO_Init(GPIO_TypeDef * GPIOx, uint32_t n, uint32_t dir, uint32_t pull_up, uint32_t pull_down);	//���ų�ʼ�����������ŷ����������衢��������

void GPIO_SetBit(GPIO_TypeDef * GPIOx, uint32_t n);						//������ָ�������ŵ�ƽ�ø�
void GPIO_ClrBit(GPIO_TypeDef * GPIOx, uint32_t n);						//������ָ�������ŵ�ƽ�õ�
void GPIO_InvBit(GPIO_TypeDef * GPIOx, uint32_t n);						//������ָ�������ŵ�ƽ��ת
uint32_t GPIO_GetBit(GPIO_TypeDef * GPIOx, uint32_t n);					//��ȡ����ָ�������ŵĵ�ƽ״̬
void GPIO_SetBits(GPIO_TypeDef * GPIOx, uint32_t n, uint32_t w);		//������ָ���Ĵ�n��ʼ��wλ�������ŵĵ�ƽ�ø�
void GPIO_ClrBits(GPIO_TypeDef * GPIOx, uint32_t n, uint32_t w);		//������ָ���Ĵ�n��ʼ��wλ�������ŵĵ�ƽ�õ�
void GPIO_InvBits(GPIO_TypeDef * GPIOx, uint32_t n, uint32_t w);		//������ָ���Ĵ�n��ʼ��wλ�������ŵĵ�ƽ��ת
uint32_t GPIO_GetBits(GPIO_TypeDef * GPIOx, uint32_t n, uint32_t w);	//��ȡ����ָ���Ĵ�n��ʼ��wλ�������ŵĵ�ƽ״̬

void GPIO_AtomicSetBit(GPIO_TypeDef * GPIOx, uint32_t n);
void GPIO_AtomicClrBit(GPIO_TypeDef * GPIOx, uint32_t n);
void GPIO_AtomicInvBit(GPIO_TypeDef * GPIOx, uint32_t n);
void GPIO_AtomicSetBits(GPIO_TypeDef * GPIOx, uint32_t n, uint32_t w);
void GPIO_AtomicClrBits(GPIO_TypeDef * GPIOx, uint32_t n, uint32_t w);
void GPIO_AtomicInvBits(GPIO_TypeDef * GPIOx, uint32_t n, uint32_t w);


#endif //__SWM320_GPIO_H__
