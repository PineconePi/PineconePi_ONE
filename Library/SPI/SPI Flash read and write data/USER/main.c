/* ****************************** 作者：残弈悟恩 ***************************************
 * 文件名称 : main.c
 * 文件描述 : 向SPI Flash写入字符串，之后读出，对比其写入和读出的是否一样，继而实现SPI的读写操作
 * 硬件平台 : 飞天二号开发板（处理器：SWM32LQ100FA）  
 * 软件环境 : Keil uVision5.23 + SWM320库函数
 * 
 * 版本代号 : V1.1 	   					
 * 修改日期 : 2018-5-12
 * 版权归属 : 华芯微特和作者共同所有
 * 仅供学习使用，未经作者许可，不得用于其它商业用途，盗版必究。
************************************************************************************** */
#include "SWM320.h"
#include "bsp_systick.h"
#include "bsp_uart.h"
#include "bsp_SPIFlash.h"

/* 获取缓冲区的长度 */
#define TxBufferSize1   (countof(TxBuffer1) - 1)
#define RxBufferSize1   (countof(TxBuffer1) - 1)
#define countof(a)      (sizeof(a) / sizeof(*(a)))
#define  BufferSize (countof(Tx_Buffer)-1)

#define  FLASH_WriteAddress     0x00000
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress

/* 发送缓冲区初始化 */
uint8_t Tx_Buffer[] = "PineconePi ONE!!!!";
uint8_t Rx_Buffer[BufferSize];

int main(void)
{	
	uint32_t DeviceID = 0;
	uint32_t FlashID = 0;
	
	SystemInit();
	SerialInit(115200);							// 初始化串口
	SysTick_Init();								// 初始化滴答定时器
	SPI_Flash_Init();
	
	printf("SPI Flash Test Project RunOK ...... \r\n");
	
	DeviceID = SPI_FLASH_ReadDeviceID();		// 获取 Flash Device ID
	SWM_Delay(200);
	
	FlashID = SPI_FLASH_ReadID();				// 获取 SPI Flash ID
	printf("\r\n FlashID is 0x%X,\
	Manufacturer Device ID is 0x%X\r\n", FlashID, DeviceID);

	/* 检验 SPI Flash ID */
	if (FlashID == sFLASH_ID) 
	{	
		printf("\r\nSPI FLASH is W25Q128 !\r\n");
		
		/* 擦除将要写入的 SPI FLASH 扇区，FLASH写入前要先擦除 */
		SPI_FLASH_SectorErase(FLASH_SectorToErase);	 	 
		
		/* 将发送缓冲区的数据写到flash中 */
		SPI_FLASH_BufferWrite(Tx_Buffer, FLASH_WriteAddress, BufferSize);
		printf("\r\nWrite Date:\r\n%s", Tx_Buffer);
		
		/* 将刚刚写入的数据读出来放到接收缓冲区中 */
		SPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize);
		printf("\r\nRead Date:\r\n%s", Rx_Buffer);

	}
	
	while(1)
	{
	}
}

