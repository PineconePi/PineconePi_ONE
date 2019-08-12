#ifndef __SWM320_TIMR_H__
#define __SWM320_TIMR_H__

#define TIMR_MODE_TIMER		0
#define TIMR_MODE_COUNTER	1

void TIMR_Init(TIMR_TypeDef * TIMRx, uint32_t mode, uint32_t period, uint32_t int_en);	//��ʱ��/��������ʼ��
void TIMR_Start(TIMR_TypeDef * TIMRx);							//������ʱ�����ӳ�ʼֵ��ʼ��ʱ/����
void TIMR_Stop(TIMR_TypeDef * TIMRx);							//ֹͣ��ʱ��
void TIMR_Halt(TIMR_TypeDef * TIMRx);							//��ͣ��ʱ��������ֵ���ֲ���
void TIMR_Resume(TIMR_TypeDef * TIMRx);							//�ָ���ʱ��������ͣ����������

void TIMR_SetPeriod(TIMR_TypeDef * TIMRx, uint32_t period);		//���ö�ʱ/��������
uint32_t TIMR_GetPeriod(TIMR_TypeDef * TIMRx);					//��ȡ��ʱ/��������
uint32_t TIMR_GetCurValue(TIMR_TypeDef * TIMRx);				//��ȡ��ǰ����ֵ

void TIMR_INTEn(TIMR_TypeDef * TIMRx);							//ʹ���ж�
void TIMR_INTDis(TIMR_TypeDef * TIMRx);							//�����ж�
void TIMR_INTClr(TIMR_TypeDef * TIMRx);							//����жϱ�־
uint32_t TIMR_INTStat(TIMR_TypeDef * TIMRx);					//��ȡ�ж�״̬


#endif //__SWM320_TIMR_H__
