#ifndef __GD25Q21_H__
#define __GD25Q21_H__


void GD25Q21_Init(void);
uint16_t GD25Q21_ReadChipID(void);
uint16_t GD25Q21_ReadStatus(void);

uint32_t GD25Q21_IsBusy(void);

void GD25Q21_EraseSector(uint32_t addr);
void GD25Q21_ReadData(uint32_t addr, uint8_t buff[], uint32_t cnt);
void GD25Q21_WritePage(uint32_t addr, uint8_t data[], uint16_t cnt);

#define GD25Q21_STATUS_WIP_Pos		0		// Write In Progress
#define GD25Q21_STATUS_WIP_Msk		(0x01 << GD25Q21_STATUS_WIP_Pos)
#define GD25Q21_STATUS_WEL_Pos		1		// Write Enable Latch
#define GD25Q21_STATUS_WEL_Msk		(0x01 << GD25Q21_STATUS_WEL_Pos)
#define GD25Q21_STATUS_SBP_Pos		2		// Software Block Protect
#define GD25Q21_STATUS_SBP_Msk		(0x1F << GD25Q21_STATUS_SBP_Pos)
#define GD25Q21_STATUS_SRP_Pos		7		// Status Register Protect
#define GD25Q21_STATUS_SRP_Msk		(0x03 << GD25Q21_STATUS_SRP_Pos)
#define GD25Q21_STATUS_SUS_Pos		15		// Erase/Program Suspend
#define GD25Q21_STATUS_SUS_Msk		(0x01 << GD25Q21_STATUS_SUS_Pos)


#define GD25Q21_MANUFACTOR_ID	0xC8
#define GD25Q21_DEVICE_ID		0x11


#define GD25Q21_CMD_READ_CHIP_ID		0x90
#define GD25Q21_CMD_READ_STATUS_L		0x05
#define GD25Q21_CMD_READ_STATUS_H		0x35
#define GD25Q21_CMD_READ_DATA			0x03
#define GD25Q21_CMD_WRITE_PAGE			0x02
#define GD25Q21_CMD_ERASE_SECTOR		0x20
#define GD25Q21_CMD_WRITE_ENABLE		0x06
#define GD25Q21_CMD_WRITE_DISABLE		0x04



#endif //__GD25Q21_H__
