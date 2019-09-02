#ifndef __SWM320_I2C_H__
#define __SWM320_I2C_H__

typedef struct {
	uint8_t  Master;		//1 ����ģʽ
	uint8_t  Addr7b;		//1 7λ��ַ     0 10λ��ַ
	
	uint32_t MstClk;		//��������ʱ��Ƶ��
	uint8_t  MstIEn;		//����ģʽ�ж�ʹ��
	
	uint16_t SlvAddr;		//�ӻ���ַ
	uint8_t  SlvRxEndIEn;	//�ӻ���������ж�ʹ��
	uint8_t  SlvTxEndIEn;	//�ӻ���������ж�ʹ��
	uint8_t  SlvSTADetIEn;	//�ӻ���⵽��ʼ�ж�ʹ��
	uint8_t  SlvSTODetIEn;	//�ӻ���⵽��ֹ�ж�ʹ��
	uint8_t  SlvRdReqIEn;	//�ӻ����յ��������ж�ʹ��
	uint8_t  SlvWrReqIEn;	//�ӻ����յ�д�����ж�ʹ��
} I2C_InitStructure;


void I2C_Init(I2C_TypeDef * I2Cx, I2C_InitStructure * initStruct);

void I2C_Open(I2C_TypeDef * I2Cx);
void I2C_Close(I2C_TypeDef * I2Cx);

#endif //__SWM320_I2C_H__
