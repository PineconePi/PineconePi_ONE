#ifndef __SWM320_WDT_H__
#define	__SWM320_WDT_H__

#define WDT_MODE_RESET		0
#define WDT_MODE_INTERRUPT	1

void WDT_Init(WDT_TypeDef * WDTx, uint32_t peroid, uint32_t mode);	//WDT���Ź���ʼ��
void WDT_Start(WDT_TypeDef * WDTx);			//����ָ��WDT����ʼ����ʱ
void WDT_Stop(WDT_TypeDef * WDTx);			//�ر�ָ��WDT��ֹͣ����ʱ

void WDT_Feed(WDT_TypeDef * WDTx);			//ι�������´�װ��ֵ��ʼ����ʱ

int32_t WDT_GetValue(WDT_TypeDef * WDTx);	//��ȡָ�����Ź���ʱ���ĵ�ǰ����ʱֵ


void WDT_INTClr(WDT_TypeDef * WDTx);		//�жϱ�־���
uint32_t WDT_INTStat(WDT_TypeDef * WDTx);	//�ж�״̬��ѯ
 
#endif //__SWM320_WDT_H__
