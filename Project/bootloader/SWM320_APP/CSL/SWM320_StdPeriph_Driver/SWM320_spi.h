#ifndef __SWM320_SPI_H__
#define __SWM320_SPI_H__

typedef struct {
	uint8_t  FrameFormat;	//֡��ʽ��SPI_FORMAT_SPI��SPI_FORMAT_TI_SSI
	uint8_t  SampleEdge;	//��SPI֡��ʽ�£�ѡ�����ݲ������أ�SPI_FIRST_EDGE��SPI_SECOND_EDGE
	uint8_t  IdleLevel;		//��SPI֡��ʽ�£�ѡ�����ʱ�������ݴ���ʱ��ʱ���ߵĵ�ƽ��SPI_LOW_LEVEL��SPI_HIGH_LEVEL
	uint8_t  WordSize;		//�ֳ���, ��Чֵ4-16
	uint8_t  Master;		//1 ����ģʽ    0 �ӻ�ģʽ
	uint8_t  clkDiv;		//SPI_CLK = SYS_CLK / clkDiv����Чֵ��SPI_CLKDIV_4��SPI_CLKDIV_8��... ... ��SPI_CLKDIV_512
	
	uint8_t  RXHFullIEn;	//����FIFO�����ж�ʹ��
	uint8_t  TXEmptyIEn;	//����FIFO  ���ж�ʹ��
	uint8_t  TXCompleteIEn;	//����FIFO  ���ҷ�����λ�Ĵ������ж�ʹ��
} SPI_InitStructure;

#define SPI_FORMAT_SPI			0		//Motorola SPI ��ʽ
#define SPI_FORMAT_TI_SSI		1		//TI SSI ��ʽ

#define SPI_FIRST_EDGE			0		//��һ��ʱ���ؿ�ʼ����
#define SPI_SECOND_EDGE			1		//�ڶ���ʱ���ؿ�ʼ����

#define SPI_LOW_LEVEL			0		//����ʱʱ���߱��ֵ͵�ƽ
#define SPI_HIGH_LEVEL			1		//����ʱʱ���߱��ָߵ�ƽ

#define SPI_CLKDIV_4			0
#define SPI_CLKDIV_8			1
#define SPI_CLKDIV_16			2
#define SPI_CLKDIV_32			3
#define SPI_CLKDIV_64			4
#define SPI_CLKDIV_128			5
#define SPI_CLKDIV_256			6
#define SPI_CLKDIV_512			7



void SPI_Init(SPI_TypeDef * SPIx, SPI_InitStructure * initStruct);		//SPI��ʼ��
void SPI_Open(SPI_TypeDef * SPIx);										//SPI�򿪣������շ�
void SPI_Close(SPI_TypeDef * SPIx);										//SPI�رգ���ֹ�շ�

uint32_t SPI_Read(SPI_TypeDef * SPIx);
void SPI_Write(SPI_TypeDef * SPIx, uint32_t data);
void SPI_WriteWithWait(SPI_TypeDef * SPIx, uint32_t data);
uint32_t SPI_ReadWrite(SPI_TypeDef * SPIx, uint32_t data);

uint32_t SPI_IsRXEmpty(SPI_TypeDef * SPIx);				//����FIFO�Ƿ�գ������������Լ���SPI_Read()
uint32_t SPI_IsTXFull(SPI_TypeDef * SPIx);				//����FIFO�Ƿ����������������Լ���SPI_Write()
uint32_t SPI_IsTXEmpty(SPI_TypeDef * SPIx);				//����FIFO�Ƿ��


void SPI_INTRXHalfFullEn(SPI_TypeDef * SPIx);
void SPI_INTRXHalfFullDis(SPI_TypeDef * SPIx);
uint32_t SPI_INTRXHalfFullStat(SPI_TypeDef * SPIx);
void SPI_INTRXFullEn(SPI_TypeDef * SPIx);
void SPI_INTRXFullDis(SPI_TypeDef * SPIx);
uint32_t SPI_INTRXFullStat(SPI_TypeDef * SPIx);
void SPI_INTRXOverflowEn(SPI_TypeDef * SPIx);
void SPI_INTRXOverflowDis(SPI_TypeDef * SPIx);
void SPI_INTRXOverflowClr(SPI_TypeDef * SPIx);
uint32_t SPI_INTRXOverflowStat(SPI_TypeDef * SPIx);

void SPI_INTTXHalfFullEn(SPI_TypeDef * SPIx);
void SPI_INTTXHalfFullDis(SPI_TypeDef * SPIx);
uint32_t SPI_INTTXHalfFullStat(SPI_TypeDef * SPIx);
void SPI_INTTXEmptyEn(SPI_TypeDef * SPIx);
void SPI_INTTXEmptyDis(SPI_TypeDef * SPIx);
uint32_t SPI_INTTXEmptyStat(SPI_TypeDef * SPIx);
void SPI_INTTXCompleteEn(SPI_TypeDef * SPIx);
void SPI_INTTXCompleteDis(SPI_TypeDef * SPIx);
void SPI_INTTXCompleteClr(SPI_TypeDef * SPIx);
uint32_t SPI_INTTXCompleteStat(SPI_TypeDef * SPIx);


#endif //__SWM320_SPI_H__
