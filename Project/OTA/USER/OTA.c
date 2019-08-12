#include "OTA.h"
#include "bsp_SPIFlash.h"
#include "bsp_sdio.h"

FATFS SD_fs,FLASH_fs;							/* FatFs文件系统对象 */
FIL SD_file,FLASH_file;							/* 文件对象 */
FRESULT res_sd;                		/* 文件操作结果 */
FRESULT res_flash;                		/* 文件操作结果 */
UINT fnum;            				/* 文件成功读写数量 */
BYTE work[FF_MAX_SS]; 			/* Work area (larger is better for processing time) */

BYTE SD_ReadBuffer[1]={0};        	/* 读缓冲区 */
BYTE FLASH_WriteBuffer[1]={0};        	/* 写缓冲区 */
BYTE FLASH_ReadBuffer[1]={0};        	/* 读缓冲区 */

uint32_t SD_File_Size;
uint32_t FLASH_File_Size;

void SD_FAT_Init()
{
	res_sd = f_mount(&SD_fs,"0:",1);
	
	/* 如果没有文件系统就格式化创建创建文件系统 */
	if(res_sd == FR_NO_FILESYSTEM)
	{
		printf(">>SD卡还没有文件系统，即将进行格式化...\r\n");
		res_sd = f_mkfs("0:",FM_ANY, 0, work, sizeof work);				/* 格式化 */						
		if(res_sd == FR_OK)
		{
			printf(">>SD卡已成功格式化文件系统.\r\n");
			res_sd = f_mount(NULL,"0:",1);		/* 格式化后，先取消挂载 */				
			res_sd = f_mount(&SD_fs,"0:",1);		/* 重新挂载	*/
		}
		else
		{
			printf(">>SD卡格式化失败.\r\n");
			while(1);
		}
	}
	else if(res_sd!=FR_OK)
	{
		printf(">>SD卡挂载文件系统失败.(%d)\r\n",res_sd);
		printf(">>可能原因：SD卡初始化不成功.\r\n");
		while(1);
	}
	else
	{
		printf(">>SD卡FATFS文件系统挂载成功.\r\n");
	}	
}
void FLASH_FAT_Init()
{
res_flash = f_mount(&FLASH_fs,"1:",1);
	/* 如果没有文件系统就格式化创建创建文件系统 */
	if(res_flash == FR_NO_FILESYSTEM)
	{
		printf(">>SPI FLASH还没有文件系统，即将进行格式化...\r\n");
		res_flash = f_mkfs("1:",FM_ANY, 0, work, sizeof work);				/* 格式化 */						
		if(res_flash == FR_OK)
		{
			printf(">>SPI FLASH已成功格式化文件系统.\r\n");
			res_flash = f_mount(NULL,"1:",1);		/* 格式化后，先取消挂载 */				
			res_flash = f_mount(&FLASH_fs,"1:",1);		/* 重新挂载	*/
		}
		else
		{
			printf(">>SPI FLASH格式化失败.\r\n");
			while(1);
		}
	}
	else if(res_sd!=FR_OK)
	{
		printf(">>SPI FLASH挂载文件系统失败。(%d)\r\n",res_flash);
		printf(">>可能原因：SPIFLASH初始化不成功。\r\n");
		while(1);
	}
	else
	{
		printf(">>SPI FLASH FATFS文件系统挂载成功.\r\n");
	}	
}
void OTA_Init()
{
	SPI_Flash_Init();
	bsp_SDIOInit();
	SD_FAT_Init();
	FLASH_FAT_Init();
}
void Clear_Interrupt()
{
				//关闭boot程序用到的中断
}
void JumpToApp(uint32_t addr)
{	
	
	uint32_t sp = *((volatile uint32_t *)(addr));
	uint32_t pc = *((volatile uint32_t *)(addr + 4));
	Clear_Interrupt();
	typedef void (*Func_void_void)(void);
	Func_void_void ResetHandler = (Func_void_void)pc;
	
	__disable_irq();
	
	SCB->VTOR = addr;
	
	
	__set_MSP(sp);
	
	ResetHandler();
	
	while(1) __NOP();
}
void OTA_SDtoSPIFLASH(const TCHAR* SD_path,const TCHAR* FLASH_path)
{
	uint32_t i=0;
	uint32_t progress=0;
	printf(">>开始将SD卡上的数据写入SPI FLASH...\r\n");
	res_sd = f_open(&SD_file, SD_path, FA_OPEN_EXISTING | FA_READ);
	res_flash = f_open(&FLASH_file, FLASH_path,FA_CREATE_ALWAYS | FA_WRITE );
	
	SD_File_Size=(uint32_t)f_size(&SD_file);
  printf("文件大小：%d字节\r\n",SD_File_Size);	
	
	
	if((res_sd == FR_OK)&&(res_flash == FR_OK))
	{
		
		for(i=0;i<SD_File_Size;i++)
		{
		f_lseek(&SD_file,i); //左边是打开的文件 右边是操作起始的位置
		f_lseek(&FLASH_file,i); //左边是打开的文件 右边是操作起始的位置
		res_sd = f_read(&SD_file, SD_ReadBuffer, sizeof(SD_ReadBuffer), &fnum); 
		FLASH_WriteBuffer[0]=SD_ReadBuffer[0];
		//printf("0x%X，",FLASH_WriteBuffer[0]);
		res_flash=f_write(&FLASH_file,FLASH_WriteBuffer,sizeof(FLASH_WriteBuffer),&fnum);
		if(i%(uint32_t)(SD_File_Size/100)==0)
		{
			progress++;
			printf("Step1_完成进度：%d \r\n",progress);
		//	printf("0x%X",FLASH_WriteBuffer[0]);
		}
	}
		if(res_sd == FR_OK)
		{
			//printf(">>移植已完成,总计写入%d字节数据.\r\n",i);
		}
		else
		{
			printf(">>SD卡文件读取失败：(%d)\n",res_sd);
		}		
	}
	else
	{
		printf(">>SD卡打开文件失败。\r\n");
	}
	f_close(&SD_file);	/* 不再读写，关闭文件 */
	f_close(&FLASH_file);
	}
void OTA_SPIFLASHtoFLASH(const TCHAR* path, uint32_t flash_addr)
{
	uint32_t i=0,j=0,k=0;
	uint32_t progress=0;
	uint32_t loop=0;
	uint8_t Buffer[256]={0}; //256个字节
  uint32_t Buffer_Word[64]={0}; //64个字
	printf(">>开始将SPI FLASH上的数据写入FLASH...");	
	res_flash = f_open(&FLASH_file,path,FA_OPEN_EXISTING | FA_READ );
	FLASH_File_Size=(uint32_t)f_size(&FLASH_file);
	printf("文件大小：%d字节\r\n",FLASH_File_Size);	
	loop=FLASH_File_Size/256;//256字节|64字|
	for(i=0;i<(loop+1);i++)
	{
		for(j=0;j<256;j++)
		{
	  f_lseek(&FLASH_file,j+i*256); //左边是打开的文件 右边是操作起始的位置
	  res_flash = f_read(&FLASH_file, FLASH_ReadBuffer, sizeof(FLASH_ReadBuffer), &fnum);
    Buffer[j]=FLASH_ReadBuffer[0];
		}
		for(k=0;k<64;k++)
		{
		Buffer_Word[k]=(((uint32_t)Buffer[4*k+3])<<24)|(((uint32_t)Buffer[4*k+2])<<16)|(((uint32_t)Buffer[4*k+1])<<8)|Buffer[4*k];		
		}
		FLASH_Write(flash_addr+256*i,Buffer_Word, 64);	
	if(i%(uint32_t)(loop/10)==0)
	{
			progress++;
		printf("Step2_OTA完成进度：%d\r\n",progress);
	}
	for(j=0;j<256;j++)
		{
    Buffer[j]=0;
		}
	}
	f_close(&FLASH_file);
	printf("OTA成功！开始运行APP\r\n");
  JumpToApp(flash_addr);
}



void OTA_FLASHtoSDRAM(const TCHAR* path, uint32_t sdr_addr)
{
	uint32_t i=0;
	uint32_t progress=0;
	
	res_flash = f_open(&FLASH_file,"1:1.bin",FA_OPEN_EXISTING | FA_READ );
	FLASH_File_Size=(uint32_t)f_size(&FLASH_file);
	
	for(i=0;i<FLASH_File_Size;i++)
	{
	f_lseek(&FLASH_file,i); //左边是打开的文件 右边是操作起始的位置
	res_flash = f_read(&FLASH_file, FLASH_ReadBuffer, sizeof(FLASH_ReadBuffer), &fnum); 
	*((volatile uint32_t *) (SDRAMM_BASE + i*4)) = FLASH_ReadBuffer[0];
	if(i%(uint32_t)(FLASH_File_Size/100)==0)
	{
			progress++;
			printf("OTA完成进度：%d\r\n",progress);
		//	printf("0x%X",FLASH_WriteBuffer[0]);
	}
	}
}