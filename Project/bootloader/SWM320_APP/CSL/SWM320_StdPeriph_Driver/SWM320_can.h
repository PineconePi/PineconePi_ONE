#ifndef __SWM320_CAN_H__
#define __SWM320_CAN_H__

#define CAN_FRAME_STD	0
#define CAN_FRAME_EXT	1

typedef struct {
	uint8_t  Mode;			//CAN_MODE_NORMAL��CAN_MODE_LISTEN��CAN_MODE_SELFTEST
	uint8_t  CAN_BS1;		//CAN_BS1_1tq��CAN_BS1_2tq��... ... ��CAN_BS1_16tq
	uint8_t  CAN_BS2;		//CAN_BS2_1tq��CAN_BS2_2tq��... ... ��CAN_BS2_8tq
	uint8_t  CAN_SJW;		//CAN_SJW_1tq��CAN_SJW_2tq��CAN_SJW_3tq��CAN_SJW_4tq
	uint32_t Baudrate;		//�����ʣ���λ�������ʣ�ȡֵ1--1000000
	uint8_t  FilterMode;	//CAN_FILTER_16b��CAN_FILTER_32b
	union {
		uint32_t FilterMask32b;		//FilterCheck & (~FilterMask) == ID & (~FilterMask)��Messageͨ������
		struct {					// 0 must match    1 don't care
			uint16_t FilterMask16b1;
			uint16_t FilterMask16b2;
		};
	};
	union {
		uint32_t FilterCheck32b;
		struct {
			uint16_t FilterCheck16b1;
			uint16_t FilterCheck16b2;
		};
	};
	uint8_t  RXNotEmptyIEn;		//����FIFO�ǿգ������ݿɶ�
	uint8_t  RXOverflowIEn;		//����FIFO����������ݶ�ʧ
	uint8_t  ArbitrLostIEn;		//��������ʧ�ٲñ�ɽ��շ�
	uint8_t  ErrPassiveIEn;		//����/���ʹ������ֵ�ﵽ127
} CAN_InitStructure;

#define CAN_MODE_NORMAL		0	//����ģʽ
#define CAN_MODE_LISTEN		1	//����ģʽ
#define CAN_MODE_SELFTEST	2	//�Բ�ģʽ

#define CAN_BS1_1tq			0
#define CAN_BS1_2tq			1
#define CAN_BS1_3tq			2
#define CAN_BS1_4tq			3
#define CAN_BS1_5tq			4
#define CAN_BS1_6tq			5
#define CAN_BS1_7tq			6
#define CAN_BS1_8tq			7
#define CAN_BS1_9tq			8
#define CAN_BS1_10tq		9
#define CAN_BS1_11tq		10
#define CAN_BS1_12tq		11
#define CAN_BS1_13tq		12
#define CAN_BS1_14tq		13
#define CAN_BS1_15tq		14
#define CAN_BS1_16tq		15

#define CAN_BS2_1tq			0
#define CAN_BS2_2tq			1
#define CAN_BS2_3tq			2
#define CAN_BS2_4tq			3
#define CAN_BS2_5tq			4
#define CAN_BS2_6tq			5
#define CAN_BS2_7tq			6
#define CAN_BS2_8tq			7

#define CAN_SJW_1tq			0
#define CAN_SJW_2tq			1
#define CAN_SJW_3tq			2
#define CAN_SJW_4tq			3

#define CAN_FILTER_16b		0	//����16λ������
#define CAN_FILTER_32b		1	//һ��32λ������

typedef struct {
	uint32_t id;		//��ϢID
	uint8_t  format;	//֡��ʽ��CAN_FRAME_STD��CAN_FRAME_EXT
	uint8_t  remote;	//��Ϣ�Ƿ�ΪԶ��֡
	uint8_t  size;		//���յ������ݸ���
	uint8_t  data[8];	//���յ�������
} CAN_RXMessage;


void CAN_Init(CAN_TypeDef * CANx, CAN_InitStructure * initStruct);
void CAN_Open(CAN_TypeDef * CANx);
void CAN_Close(CAN_TypeDef * CANx);

void CAN_Transmit(CAN_TypeDef * CANx, uint32_t format, uint32_t id, uint8_t data[], uint32_t size, uint32_t once);
void CAN_TransmitRequest(CAN_TypeDef * CANx, uint32_t format, uint32_t id, uint32_t once);
void CAN_Receive(CAN_TypeDef * CANx, CAN_RXMessage *msg);

uint32_t CAN_TXComplete(CAN_TypeDef * CANx);
uint32_t CAN_TXSuccess(CAN_TypeDef * CANx);

void CAN_AbortTransmit(CAN_TypeDef * CANx);

uint32_t CAN_TXBufferReady(CAN_TypeDef * CANx);
uint32_t CAN_RXDataAvailable(CAN_TypeDef * CANx);

void CAN_SetBaudrate(CAN_TypeDef * CANx, uint32_t baudrate, uint32_t CAN_BS1, uint32_t CAN_BS2, uint32_t CAN_SJW);

void CAN_SetFilter32b(CAN_TypeDef * CANx, uint32_t check, uint32_t mask);
void CAN_SetFilter16b(CAN_TypeDef * CANx, uint16_t check1, uint16_t mask1, uint16_t check2, uint16_t mask2);


void CAN_INTRXNotEmptyEn(CAN_TypeDef * CANx);
void CAN_INTRXNotEmptyDis(CAN_TypeDef * CANx);
uint32_t CAN_INTRXNotEmptyStat(CAN_TypeDef * CANx);

void CAN_INTTXBufEmptyEn(CAN_TypeDef * CANx);
void CAN_INTTXBufEmptyDis(CAN_TypeDef * CANx);
uint32_t CAN_INTTXBufEmptyStat(CAN_TypeDef * CANx);

void CAN_INTErrWarningEn(CAN_TypeDef * CANx);
void CAN_INTErrWarningDis(CAN_TypeDef * CANx);
uint32_t CAN_INTErrWarningStat(CAN_TypeDef * CANx);

void CAN_INTRXOverflowEn(CAN_TypeDef * CANx);
void CAN_INTRXOverflowDis(CAN_TypeDef * CANx);
uint32_t CAN_INTRXOverflowStat(CAN_TypeDef * CANx);
void CAN_INTRXOverflowClear(CAN_TypeDef * CANx);

void CAN_INTWakeupEn(CAN_TypeDef * CANx);
void CAN_INTWakeupDis(CAN_TypeDef * CANx);
uint32_t CAN_INTWakeupStat(CAN_TypeDef * CANx);

void CAN_INTErrPassiveEn(CAN_TypeDef * CANx);
void CAN_INTErrPassiveDis(CAN_TypeDef * CANx);
uint32_t CAN_INTErrPassiveStat(CAN_TypeDef * CANx);

void CAN_INTArbitrLostEn(CAN_TypeDef * CANx);
void CAN_INTArbitrLostDis(CAN_TypeDef * CANx);
uint32_t CAN_INTArbitrLostStat(CAN_TypeDef * CANx);

void CAN_INTBusErrorEn(CAN_TypeDef * CANx);
void CAN_INTBusErrorDis(CAN_TypeDef * CANx);
uint32_t CAN_INTBusErrorStat(CAN_TypeDef * CANx);

#endif //__SWM320_CAN_H__
