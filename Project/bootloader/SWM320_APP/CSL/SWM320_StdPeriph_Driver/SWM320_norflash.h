#ifndef __SWM320_NORFLASH_H__
#define __SWM320_NORFLASH_H__

typedef struct {
	uint8_t DataWidth;			// 8��16
	
	uint8_t WELowPulseTime;		// WE# pulse width����λΪϵͳʱ�����ڣ����ֵΪ7
	uint8_t OEPreValidTime;		// Valid data output after OE# low����λΪϵͳʱ�����ڣ����ֵΪ15
	
	uint8_t OperFinishIEn;		// ����(д�롢����)����ж�ʹ��
	uint8_t OperTimeoutIEn;
} NORFL_InitStructure;



void NORFL_Init(NORFL_InitStructure * initStruct);
uint32_t NORFL_ChipErase(void);
uint32_t NORFL_SectorErase(uint32_t addr);
uint32_t NORFL_Write(uint32_t addr, uint32_t data);
uint32_t NORFL_Read(uint32_t addr);
uint16_t NORFL_ReadID(uint32_t id_addr);


/* ��ǰ�汾���߶�ֻ֧���ֶ�
#define NORFL_Read8(addr)			*((volatile uint8_t  *)(NORFLM_BASE + addr))
#define NORFL_Read16(addr)			*((volatile uint16_t *)(NORFLM_BASE + addr))    */
#define NORFL_Read32(addr)			*((volatile uint32_t *)(NORFLM_BASE + addr))



#define NORFL_CMD_READ              0
#define NORFL_CMD_RESET             1
#define NORFL_CMD_AUTO_SELECT       2
#define NORFL_CMD_PROGRAM           3
#define NORFL_CMD_CHIP_ERASE        4
#define NORFL_CMD_SECTOR_ERASE      5

#endif // __SWM320_NORFLASH_H__
