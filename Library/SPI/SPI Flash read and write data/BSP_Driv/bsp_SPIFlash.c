#include "bsp_SPIFlash.h"

static __IO uint32_t  SPITimeout = SPIT_LONG_TIMEOUT;   

void SPI_Flash_Init(void)
{
	SPI_InitStructure SPI_initStruct;
	
	GPIO_Init(GPIOP, PIN23,  1, 0, 0);				// 输出，接CS
	
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
  * @brief  读取FLASH Device ID
  * @param 	无
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
  * @brief  读取FLASH ID
  * @param 	无
  * @retval FLASH ID
  */
uint32_t SPI_FLASH_ReadID(void)
{
	uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
	SPIFlash_CSn_Clr;
	/* 发送JEDEC指令，读取ID */
	SPI_ReadWrite(SPI0, W25X_JedecDeviceID);
	Temp0 = SPI_ReadWrite(SPI0, Dummy_Byte);/* 读取一个字节数据 */
	Temp1 = SPI_ReadWrite(SPI0, Dummy_Byte);
	Temp2 = SPI_ReadWrite(SPI0, Dummy_Byte);
	SPIFlash_CSn_Set;
	/*把数据组合起来，作为函数的返回值*/
	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

	return Temp;
}
 /**
  * @brief  向FLASH发送 写使能 命令
  * @param  none
  * @retval none
  */
void SPI_FLASH_WriteEnable(void)
{
	SPIFlash_CSn_Clr;						/* 通讯开始：CS低 */
	SPI_ReadWrite(SPI0, W25X_WriteEnable); 	/* 发送写使能命令*/
	SPIFlash_CSn_Set;						/*通讯结束：CS高 */
}
 /**
  * @brief  等待WIP(BUSY)标志被置0，即等待到FLASH内部数据写入完毕
  * @param  none
  * @retval none
  */
void SPI_FLASH_WaitForWriteEnd(void)
{
	uint8_t FLASH_Status = 0;

	SPIFlash_CSn_Clr;						/* 通讯开始：CS低 */
	SPI_ReadWrite(SPI0, W25X_ReadStatusReg);/* 发送 读状态寄存器 命令 */
	SPITimeout = SPIT_FLAG_TIMEOUT;
	do/* 若FLASH忙碌，则等待 */
	{
		/* 读取FLASH芯片的状态寄存器 */
		FLASH_Status = SPI_ReadWrite(SPI0, Dummy_Byte);	 
		if((SPITimeout--) == 0) 
		{
			//SPI_TIMEOUT_UserCallback(4);
			return;
		} 
	}
	while ((FLASH_Status & WIP_Flag) == 1); /* 正在写入标志 */
	SPIFlash_CSn_Set;						/*通讯结束：CS高 */
}
 /**
  * @brief  擦除FLASH扇区
  * @param  SectorAddr：要擦除的扇区地址
  * @retval 无
  */
void SPI_FLASH_SectorErase(uint32_t SectorAddr)
{
	SPI_FLASH_WriteEnable();/* 发送FLASH写使能命令 */
	SPI_FLASH_WaitForWriteEnd();
	/* 擦除扇区 */
	SPIFlash_CSn_Clr;						/* 通讯开始：CS低 */
	SPI_ReadWrite(SPI0, W25X_SectorErase);	/* 发送扇区擦除指令*/
	SPI_ReadWrite(SPI0, (SectorAddr & 0xFF0000) >> 16);/*发送擦除扇区地址的高位*/
	/* 发送擦除扇区地址的中位 */
	SPI_ReadWrite(SPI0, (SectorAddr & 0xFF00) >> 8);
	/* 发送擦除扇区地址的低位 */
	SPI_ReadWrite(SPI0, SectorAddr & 0xFF);
	SPIFlash_CSn_Set;						/*通讯结束：CS高 */
	/* 等待擦除完毕*/
	SPI_FLASH_WaitForWriteEnd();
}

/**
  * @brief  对FLASH按页写入数据，调用本函数写入数据前需要先擦除扇区
  * @param	pBuffer，要写入数据的指针
  * @param WriteAddr，写入地址
  * @param  NumByteToWrite，写入数据长度，必须小于等于SPI_FLASH_PerWritePageSize
  * @retval 无
  */
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	/* 发送FLASH写使能命令 */
	SPI_FLASH_WriteEnable();

	SPIFlash_CSn_Clr;						/* 通讯开始：CS低 */
	/* 写页写指令*/
	SPI_ReadWrite(SPI0, W25X_PageProgram);
	/*发送写地址的高位*/
	SPI_ReadWrite(SPI0, (WriteAddr & 0xFF0000) >> 16);
	/*发送写地址的中位*/
	SPI_ReadWrite(SPI0, (WriteAddr & 0xFF00) >> 8);
	/*发送写地址的低位*/
	SPI_ReadWrite(SPI0, WriteAddr & 0xFF);

	if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
	{
		NumByteToWrite = SPI_FLASH_PerWritePageSize;
		printf("SPI_FLASH_PageWrite too large! \r\n");
	}

	/* 写入数据*/
	while (NumByteToWrite--)
	{
		SPI_ReadWrite(SPI0, *pBuffer);/* 发送当前要写入的字节数据 */
		pBuffer++;/* 指向下一字节数据 */
	}
	SPIFlash_CSn_Set;						/*通讯结束：CS高 */
	SPI_FLASH_WaitForWriteEnd();/* 等待写入完毕*/
}

 /**
  * @brief  对FLASH写入数据，调用本函数写入数据前需要先擦除扇区
  * @param	pBuffer，要写入数据的指针
  * @param  WriteAddr，写入地址
  * @param  NumByteToWrite，写入数据长度
  * @retval 无
  */
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

	Addr = WriteAddr % SPI_FLASH_PageSize;	/*mod运算求余，若writeAddr是SPI_FLASH_PageSize整数倍，运算结果Addr值为0*/
	count = SPI_FLASH_PageSize - Addr;		/*差count个数据值，刚好可以对齐到页地址*/
	NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;	/*计算出要写多少整数页*/
	NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;	/*mod运算求余，计算出剩余不满一页的字节数*/

	if (Addr == 0) 							/* Addr=0,则WriteAddr 刚好按页对齐 aligned  */
	{
		if (NumOfPage == 0) 				/* NumByteToWrite < SPI_FLASH_PageSize */
		{
			SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
		}
		else /* NumByteToWrite > SPI_FLASH_PageSize */
		{
			while (NumOfPage--)/*先把整数页都写了*/
			{
			SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
			WriteAddr +=  SPI_FLASH_PageSize;
			pBuffer += SPI_FLASH_PageSize;
		}
			
			/*若有多余的不满一页的数据，把它写完*/
			SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
		}
	}
	/* 若地址与 SPI_FLASH_PageSize 不对齐  */
	else 
	{
		/* NumByteToWrite < SPI_FLASH_PageSize */
	if (NumOfPage == 0) 
	{
			/*当前页剩余的count个位置比NumOfSingle小，写不完*/
	  if (NumOfSingle > count) 
	  {
		temp = NumOfSingle - count;
				
				/*先写满当前页*/
		SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
		WriteAddr +=  count;
		pBuffer += count;
				
				/*再写剩余的数据*/
		SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
	  }
	  else /*当前页剩余的count个位置能写完NumOfSingle个数据*/
	  {				
		SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
	  }
	}
	else /* NumByteToWrite > SPI_FLASH_PageSize */
	{
			/*地址不对齐多出的count分开处理，不加入这个运算*/
	  NumByteToWrite -= count;
	  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
	  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

	  SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
	  WriteAddr +=  count;
	  pBuffer += count;
			
			/*把整数页都写了*/
	  while (NumOfPage--)
	  {
		SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
		WriteAddr +=  SPI_FLASH_PageSize;
		pBuffer += SPI_FLASH_PageSize;
	  }
			/*若有多余的不满一页的数据，把它写完*/
	  if (NumOfSingle != 0)
	  {
		SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
	  }
	}
	}
}

 /**
  * @brief  读取FLASH数据
  * @param 	pBuffer，存储读出数据的指针
  * @param   ReadAddr，读取地址
  * @param   NumByteToRead，读取数据长度
  * @retval 无
  */
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
	SPIFlash_CSn_Clr;						/* 通讯开始：CS低 */
	SPI_ReadWrite(SPI0, W25X_ReadData);		/* 发送 读 指令 */

	SPI_ReadWrite(SPI0, (ReadAddr & 0xFF0000) >> 16);	/* 发送 读 地址高位 */
	SPI_ReadWrite(SPI0, (ReadAddr& 0xFF00) >> 8);		/* 发送 读 地址中位 */
	SPI_ReadWrite(SPI0, ReadAddr & 0xFF);				/* 发送 读 地址低位 */
  
	while (NumByteToRead--)					/* 读取数据 */
	{
		*pBuffer = SPI_ReadWrite(SPI0, Dummy_Byte);/* 读取一个字节*/
		pBuffer++;							/* 指向下一个字节缓冲区 */
	}
	SPIFlash_CSn_Set;						/*通讯结束：CS高 */
}

//进入掉电模式
void SPI_Flash_PowerDown(void)   
{ 
	SPIFlash_CSn_Clr;						/* 通讯开始：CS低 */
	SPI_ReadWrite(SPI0, W25X_PowerDown); 	/* 发送 掉电 命令 */
	SPIFlash_CSn_Set;						/*通讯结束：CS高 */
}

void SPI_Flash_WAKEUP(void)    //唤醒SPI_FLASH
{
  /*选择 FLASH: CS 低 */
  SPIFlash_CSn_Clr;

  /* 发上 上电 命令 */
  SPI_ReadWrite(SPI0, W25X_ReleasePowerDown);

  /* 停止信号 FLASH: CS 高 */
  SPIFlash_CSn_Set;                   //等待TRES1
}   
