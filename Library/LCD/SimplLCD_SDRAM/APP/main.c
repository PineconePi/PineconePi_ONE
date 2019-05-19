#include "SWM320.h"

#include "ugui.h"

/* 注意：
	存储器的带宽需要大于LCD刷新所需带宽，否则显示会出现虚影
*/

UG_GUI gui;


#if defined ( __CC_ARM )

uint32_t LCD_Buffer[480*272 * 2 / 4] __attribute__((at(SDRAMM_BASE))) = {0};

#elif defined ( __ICCARM__ )

uint32_t *LCD_Buffer = (uint32_t *)SDRAMM_BASE;

#endif


void _HW_DrawPoint(UG_S16 x, UG_S16 y, UG_COLOR c)
{	
	LCD_Buffer[y*240 + x/2] &= ~(0xFFFF << ((x%2) == 0 ? 0 : 16));
	LCD_Buffer[y*240 + x/2] |=  (c      << ((x%2) == 0 ? 0 : 16));
}


void SerialInit(void);
void MemoryInit(void);
void RGBLCDInit(void);

int main(void)
{
	uint32_t i,j;
	uint16_t color[3] = {C_GREEN, C_BLACK, C_BLUE};
	
	SystemInit();
	
	for(i = 0; i < 1000000; i++) __NOP();	// 等待屏幕完成复位
	
	SerialInit();
	
	MemoryInit();
	
	RGBLCDInit();
	
	LCD->SRCADDR = (uint32_t)LCD_Buffer;
	
	LCD_Start(LCD);
	
	UG_Init(&gui,(void(*)(UG_S16,UG_S16,UG_COLOR))_HW_DrawPoint, 480, 272);
		
 	UG_FillScreen(C_RED);
	
	UG_DrawLine(40, 50, 440, 50, C_GREEN);
	
	UG_DrawLine(80, 36, 80, 236, C_BLUE);
	
	UG_FillFrame(100, 75, 400, 150, C_BLACK);
	
	UG_FontSelect(&FONT_12X20);
	UG_PutString(120, 80, "Hi from Synwit");
	
	UG_DrawCircle(240, 210, 50, C_GREEN);
	UG_DrawCircle(240, 210, 51, C_GREEN);
	UG_DrawCircle(240, 210, 52, C_GREEN);

	while(1==1)
	{		
		for(i = 0; i < 3; i++)
		{
			for(j = 0; j < SystemCoreClock/20; j++) __NOP();
			
			UG_FillFrame(100, 75, 400, 150, color[i]);
		}
	}
}



void RGBLCDInit(void)
{
	LCD_InitStructure LCD_initStruct;
	
	GPIO_Init(GPIOC, PIN2, 1, 0, 0);//背光控制
	GPIO_SetBit(GPIOC, PIN2);		//点亮背光
	
	PORT->PORTN_SEL0 = 0xAAAAAAAA;	//GPION.0～15  LCD_DATA0～15
	PORT->PORTN_SEL1 = 0xAA;
	
	LCD_initStruct.Interface = LCD_INTERFACE_RGB;
	LCD_initStruct.Dir = LCD_DIR_PORTRAIT;
	LCD_initStruct.HnPixel = 480;	//水平480像素
	LCD_initStruct.VnPixel = 272;	//垂直272像素
	LCD_initStruct.Hfp = 5;
	LCD_initStruct.Hbp = 40;
	LCD_initStruct.Vfp = 8;
	LCD_initStruct.Vbp = 8;
	LCD_initStruct.ClkDiv = LCD_CLKDIV_12;	// 注意手册上屏幕的时钟要求
	LCD_initStruct.ClkAlways = 0;
	LCD_initStruct.SamplEdge = LCD_SAMPLEDGE_RISE;
	LCD_initStruct.HsyncWidth = LCD_HSYNC_1DOTCLK;
	LCD_initStruct.IntEOTEn = 1;
	LCD_Init(LCD, &LCD_initStruct);
}

void LCD_Handler(void)
{
	LCD_INTClr(LCD);
	
	LCD_Start(LCD);
}

void MemoryInit(void)
{
	SDRAM_InitStructure SDRAM_InitStruct;
	
	PORT->PORTP_SEL0 = 0xAAAAAAAA;		//PP0-23 => ADDR0-23
	PORT->PORTP_SEL1 = 0x0000AAAA;
	
	PORT->PORTM_SEL0 = 0xAAAAAAAA;		//PM0-15 => DATA15-0
	PORT->PORTM_INEN = 0xFFFF;
	
	PORT->PORTM_SEL1 = 0xAAA;			//PM16 => OEN、PM17 => WEN、PM18 => NORFL_CSN、PM19 => SDRAM_CSN、PM20 => SRAM_CSN、PM21 => SDRAM_CKE
	
	SDRAM_InitStruct.DataWidth = SDRAM_DATAWIDTH_16;
	SDRAM_InitStruct.CellSize = SDRAM_CELLSIZE_256Mb;
	SDRAM_InitStruct.CellBank = SDRAM_CELLBANK_4;
	SDRAM_InitStruct.CellWidth = SDRAM_CELLWIDTH_16;
	SDRAM_InitStruct.TimeTMRD = SDRAM_TMRD_3;
	SDRAM_InitStruct.TimeTRRD = SDRAM_TRRD_2;
	SDRAM_InitStruct.TimeTRAS = SDRAM_TRAS_6;
	SDRAM_InitStruct.TimeTRC  = SDRAM_TRC_8;
	SDRAM_InitStruct.TimeTRCD = SDRAM_TRCD_3;
	SDRAM_InitStruct.TimeTRP  = SDRAM_TRP_3;
	SDRAM_Init(&SDRAM_InitStruct);
}

void SerialInit(void)
{
	UART_InitStructure UART_initStruct;
	
	PORT_Init(PORTA, PIN2, FUNMUX0_UART0_RXD, 1);	//GPIOA.2配置为UART0输入引脚
	PORT_Init(PORTA, PIN3, FUNMUX1_UART0_TXD, 0);	//GPIOA.3配置为UART0输出引脚
 	
 	UART_initStruct.Baudrate = 57600;
	UART_initStruct.DataBits = UART_DATA_8BIT;
	UART_initStruct.Parity = UART_PARITY_NONE;
	UART_initStruct.StopBits = UART_STOP_1BIT;
	UART_initStruct.RXThresholdIEn = 0;
	UART_initStruct.TXThresholdIEn = 0;
	UART_initStruct.TimeoutIEn = 0;
 	UART_Init(UART0, &UART_initStruct);
	UART_Open(UART0);
}

/****************************************************************************************************************************************** 
* 函数名称: fputc()
* 功能说明: printf()使用此函数完成实际的串口打印动作
* 输    入: int ch		要打印的字符
*			FILE *f		文件句柄
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
int fputc(int ch, FILE *f)
{
	UART_WriteByte(UART0, ch);
	
	while(UART_IsTXBusy(UART0));
 	
	return ch;
}
