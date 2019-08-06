#include "bsp_SPIFlash.h"

static __IO uint32_t  SPITimeout = SPIT_LONG_TIMEOUT;   

void SPI_Flash_Init(void)
{
	SPI_InitStructure SPI_initStruct;
	
	GPIO_Init(GPIOP, PIN23,  1, 0, 0);				// �������CS
	
	PORT_Init(PORTC, PIN1, FUNMUX1_SPI0_SCLK, 0);
	PORT_Init(PORTC, PIN2, FUNMUX0_SPI0_MOSI, 0);
	PORT_Init(PORTC, PIN3, FUNMUX1_SPI0_MISO, 1);
	
	SPI_initStruct.clkDiv = SPI_CLKDIV_256;
	SPI_initStruct.FrameFormat = SPI_FORMAT_SPI;
	SPI_initStruct.SampleEdge = SPI_FIRST_EDGE;
	SPI_initStruct.IdleLevel = SPI_LOW_LEVEL;
	SPI_initStruct.WordSize = 8;
	SPI_initStruct.Master = 1;
	SPI_initStruct.RXHFullIEn = 0;
	SPI_initStruct.TXEmptyIEn = 0;
	SPI_initStruct.TXCompleteIEn = 0;
	SPI_Init(SPI0, &SPI_initStruct);
	SPI_Open(SPI0);
}
 /**
  * @brief  ��ȡFLASH Device ID
  * @param 	��
  * @retval FLASH Device ID
  */
uint32_t SPI_FLASH_ReadDeviceID(void)
{
	uint32_t Temp = 0;
	SPIFlash_CSn_Clr;
	/* Send "RDID " instruction */
	SPI_ReadWrite(SPI0, W25X_DeviceID);
	SPI_ReadWrite(SPI0, Dummy_Byte);
	SPI_ReadWrite(SPI0, Dummy_Byte);
	SPI_ReadWrite(SPI0, Dummy_Byte);
  
	/* Read a byte from the FLASH */
	Temp = SPI_ReadWrite(SPI0, Dummy_Byte);
	SPIFlash_CSn_Set;
	return Temp;
}

 /**
  * @brief  ��ȡFLASH ID
  * @param 	��
  * @retval FLASH ID
  */
uint32_t SPI_FLASH_ReadID(void)
{
	uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
	SPIFlash_CSn_Clr;
	/* ����JEDECָ���ȡID */
	SPI_ReadWrite(SPI0, W25X_JedecDeviceID);
	Temp0 = SPI_ReadWrite(SPI0, Dummy_Byte);/* ��ȡһ���ֽ����� */
	Temp1 = SPI_ReadWrite(SPI0, Dummy_Byte);
	Temp2 = SPI_ReadWrite(SPI0, Dummy_Byte);
	SPIFlash_CSn_Set;
	/*�����������������Ϊ�����ķ���ֵ*/
	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

	return Temp;
}
 /**
  * @brief  ��FLASH���� дʹ�� ����
  * @param  none
  * @retval none
  */
void SPI_FLASH_WriteEnable(void)
{
	SPIFlash_CSn_Clr;						/* ͨѶ��ʼ��CS�� */
	SPI_ReadWrite(SPI0, W25X_WriteEnable); 	/* ����дʹ������*/
	SPIFlash_CSn_Set;						/*ͨѶ������CS�� */
}
 /**
  * @brief  �ȴ�WIP(BUSY)��־����0�����ȴ���FLASH�ڲ�����д�����
  * @param  none
  * @retval none
  */
void SPI_FLASH_WaitForWriteEnd(void)
{
	uint8_t FLASH_Status = 0;

	SPIFlash_CSn_Clr;						/* ͨѶ��ʼ��CS�� */
	SPI_ReadWrite(SPI0, W25X_ReadStatusReg);/* ���� ��״̬�Ĵ��� ���� */
	SPITimeout = SPIT_FLAG_TIMEOUT;
	do/* ��FLASHæµ����ȴ� */
	{
		/* ��ȡFLASHоƬ��״̬�Ĵ��� */
		FLASH_Status = SPI_ReadWrite(SPI0, Dummy_Byte);	 
		if((SPITimeout--) == 0) 
		{
			//SPI_TIMEOUT_UserCallback(4);
			return;
		} 
	}
	while ((FLASH_Status & WIP_Flag) == 1); /* ����д���־ */
	SPIFlash_CSn_Set;						/*ͨѶ������CS�� */
}
 /**
  * @brief  ����FLASH����
  * @param  SectorAddr��Ҫ������������ַ
  * @retval ��
  */
void SPI_FLASH_SectorErase(uint32_t SectorAddr)
{
	SPI_FLASH_WriteEnable();/* ����FLASHдʹ������ */
	SPI_FLASH_WaitForWriteEnd();
	/* �������� */
	SPIFlash_CSn_Clr;						/* ͨѶ��ʼ��CS�� */
	SPI_ReadWrite(SPI0, W25X_SectorErase);	/* ������������ָ��*/
	SPI_ReadWrite(SPI0, (SectorAddr & 0xFF0000) >> 16);/*���Ͳ���������ַ�ĸ�λ*/
	/* ���Ͳ���������ַ����λ */
	SPI_ReadWrite(SPI0, (SectorAddr & 0xFF00) >> 8);
	/* ���Ͳ���������ַ�ĵ�λ */
	SPI_ReadWrite(SPI0, SectorAddr & 0xFF);
	SPIFlash_CSn_Set;						/*ͨѶ������CS�� */
	/* �ȴ��������*/
	SPI_FLASH_WaitForWriteEnd();
}

/**
  * @brief  ��FLASH��ҳд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
  * @param	pBuffer��Ҫд�����ݵ�ָ��
  * @param WriteAddr��д���ַ
  * @param  NumByteToWrite��д�����ݳ��ȣ�����С�ڵ���SPI_FLASH_PerWritePageSize
  * @retval ��
  */
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	/* ����FLASHдʹ������ */
	SPI_FLASH_WriteEnable();

	SPIFlash_CSn_Clr;						/* ͨѶ��ʼ��CS�� */
	/* дҳдָ��*/
	SPI_ReadWrite(SPI0, W25X_PageProgram);
	/*����д��ַ�ĸ�λ*/
	SPI_ReadWrite(SPI0, (WriteAddr & 0xFF0000) >> 16);
	/*����д��ַ����λ*/
	SPI_ReadWrite(SPI0, (WriteAddr & 0xFF00) >> 8);
	/*����д��ַ�ĵ�λ*/
	SPI_ReadWrite(SPI0, WriteAddr & 0xFF);

	if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
	{
		NumByteToWrite = SPI_FLASH_PerWritePageSize;
		printf("SPI_FLASH_PageWrite too large! \r\n");
	}

	/* д������*/
	while (NumByteToWrite--)
	{
		SPI_ReadWrite(SPI0, *pBuffer);/* ���͵�ǰҪд����ֽ����� */
		pBuffer++;/* ָ����һ�ֽ����� */
	}
	SPIFlash_CSn_Set;						/*ͨѶ������CS�� */
	SPI_FLASH_WaitForWriteEnd();/* �ȴ�д�����*/
}

 /**
  * @brief  ��FLASHд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
  * @param	pBuffer��Ҫд�����ݵ�ָ��
  * @param  WriteAddr��д���ַ
  * @param  NumByteToWrite��д�����ݳ���
  * @retval ��
  */
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

	Addr = WriteAddr % SPI_FLASH_PageSize;	/*mod�������࣬��writeAddr��SPI_FLASH_PageSize��������������AddrֵΪ0*/
	count = SPI_FLASH_PageSize - Addr;		/*��count������ֵ���պÿ��Զ��뵽ҳ��ַ*/
	NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;	/*�����Ҫд��������ҳ*/
	NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;	/*mod�������࣬�����ʣ�಻��һҳ���ֽ���*/

	if (Addr == 0) 							/* Addr=0,��WriteAddr �պð�ҳ���� aligned  */
	{
		if (NumOfPage == 0) 				/* NumByteToWrite < SPI_FLASH_PageSize */
		{
			SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
		}
		else /* NumByteToWrite > SPI_FLASH_PageSize */
		{
			while (NumOfPage--)/*�Ȱ�����ҳ��д��*/
			{
			SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
			WriteAddr +=  SPI_FLASH_PageSize;
			pBuffer += SPI_FLASH_PageSize;
		}
			
			/*���ж���Ĳ���һҳ�����ݣ�����д��*/
			SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
		}
	}
	/* ����ַ�� SPI_FLASH_PageSize ������  */
	else 
	{
		/* NumByteToWrite < SPI_FLASH_PageSize */
	if (NumOfPage == 0) 
	{
			/*��ǰҳʣ���count��λ�ñ�NumOfSingleС��д����*/
	  if (NumOfSingle > count) 
	  {
		temp = NumOfSingle - count;
				
				/*��д����ǰҳ*/
		SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
		WriteAddr +=  count;
		pBuffer += count;
				
				/*��дʣ�������*/
		SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
	  }
	  else /*��ǰҳʣ���count��λ����д��NumOfSingle������*/
	  {				
		SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
	  }
	}
	else /* NumByteToWrite > SPI_FLASH_PageSize */
	{
			/*��ַ����������count�ֿ������������������*/
	  NumByteToWrite -= count;
	  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
	  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

	  SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
	  WriteAddr +=  count;
	  pBuffer += count;
			
			/*������ҳ��д��*/
	  while (NumOfPage--)
	  {
		SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
		WriteAddr +=  SPI_FLASH_PageSize;
		pBuffer += SPI_FLASH_PageSize;
	  }
			/*���ж���Ĳ���һҳ�����ݣ�����д��*/
	  if (NumOfSingle != 0)
	  {
		SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
	  }
	}
	}
}

 /**
  * @brief  ��ȡFLASH����
  * @param 	pBuffer���洢�������ݵ�ָ��
  * @param   ReadAddr����ȡ��ַ
  * @param   NumByteToRead����ȡ���ݳ���
  * @retval ��
  */
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
	SPIFlash_CSn_Clr;						/* ͨѶ��ʼ��CS�� */
	SPI_ReadWrite(SPI0, W25X_ReadData);		/* ���� �� ָ�� */

	SPI_ReadWrite(SPI0, (ReadAddr & 0xFF0000) >> 16);	/* ���� �� ��ַ��λ */
	SPI_ReadWrite(SPI0, (ReadAddr& 0xFF00) >> 8);		/* ���� �� ��ַ��λ */
	SPI_ReadWrite(SPI0, ReadAddr & 0xFF);				/* ���� �� ��ַ��λ */
  
	while (NumByteToRead--)					/* ��ȡ���� */
	{
		*pBuffer = SPI_ReadWrite(SPI0, Dummy_Byte);/* ��ȡһ���ֽ�*/
		pBuffer++;							/* ָ����һ���ֽڻ����� */
	}
	SPIFlash_CSn_Set;						/*ͨѶ������CS�� */
}

//�������ģʽ
void SPI_Flash_PowerDown(void)   
{ 
	SPIFlash_CSn_Clr;						/* ͨѶ��ʼ��CS�� */
	SPI_ReadWrite(SPI0, W25X_PowerDown); 	/* ���� ���� ���� */
	SPIFlash_CSn_Set;						/*ͨѶ������CS�� */
}

void SPI_Flash_WAKEUP(void)    //����SPI_FLASH
{
  /*ѡ�� FLASH: CS �� */
  SPIFlash_CSn_Clr;

  /* ���� �ϵ� ���� */
  SPI_ReadWrite(SPI0, W25X_ReleasePowerDown);

  /* ֹͣ�ź� FLASH: CS �� */
  SPIFlash_CSn_Set;                   //�ȴ�TRES1
}   
