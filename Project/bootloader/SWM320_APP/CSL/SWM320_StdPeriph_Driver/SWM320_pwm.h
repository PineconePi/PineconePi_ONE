#ifndef __SWM320_PWM_H__
#define	__SWM320_PWM_H__

typedef struct {
	uint8_t  clk_div;		//PWM_CLKDIV_1��PWM_CLKDIV_8
	
	uint8_t  mode;			//PWM_MODE_INDEP��PWM_MODE_COMPL��PWM_MODE_INDEP_CALIGN��PWM_MODE_COMPL_CALIGN
	
	uint16_t cycleA;		//A·����
	uint16_t hdutyA;		//A·ռ�ձ�
	uint16_t deadzoneA;		//A·����ʱ����ȡֵ0--1023
	uint8_t  initLevelA;	//A·��ʼ�����ƽ��0 �͵�ƽ    1 �ߵ�ƽ
	
	uint16_t cycleB;		//B·����
	uint16_t hdutyB;		//B·ռ�ձ�
	uint16_t deadzoneB;		//B·����ʱ����ȡֵ0--1023
	uint8_t  initLevelB;	//B·��ʼ�����ƽ��0 �͵�ƽ    1 �ߵ�ƽ
	
	uint8_t  HEndAIEn;		//A·�ߵ�ƽ�����ж�ʹ��
	uint8_t  NCycleAIEn;	//A·�����ڿ�ʼ�ж�ʹ��
	uint8_t  HEndBIEn;		//B·�ߵ�ƽ�����ж�ʹ��
	uint8_t  NCycleBIEn;	//B·�����ڿ�ʼ�ж�ʹ��
} PWM_InitStructure;

#define PWM_CLKDIV_1	0
#define PWM_CLKDIV_8	1

#define PWM_MODE_INDEP			0		//A·��B·Ϊ��·�������
#define PWM_MODE_COMPL			1		//A·��B·Ϊһ·�������
#define PWM_MODE_INDEP_CALIGN	3		//A·��B·Ϊ��·������������Ķ���
#define PWM_MODE_COMPL_CALIGN	4		//A·��B·Ϊһ·������������Ķ���

#define PWM_CH_A	0
#define PWM_CH_B	1


void PWM_Init(PWM_TypeDef * PWMx, PWM_InitStructure * initStruct);			//PWM��ʼ��
void PWM_Start(PWM_TypeDef * PWMx, uint32_t chA, uint32_t chB);				//����PWM����ʼPWM���
void PWM_Stop(PWM_TypeDef * PWMx, uint32_t chA, uint32_t chB);				//�ر�PWM��ֹͣPWM���

void PWM_SetCycle(PWM_TypeDef * PWMx, uint32_t chn, uint16_t cycle);		//��������
uint16_t PWM_GetCycle(PWM_TypeDef * PWMx, uint32_t chn);	   				//��ȡ����
void PWM_SetHDuty(PWM_TypeDef * PWMx, uint32_t chn, uint16_t hduty);		//���øߵ�ƽʱ��
uint16_t PWM_GetHDuty(PWM_TypeDef * PWMx, uint32_t chn);					//��ȡ�ߵ�ƽʱ��
void PWM_SetDeadzone(PWM_TypeDef * PWMx, uint32_t chn, uint8_t deadzone);	//��������ʱ��
uint8_t  PWM_GetDeadzone(PWM_TypeDef * PWMx, uint32_t chn);		 			//��ȡ����ʱ��

void PWM_IntNCycleEn(PWM_TypeDef * PWMx, uint32_t chn);   					//�����ڿ�ʼ�ж�ʹ��
void PWM_IntNCycleDis(PWM_TypeDef * PWMx, uint32_t chn);  					//�����ڿ�ʼ�жϽ���
void PWM_IntNCycleClr(PWM_TypeDef * PWMx, uint32_t chn);  					//�����ڿ�ʼ�жϱ�־���
uint32_t PWM_IntNCycleStat(PWM_TypeDef * PWMx, uint32_t chn); 				//�����ڿ�ʼ�ж��Ƿ���
void PWM_IntHEndEn(PWM_TypeDef * PWMx, uint32_t chn); 						//�ߵ�ƽ�����ж�ʹ��
void PWM_IntHEndDis(PWM_TypeDef * PWMx, uint32_t chn);						//�ߵ�ƽ�����жϽ���
void PWM_IntHEndClr(PWM_TypeDef * PWMx, uint32_t chn);	 					//�ߵ�ƽ�����жϱ�־���
uint32_t PWM_IntHEndStat(PWM_TypeDef * PWMx, uint32_t chn);					//�ߵ�ƽ�����ж��Ƿ���


#endif //__SWM320_PWM_H__
