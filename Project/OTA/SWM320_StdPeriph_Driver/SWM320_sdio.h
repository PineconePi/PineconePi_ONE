#ifndef __SWM320_SDIO_H__
#define __SWM320_SDIO_H__


#define SD_CMD_GO_IDLE_STATE                       ((uint8_t)0)
#define SD_CMD_SEND_OP_COND                        ((uint8_t)1)
#define SD_CMD_ALL_SEND_CID                        ((uint8_t)2)
#define SD_CMD_SET_REL_ADDR                        ((uint8_t)3)
#define SD_CMD_SET_DSR                             ((uint8_t)4)
#define SD_CMD_HS_SWITCH                           ((uint8_t)6)
#define SD_CMD_SEL_DESEL_CARD                      ((uint8_t)7)
#define SD_CMD_SEND_IF_COND                        ((uint8_t)8)
#define SD_CMD_SEND_CSD                            ((uint8_t)9)
#define SD_CMD_SEND_CID                            ((uint8_t)10)
#define SD_CMD_STOP_TRANSMISSION                   ((uint8_t)12)
#define SD_CMD_SEND_STATUS                         ((uint8_t)13)
#define SD_CMD_SET_BLOCKLEN                        ((uint8_t)16)
#define SD_CMD_READ_SINGLE_BLOCK                   ((uint8_t)17)
#define SD_CMD_READ_MULT_BLOCK                     ((uint8_t)18)
#define SD_CMD_WRITE_SINGLE_BLOCK                  ((uint8_t)24)
#define SD_CMD_WRITE_MULT_BLOCK                    ((uint8_t)25)
#define SD_CMD_PROG_CID                            ((uint8_t)26)
#define SD_CMD_PROG_CSD                            ((uint8_t)27)
#define SD_CMD_APP_CMD                             ((uint8_t)55)

/*Following commands are SD Card Specific commands.
  SDIO_APP_CMD should be sent before sending these commands. */
#define SD_CMD_APP_SD_SET_BUSWIDTH                 ((uint8_t)6)
#define SD_CMD_SD_APP_STAUS                        ((uint8_t)13)
#define SD_CMD_SD_APP_SEND_NUM_WRITE_BLOCKS        ((uint8_t)22)
#define SD_CMD_SD_APP_OP_COND                      ((uint8_t)41)
#define SD_CMD_SD_APP_SET_CLR_CARD_DETECT          ((uint8_t)42)
#define SD_CMD_SD_APP_SEND_SCR                     ((uint8_t)51)
#define SD_CMD_SDIO_RW_DIRECT                      ((uint8_t)52)
#define SD_CMD_SDIO_RW_EXTENDED                    ((uint8_t)53)


#define SD_RESP_NO			0	//0 ����Ӧ
#define SD_RESP_32b			2	//2 32λ��Ӧ
#define SD_RESP_128b		1	//1 128λ��Ӧ
#define SD_RESP_32b_busy	3	//3 32λ��Ӧ��check Busy after response

#define SD_CLK_400KHz		0
#define SD_CLK_20MHz		1

#define SD_BUSWIDTH_1b		0
#define SD_BUSWIDTH_4b		2

#define SD_RES_OK			0
#define SD_RES_ERR			1


typedef struct
{
  __IO uint8_t  CSDStruct;            // CSD structure 
  __IO uint8_t  SysSpecVersion;       // System specification version 
  __IO uint8_t  Reserved1;            // Reserved 
  __IO uint8_t  TAAC;                 // Data read access-time 1 
  __IO uint8_t  NSAC;                 // Data read access-time 2 in CLK cycles 
  __IO uint8_t  MaxBusClkFrec;        // Max. bus clock frequency 
  __IO uint16_t CardComdClasses;      //< Card command classes 
  __IO uint8_t  RdBlockLen;           // Max. read data block length 
  __IO uint8_t  PartBlockRead;        // Partial blocks for read allowed 
  __IO uint8_t  WrBlockMisalign;      // Write block misalignment 
  __IO uint8_t  RdBlockMisalign;      // Read block misalignment 
  __IO uint8_t  DSRImpl;              // DSR implemented 
  __IO uint8_t  Reserved2;            // Reserved 
  __IO uint32_t DeviceSize;           // Device Size 
  __IO uint8_t  MaxRdCurrentVDDMin;   // Max. read current @ VDD min 
  __IO uint8_t  MaxRdCurrentVDDMax;   // Max. read current @ VDD max 
  __IO uint8_t  MaxWrCurrentVDDMin;   // Max. write current @ VDD min 
  __IO uint8_t  MaxWrCurrentVDDMax;   // Max. write current @ VDD max 
  __IO uint8_t  DeviceSizeMul;        // Device size multiplier 
  __IO uint8_t  EraseGrSize;          // Erase group size 
  __IO uint8_t  EraseGrMul;           // Erase group size multiplier 
  __IO uint8_t  WrProtectGrSize;      // Write protect group size 
  __IO uint8_t  WrProtectGrEnable;    // Write protect group enable 
  __IO uint8_t  ManDeflECC;           // Manufacturer default ECC 
  __IO uint8_t  WrSpeedFact;          // Write speed factor 
  __IO uint8_t  MaxWrBlockLen;        // Max. write data block length 
  __IO uint8_t  WriteBlockPaPartial;  // Partial blocks for write allowed 
  __IO uint8_t  Reserved3;            // Reserded 
  __IO uint8_t  ContentProtectAppli;  // Content protection application 
  __IO uint8_t  FileFormatGrouop;     // File format group 
  __IO uint8_t  CopyFlag;             // Copy flag (OTP) 
  __IO uint8_t  PermWrProtect;        // Permanent write protection 
  __IO uint8_t  TempWrProtect;        // Temporary write protection 
  __IO uint8_t  FileFormat;           // File Format 
  __IO uint8_t  ECC;                  // ECC code 
} SD_CSD;

typedef struct
{
  __IO uint8_t  ManufacturerID;       // ManufacturerID 
  __IO uint16_t OEM_AppliID;          // OEM/Application ID 
  __IO uint32_t ProdName1;            // Product Name part1 
  __IO uint8_t  ProdName2;            // Product Name part2
  __IO uint8_t  ProdRev;              // Product Revision 
  __IO uint32_t ProdSN;               // Product Serial Number 
  __IO uint8_t  Reserved1;            // Reserved1 
  __IO uint16_t ManufactDate;         // Manufacturing Date 
} SD_CID;


#define SDIO_STD_CAPACITY_SD_CARD_V1_1             ((uint32_t)0x00000000)
#define SDIO_STD_CAPACITY_SD_CARD_V2_0             ((uint32_t)0x00000001)
#define SDIO_HIGH_CAPACITY_SD_CARD                 ((uint32_t)0x00000002)
#define SDIO_MULTIMEDIA_CARD                       ((uint32_t)0x00000003)
#define SDIO_SECURE_DIGITAL_IO_CARD                ((uint32_t)0x00000004)
#define SDIO_HIGH_SPEED_MULTIMEDIA_CARD            ((uint32_t)0x00000005)
#define SDIO_SECURE_DIGITAL_IO_COMBO_CARD          ((uint32_t)0x00000006)
#define SDIO_HIGH_CAPACITY_MMC_CARD                ((uint32_t)0x00000007)


typedef struct
{
  SD_CSD SD_csd;
  SD_CID SD_cid;
  uint64_t CardCapacity;  // Card Capacity 
  uint32_t CardBlockSize; // Card Block Size 
  uint16_t RCA;
  uint8_t CardType;
} SD_CardInfo;


extern SD_CardInfo SD_cardInfo;

uint32_t SDIO_Init(void);
void SDIO_BlockWrite(uint32_t block_addr, uint32_t buff[]);
void SDIO_BlockRead(uint32_t block_addr, uint32_t buff[]);

void SDIO_SendCmd(uint32_t cmd, uint32_t arg, uint32_t resp_type, uint32_t *resp_data, uint32_t have_data, uint32_t data_read);

void parseCID(uint32_t CID_Tab[4]);
void parseCSD(uint32_t CID_Tab[4]);

uint32_t calcSDCLKDiv(uint32_t freq_sel);

#endif //__SWM320_SDIO_H__
