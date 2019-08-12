/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "ff.h"
#include "bsp_sdio.h"
#include "bsp_SPIFlash.h"
 
/* 为每个设备定义一个物理编号 */
//#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		0	/* Example: Map MMC/SD card to physical drive 1 */
#define SPI_FLASH		1	/* Example: Map USB MSD to physical drive 2 */
//#define DEV_USB		2	/* Example: Map SPI_FLASH to physical drive 3 */

/*-----------------------------------------------------------------------*/
/* 获取设备状态                                                          */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status (
	BYTE pdrv		/* 物理编号 */
)
{
	DSTATUS status = STA_NOINIT;
	
	switch (pdrv) {
		case DEV_MMC:				/* SD CARD */
			status &= ~STA_NOINIT;
			break;
		
    case SPI_FLASH:      
      /* SPI Flash状态检测：读取SPI Flash 设备ID */
      if(sFLASH_ID == SPI_FLASH_ReadID())
      {
        /* 设备ID读取结果正确 */
        status &= ~STA_NOINIT;
      }
      else
      {
        /* 设备ID读取结果错误 */
        status = STA_NOINIT;;
      }
			break;
		default:
			status = STA_NOINIT;
	}
	return status;
}

/*-----------------------------------------------------------------------*/
/* 设备初始化                                                            */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize (
	BYTE pdrv				/* 物理编号 */
)
{
	uint16_t i;
	DSTATUS status = STA_NOINIT;	
	switch (pdrv) {
		case DEV_MMC:	         /* SD CARD */
			if(bsp_SDIOInit()== SD_RES_OK)
			{
				status = RES_OK;
			}
			else 
			{
				status = STA_NOINIT;
			}
		
			break;
			
      case SPI_FLASH:    /* SPI Flash */ 
      /* 初始化SPI Flash */
			SPI_Flash_Init();
      /* 延时一小段时间 */
      i=500;
	    while(--i);	
      /* 获取SPI Flash芯片状态 */
			SPI_Flash_WAKEUP();
      status=disk_status(SPI_FLASH);
			break;
		default:
			status = STA_NOINIT;
	}
	return status;
}


/*-----------------------------------------------------------------------*/
/* 读扇区：读取扇区内容到指定存储区                                              */
/*-----------------------------------------------------------------------*/
DRESULT disk_read (
	BYTE pdrv,		/* 设备物理编号(0..) */
	BYTE *buff,		/* 数据缓存区 */
	DWORD sector,	/* 扇区首地址 */
	UINT count		/* 扇区个数(1..128) */
)
{
	DRESULT status = RES_PARERR;
	
	switch (pdrv) {
		case DEV_MMC:			/* SD CARD */
			SDIO_BlockRead(sector, (uint32_t *)buff); // 调用库函数读扇区
			status = RES_OK;
		break;
    
		case SPI_FLASH:
      /* 扇区偏移6MB，外部Flash文件系统空间放在SPI Flash后面10MB空间 */
      sector+=1536;      
      SPI_FLASH_BufferRead(buff, sector <<12, count<<12);
      status = RES_OK;
		break;
				
		default:
			status = RES_PARERR;
	}
	return status;
}

/*-----------------------------------------------------------------------*/
/* 写扇区：讲数据写入指定扇区空间上                                      */
/*-----------------------------------------------------------------------*/
#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			  /* 设备物理编号(0..) */
	const BYTE *buff,	/* 欲写入数据的缓存区 */
	DWORD sector,		  /* 扇区首地址 */
	UINT count			  /* 扇区个数(1..128) */
)
{
	uint32_t write_addr; 
	DRESULT status = RES_PARERR;
	
	if (!count) {
		return RES_PARERR;		/* Check parameter */
	}

	switch (pdrv) {
		case DEV_MMC:	/* SD CARD */  
			SDIO_BlockWrite(sector, (uint32_t *)buff); // 调用库函数写扇区
			status = RES_OK;
		break;
		
		case SPI_FLASH:
      /* 扇区偏移6MB，外部Flash文件系统空间放在SPI Flash后面10MB空间 */
			sector+=1536;
      write_addr = sector<<12;    
      SPI_FLASH_SectorErase(write_addr);
      SPI_FLASH_BufferWrite((uint8_t *)buff,write_addr,count<<12);
      status = RES_OK;
		break;
		
		default:
			status = RES_PARERR;
	}
	return status;
}
#endif


/*-----------------------------------------------------------------------*/
/* 其他控制                                                              */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* 物理编号 */
	BYTE cmd,		  /* 控制指令 */
	void *buff		/* 写入或者读取数据地址指针 */
)
{
	DRESULT status = RES_PARERR;
	switch (pdrv) {
		case DEV_MMC:	/* SD CARD */
			switch (cmd) 
			{
				// Get R/W sector size (WORD) 
				case GET_SECTOR_SIZE :    
					*(WORD * )buff = 512;
				break;
				// Get erase block size in unit of sector (DWORD)
				case GET_BLOCK_SIZE :      
					*(DWORD * )buff = 512*8;
				break;

				case GET_SECTOR_COUNT:
					*(DWORD * )buff = 31250000;		// 这个可以用文件系统的函数获取，这里是笔者算出的扇区数(16000 000 000 / 512)
					break;
				case CTRL_SYNC :
				break;
			}
			status = RES_OK;
			break;
 
		case SPI_FLASH:
			switch (cmd) {
        /* 扇区数量：2560*4096/1024/1024=10(MB) */
        case GET_SECTOR_COUNT:
          *(DWORD * )buff = 2560;		
        break;
        /* 扇区大小  */
        case GET_SECTOR_SIZE :
          *(WORD * )buff = 4096;
        break;
        /* 同时擦除扇区个数 */
        case GET_BLOCK_SIZE :
          *(DWORD * )buff = 1;
        break;        
      }
      status = RES_OK;
		break;
			
		default:
			status = RES_PARERR;
	}
	return status;
}
#endif

