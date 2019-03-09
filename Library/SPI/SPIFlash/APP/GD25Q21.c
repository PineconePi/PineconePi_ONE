/****************************************************************************************************************************************** 
* 文件名称:	GD25Q21.c
* 功能说明:	GD25Q21 SPI Flash 驱动程序
* 技术支持:	http://www.synwit.com.cn/e/tool/gbook/?bid=1
* 注意事项:
* 版本日期: V1.0.0		2017年10月26日
* 升级记录: 
*******************************************************************************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION 
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, SYNWIT SHALL NOT BE HELD LIABLE 
* FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT 
* OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONN-
* -ECTION WITH THEIR PRODUCTS.
*
* COPYRIGHT 2012 Synwit Technology  
*******************************************************************************************************************************************/
#include "SWM320.h"

#include "GD25Q21.h"


/****************************************************************************************************************************************** 
* 函数名称:	GD25Q21_Init()
* 功能说明:	初始化GD25Q21使用的SPI接口，并读取芯片ID以判断其是否是GD25Q21芯片
* 输    入: 无
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void GD25Q21_Init(void)
{
	uint16_t chip_id;
	uint8_t manufacture_id, device_id;
	SPI_InitStructure SPI_initStruct;
	
	GPIO_Init(GPIOP, PIN23, 1, 0, 0);
#define GD25Q21_Assert()	GPIO_ClrBit(GPIOP, PIN23)
#define GD25Q21_Deassert()	GPIO_SetBit(GPIOP, PIN23)
	GD25Q21_Deassert();
	
	PORT_Init(PORTC, PIN1, FUNMUX1_SPI0_SCLK, 0);
	PORT_Init(PORTC, PIN2, FUNMUX0_SPI0_MOSI, 0);
	PORT_Init(PORTC, PIN3, FUNMUX1_SPI0_MISO, 1);
	
	SPI_initStruct.clkDiv = SPI_CLKDIV_32;
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
	
	chip_id = GD25Q21_ReadChipID();
	manufacture_id = chip_id >> 8;
	device_id = chip_id & 0xFF;
	
	if((manufacture_id != GD25Q21_MANUFACTOR_ID) ||
	   (device_id      != GD25Q21_DEVICE_ID))
	{
		printf("This Chip(Manufactor ID: %02X, Device ID: %02X) is not GD25Q21\r\n", manufacture_id, device_id);
	}
}

/****************************************************************************************************************************************** 
* 函数名称:	GD25Q21_ReadChipID()
* 功能说明:	读取芯片ID
* 输    入: 无
* 输    出: uint16_t			芯片ID，共16位，高8位为厂商ID，低8位为Device ID
* 注意事项: 无
******************************************************************************************************************************************/
uint16_t GD25Q21_ReadChipID(void)
{
	uint8_t manufacture_id, device_id;
	
	GD25Q21_Assert();
	
	SPI_ReadWrite(SPI0, GD25Q21_CMD_READ_CHIP_ID);
	SPI_ReadWrite(SPI0, 0x00);
	SPI_ReadWrite(SPI0, 0x00);
	SPI_ReadWrite(SPI0, 0x00);
	
	manufacture_id = SPI_ReadWrite(SPI0, 0xFF);
	device_id      = SPI_ReadWrite(SPI0, 0xFF);
	
	GD25Q21_Deassert();
	
	return (manufacture_id << 8) | device_id;
}

/****************************************************************************************************************************************** 
* 函数名称:	GD25Q21_ReadStatus()
* 功能说明:	读取芯片状态寄存器
* 输    入: 无
* 输    出: uint16_t			芯片状态
* 注意事项: 无
******************************************************************************************************************************************/
uint16_t GD25Q21_ReadStatus(void)
{
	uint16_t status;
	
	GD25Q21_Assert();
	
	SPI_ReadWrite(SPI0, GD25Q21_CMD_READ_STATUS_L);
	status = SPI_ReadWrite(SPI0, 0xFF);
	
	SPI_ReadWrite(SPI0, GD25Q21_CMD_READ_STATUS_H);
	status |= SPI_ReadWrite(SPI0, 0xFF) << 8;
	
	GD25Q21_Deassert();
	
	return status;
}

/****************************************************************************************************************************************** 
* 函数名称:	GD25Q21_IsBusy()
* 功能说明:	芯片忙检测，执行擦除、写入操作后需要执行忙检测以判断芯片是否已经完成操作
* 输    入: 无
* 输    出: uint32_t			1 芯片正在执行内部擦除、写入操作    0 芯片未在执行内部操作
* 注意事项: 无
******************************************************************************************************************************************/
uint32_t GD25Q21_IsBusy(void)
{
	return (GD25Q21_ReadStatus() & GD25Q21_STATUS_WIP_Msk) ? 1 : 0;
}

/****************************************************************************************************************************************** 
* 函数名称:	GD25Q21_EraseSector()
* 功能说明:	扇区擦除
* 输    入: uint32_t addr		要擦除的扇区的地址，每扇区4K字节
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void GD25Q21_EraseSector(uint32_t addr)
{
	GD25Q21_Assert();
	
	SPI_ReadWrite(SPI0, GD25Q21_CMD_WRITE_ENABLE);
	
	GD25Q21_Deassert();
	
	GD25Q21_Assert();
		
	SPI_ReadWrite(SPI0, GD25Q21_CMD_ERASE_SECTOR);
	SPI_ReadWrite(SPI0, (addr >> 16) & 0xFF);
	SPI_ReadWrite(SPI0, (addr >>  8) & 0xFF);
	SPI_ReadWrite(SPI0, (addr >>  0) & 0xFF);
	
	GD25Q21_Deassert();
}

/****************************************************************************************************************************************** 
* 函数名称:	GD25Q21_ReadData()
* 功能说明:	从GD25Q21指定地址读取指定个字节数据
* 输    入: uint32_t addr		要读取的数据在GD25Q21上的地址
*			uint8_t buff[]		读取到的数据存储在buff中
*			uint32_t cnt		要读取数据的字节个数
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void GD25Q21_ReadData(uint32_t addr, uint8_t buff[], uint32_t cnt)
{
	uint32_t i;
	
	GD25Q21_Assert();
	
	SPI_ReadWrite(SPI0, GD25Q21_CMD_READ_DATA);
	SPI_ReadWrite(SPI0, (addr >> 16) & 0xFF);
	SPI_ReadWrite(SPI0, (addr >>  8) & 0xFF);
	SPI_ReadWrite(SPI0, (addr >>  0) & 0xFF);
	
	for(i = 0; i < cnt; i++)
	{
		buff[i] = SPI_ReadWrite(SPI0, 0xFF);
	}
	
	GD25Q21_Deassert();
}

/****************************************************************************************************************************************** 
* 函数名称:	GD25Q21_WritePage()
* 功能说明:	向GD25Q21指定地址处写入指定个字节数据
* 输    入: uint32_t addr		数据要写入到GD25Q21的位置
*			uint8_t data[]		要写入的数据
*			uint16_t cnt		要写入的数据个数，每页256字节，每次写入的数据必须在同一页的地址范围内
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void GD25Q21_WritePage(uint32_t addr, uint8_t data[], uint16_t cnt)
{
	uint32_t i;
	
	GD25Q21_Assert();
	
	SPI_ReadWrite(SPI0, GD25Q21_CMD_WRITE_ENABLE);
	
	GD25Q21_Deassert();
	
	GD25Q21_Assert();
	
	SPI_ReadWrite(SPI0, GD25Q21_CMD_WRITE_PAGE);
	SPI_ReadWrite(SPI0, (addr >> 16) & 0xFF);
	SPI_ReadWrite(SPI0, (addr >>  8) & 0xFF);
	SPI_ReadWrite(SPI0, (addr >>  0) & 0xFF);
	
	for(i = 0; i < cnt; i++)
	{
		SPI_ReadWrite(SPI0, data[i]);
	}
	
	GD25Q21_Deassert();
}
