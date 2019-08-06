#ifndef __BSP_SPIFlash_H
#define __BSP_SPIFlash_H

#include "SWM320.h"

//#define  sFLASH_ID              0xEF3015   //W25X16
//#define  sFLASH_ID              0xEF4015	 //W25Q16
#define  sFLASH_ID              0XEF4018   //W25Q128
//#define  sFLASH_ID              0XEF4017    //W25Q64

#define SPI_FLASH_PageSize              256  //读
#define SPI_FLASH_PerWritePageSize      256   //写

/*命令定义-开头*******************************/
#define W25X_WriteEnable		      0x06 
#define W25X_WriteDisable		      0x04 
#define W25X_ReadStatusReg		    0x05 
#define W25X_WriteStatusReg		    0x01 
#define W25X_ReadData			        0x03 
#define W25X_FastReadData		      0x0B 
#define W25X_FastReadDual		      0x3B 
#define W25X_PageProgram		      0x02 
#define W25X_BlockErase			      0xD8 
#define W25X_SectorErase		      0x20 
#define W25X_ChipErase			      0xC7 
#define W25X_PowerDown			      0xB9 
#define W25X_ReleasePowerDown	    0xAB 
#define W25X_DeviceID			     0xAB 
#define W25X_ManufactDeviceID   	0x90 
#define W25X_JedecDeviceID		    0x9F

/* WIP(busy)标志，FLASH内部正在写入 */
#define WIP_Flag                  0x01
#define Dummy_Byte                0xFF

#define SPIFlash_CSn_Clr  		GPIO_ClrBit(GPIOP, PIN23)				// SPI片选信号 拉低	
#define SPIFlash_CSn_Set  		GPIO_SetBit(GPIOP, PIN23)				// SPI片选信号 置高

/*等待超时时间*/
#define SPIT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define SPIT_LONG_TIMEOUT         ((uint32_t)(10 * SPIT_FLAG_TIMEOUT))

uint8_t SPI_Flash_Init(void); //0:成功 1:失败

void SPI_Flash_PowerDown(void);
uint32_t SPI_FLASH_ReadDeviceID(void);
uint32_t SPI_FLASH_ReadID(void);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);
void SPI_FLASH_SectorErase(uint32_t SectorAddr);
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_Flash_WAKEUP(void);
#endif
