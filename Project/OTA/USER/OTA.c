#include "OTA.h"
#include "bsp_SPIFlash.h"
#include "bsp_sdio.h"

FATFS SD_fs,FLASH_fs;							/* FatFs�ļ�ϵͳ���� */
FIL SD_file,FLASH_file;							/* �ļ����� */
FRESULT res_sd;                		/* �ļ�������� */
FRESULT res_flash;                		/* �ļ�������� */
UINT fnum;            				/* �ļ��ɹ���д���� */
BYTE work[FF_MAX_SS]; 			/* Work area (larger is better for processing time) */

BYTE SD_ReadBuffer[1]={0};        	/* �������� */
BYTE FLASH_WriteBuffer[1]={0};        	/* д������ */
BYTE FLASH_ReadBuffer[1]={0};        	/* �������� */

uint32_t SD_File_Size;
uint32_t FLASH_File_Size;

void SD_FAT_Init()
{
	res_sd = f_mount(&SD_fs,"0:",1);
	
	/* ���û���ļ�ϵͳ�͸�ʽ�����������ļ�ϵͳ */
	if(res_sd == FR_NO_FILESYSTEM)
	{
		printf(">>SD����û���ļ�ϵͳ���������и�ʽ��...\r\n");
		res_sd = f_mkfs("0:",FM_ANY, 0, work, sizeof work);				/* ��ʽ�� */						
		if(res_sd == FR_OK)
		{
			printf(">>SD���ѳɹ���ʽ���ļ�ϵͳ.\r\n");
			res_sd = f_mount(NULL,"0:",1);		/* ��ʽ������ȡ������ */				
			res_sd = f_mount(&SD_fs,"0:",1);		/* ���¹���	*/
		}
		else
		{
			printf(">>SD����ʽ��ʧ��.\r\n");
			while(1);
		}
	}
	else if(res_sd!=FR_OK)
	{
		printf(">>SD�������ļ�ϵͳʧ��.(%d)\r\n",res_sd);
		printf(">>����ԭ��SD����ʼ�����ɹ�.\r\n");
		while(1);
	}
	else
	{
		printf(">>SD��FATFS�ļ�ϵͳ���سɹ�.\r\n");
	}	
}
void FLASH_FAT_Init()
{
res_flash = f_mount(&FLASH_fs,"1:",1);
	/* ���û���ļ�ϵͳ�͸�ʽ�����������ļ�ϵͳ */
	if(res_flash == FR_NO_FILESYSTEM)
	{
		printf(">>SPI FLASH��û���ļ�ϵͳ���������и�ʽ��...\r\n");
		res_flash = f_mkfs("1:",FM_ANY, 0, work, sizeof work);				/* ��ʽ�� */						
		if(res_flash == FR_OK)
		{
			printf(">>SPI FLASH�ѳɹ���ʽ���ļ�ϵͳ.\r\n");
			res_flash = f_mount(NULL,"1:",1);		/* ��ʽ������ȡ������ */				
			res_flash = f_mount(&FLASH_fs,"1:",1);		/* ���¹���	*/
		}
		else
		{
			printf(">>SPI FLASH��ʽ��ʧ��.\r\n");
			while(1);
		}
	}
	else if(res_sd!=FR_OK)
	{
		printf(">>SPI FLASH�����ļ�ϵͳʧ�ܡ�(%d)\r\n",res_flash);
		printf(">>����ԭ��SPIFLASH��ʼ�����ɹ���\r\n");
		while(1);
	}
	else
	{
		printf(">>SPI FLASH FATFS�ļ�ϵͳ���سɹ�.\r\n");
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
				//�ر�boot�����õ����ж�
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
	printf(">>��ʼ��SD���ϵ�����д��SPI FLASH...\r\n");
	res_sd = f_open(&SD_file, SD_path, FA_OPEN_EXISTING | FA_READ);
	res_flash = f_open(&FLASH_file, FLASH_path,FA_CREATE_ALWAYS | FA_WRITE );
	
	SD_File_Size=(uint32_t)f_size(&SD_file);
  printf("�ļ���С��%d�ֽ�\r\n",SD_File_Size);	
	
	
	if((res_sd == FR_OK)&&(res_flash == FR_OK))
	{
		
		for(i=0;i<SD_File_Size;i++)
		{
		f_lseek(&SD_file,i); //����Ǵ򿪵��ļ� �ұ��ǲ�����ʼ��λ��
		f_lseek(&FLASH_file,i); //����Ǵ򿪵��ļ� �ұ��ǲ�����ʼ��λ��
		res_sd = f_read(&SD_file, SD_ReadBuffer, sizeof(SD_ReadBuffer), &fnum); 
		FLASH_WriteBuffer[0]=SD_ReadBuffer[0];
		//printf("0x%X��",FLASH_WriteBuffer[0]);
		res_flash=f_write(&FLASH_file,FLASH_WriteBuffer,sizeof(FLASH_WriteBuffer),&fnum);
		if(i%(uint32_t)(SD_File_Size/100)==0)
		{
			progress++;
			printf("Step1_��ɽ��ȣ�%d \r\n",progress);
		//	printf("0x%X",FLASH_WriteBuffer[0]);
		}
	}
		if(res_sd == FR_OK)
		{
			//printf(">>��ֲ�����,�ܼ�д��%d�ֽ�����.\r\n",i);
		}
		else
		{
			printf(">>SD���ļ���ȡʧ�ܣ�(%d)\n",res_sd);
		}		
	}
	else
	{
		printf(">>SD�����ļ�ʧ�ܡ�\r\n");
	}
	f_close(&SD_file);	/* ���ٶ�д���ر��ļ� */
	f_close(&FLASH_file);
	}
void OTA_SPIFLASHtoFLASH(const TCHAR* path, uint32_t flash_addr)
{
	uint32_t i=0,j=0,k=0;
	uint32_t progress=0;
	uint32_t loop=0;
	uint8_t Buffer[256]={0}; //256���ֽ�
  uint32_t Buffer_Word[64]={0}; //64����
	printf(">>��ʼ��SPI FLASH�ϵ�����д��FLASH...");	
	res_flash = f_open(&FLASH_file,path,FA_OPEN_EXISTING | FA_READ );
	FLASH_File_Size=(uint32_t)f_size(&FLASH_file);
	printf("�ļ���С��%d�ֽ�\r\n",FLASH_File_Size);	
	loop=FLASH_File_Size/256;//256�ֽ�|64��|
	for(i=0;i<(loop+1);i++)
	{
		for(j=0;j<256;j++)
		{
	  f_lseek(&FLASH_file,j+i*256); //����Ǵ򿪵��ļ� �ұ��ǲ�����ʼ��λ��
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
		printf("Step2_OTA��ɽ��ȣ�%d\r\n",progress);
	}
	for(j=0;j<256;j++)
		{
    Buffer[j]=0;
		}
	}
	f_close(&FLASH_file);
	printf("OTA�ɹ�����ʼ����APP\r\n");
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
	f_lseek(&FLASH_file,i); //����Ǵ򿪵��ļ� �ұ��ǲ�����ʼ��λ��
	res_flash = f_read(&FLASH_file, FLASH_ReadBuffer, sizeof(FLASH_ReadBuffer), &fnum); 
	*((volatile uint32_t *) (SDRAMM_BASE + i*4)) = FLASH_ReadBuffer[0];
	if(i%(uint32_t)(FLASH_File_Size/100)==0)
	{
			progress++;
			printf("OTA��ɽ��ȣ�%d\r\n",progress);
		//	printf("0x%X",FLASH_WriteBuffer[0]);
	}
	}
}