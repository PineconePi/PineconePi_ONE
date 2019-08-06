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
 
/* Ϊÿ���豸����һ�������� */
//#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		0	/* Example: Map MMC/SD card to physical drive 1 */
#define SPI_FLASH		1	/* Example: Map USB MSD to physical drive 2 */
//#define DEV_USB		2	/* Example: Map SPI_FLASH to physical drive 3 */

/*-----------------------------------------------------------------------*/
/* ��ȡ�豸״̬                                                          */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status (
	BYTE pdrv		/* ������ */
)
{
	DSTATUS status = STA_NOINIT;
	
	switch (pdrv) {
		case DEV_MMC:				/* SD CARD */
			status &= ~STA_NOINIT;
			break;
		
    case SPI_FLASH:      
      /* SPI Flash״̬��⣺��ȡSPI Flash �豸ID */
      if(sFLASH_ID == SPI_FLASH_ReadID())
      {
        /* �豸ID��ȡ�����ȷ */
        status &= ~STA_NOINIT;
      }
      else
      {
        /* �豸ID��ȡ������� */
        status = STA_NOINIT;;
      }
			break;
		default:
			status = STA_NOINIT;
	}
	return status;
}

/*-----------------------------------------------------------------------*/
/* �豸��ʼ��                                                            */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize (
	BYTE pdrv				/* ������ */
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
      /* ��ʼ��SPI Flash */
			SPI_Flash_Init();
      /* ��ʱһС��ʱ�� */
      i=500;
	    while(--i);	
      /* ��ȡSPI FlashоƬ״̬ */
			SPI_Flash_WAKEUP();
      status=disk_status(SPI_FLASH);
			break;
		default:
			status = STA_NOINIT;
	}
	return status;
}


/*-----------------------------------------------------------------------*/
/* ����������ȡ�������ݵ�ָ���洢��                                              */
/*-----------------------------------------------------------------------*/
DRESULT disk_read (
	BYTE pdrv,		/* �豸������(0..) */
	BYTE *buff,		/* ���ݻ����� */
	DWORD sector,	/* �����׵�ַ */
	UINT count		/* ��������(1..128) */
)
{
	DRESULT status = RES_PARERR;
	
	switch (pdrv) {
		case DEV_MMC:			/* SD CARD */
			SDIO_BlockRead(sector, (uint32_t *)buff); // ���ÿ⺯��������
			status = RES_OK;
		break;
    
		case SPI_FLASH:
      /* ����ƫ��6MB���ⲿFlash�ļ�ϵͳ�ռ����SPI Flash����10MB�ռ� */
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
/* д������������д��ָ�������ռ���                                      */
/*-----------------------------------------------------------------------*/
#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			  /* �豸������(0..) */
	const BYTE *buff,	/* ��д�����ݵĻ����� */
	DWORD sector,		  /* �����׵�ַ */
	UINT count			  /* ��������(1..128) */
)
{
	uint32_t write_addr; 
	DRESULT status = RES_PARERR;
	
	if (!count) {
		return RES_PARERR;		/* Check parameter */
	}

	switch (pdrv) {
		case DEV_MMC:	/* SD CARD */  
			SDIO_BlockWrite(sector, (uint32_t *)buff); // ���ÿ⺯��д����
			status = RES_OK;
		break;
		
		case SPI_FLASH:
      /* ����ƫ��6MB���ⲿFlash�ļ�ϵͳ�ռ����SPI Flash����10MB�ռ� */
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
/* ��������                                                              */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* ������ */
	BYTE cmd,		  /* ����ָ�� */
	void *buff		/* д����߶�ȡ���ݵ�ַָ�� */
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
					*(DWORD * )buff = 31250000;		// ����������ļ�ϵͳ�ĺ�����ȡ�������Ǳ��������������(16000 000 000 / 512)
					break;
				case CTRL_SYNC :
				break;
			}
			status = RES_OK;
			break;
 
		case SPI_FLASH:
			switch (cmd) {
        /* ����������2560*4096/1024/1024=10(MB) */
        case GET_SECTOR_COUNT:
          *(DWORD * )buff = 2560;		
        break;
        /* ������С  */
        case GET_SECTOR_SIZE :
          *(WORD * )buff = 4096;
        break;
        /* ͬʱ������������ */
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

