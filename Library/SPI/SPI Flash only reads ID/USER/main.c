
#include "SWM320.h"
#include "bsp_systick.h"
#include "bsp_uart.h"
#include "bsp_SPIFlash.h"

int main(void)
{	
	uint32_t DeviceID = 0;
	uint32_t FlashID = 0;
	
	SystemInit();
	SerialInit(115200);							// 初始化串口
	SysTick_Init();								// 初始化滴答定时器
	SPI_Flash_Init();
	
	printf("SPI Flash工程上电OK ...... \r\n");
	
	DeviceID = SPI_FLASH_ReadDeviceID();
	SWM_Delay(200);
	/* 获取 SPI Flash ID */
	FlashID = SPI_FLASH_ReadID();	
	printf("\r\n FlashID is 0x%X,\
	Manufacturer Device ID is 0x%X\r\n", FlashID, DeviceID);
	
	while(1)
	{
	}
}

