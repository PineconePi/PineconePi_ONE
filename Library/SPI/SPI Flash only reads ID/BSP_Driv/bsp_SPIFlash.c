#include "bsp_SPIFlash.h"

void SPI_Flash_Init(void)
{
	SPI_InitStructure SPI_initStruct;
	
	GPIO_Init(GPIOP, PIN23,  1, 0, 0);				// 输出，接CS
	//PORT_Init(PORTC, PIN0, FUNMUX0_SPI0_SSEL, 0);
	PORT_Init(PORTC, PIN1, FUNMUX1_SPI0_SCLK, 0);
	PORT_Init(PORTC, PIN2, FUNMUX0_SPI0_MOSI, 0);
	PORT_Init(PORTC, PIN3, FUNMUX1_SPI0_MISO, 1);
	
	SPI_initStruct.clkDiv = SPI_CLKDIV_32;
	SPI_initStruct.FrameFormat = SPI_FORMAT_SPI;
	SPI_initStruct.SampleEdge = SPI_SECOND_EDGE;
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
	GPIO_ClrBit(GPIOP, PIN23);
	/* Send "RDID " instruction */
	SPI_ReadWrite(SPI0, W25X_DeviceID);
	SPI_ReadWrite(SPI0, Dummy_Byte);
	SPI_ReadWrite(SPI0, Dummy_Byte);
	SPI_ReadWrite(SPI0, Dummy_Byte);
  
	/* Read a byte from the FLASH */
	Temp = SPI_ReadWrite(SPI0, Dummy_Byte);
	GPIO_SetBit(GPIOP, PIN23);
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
	GPIO_ClrBit(GPIOP, PIN23);
	/* 发送JEDEC指令，读取ID */
	SPI_ReadWrite(SPI0, W25X_JedecDeviceID);
	Temp0 = SPI_ReadWrite(SPI0, Dummy_Byte);/* 读取一个字节数据 */
	Temp1 = SPI_ReadWrite(SPI0, Dummy_Byte);
	Temp2 = SPI_ReadWrite(SPI0, Dummy_Byte);
	GPIO_SetBit(GPIOP, PIN23);
	/*把数据组合起来，作为函数的返回值*/
	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

	return Temp;
}

