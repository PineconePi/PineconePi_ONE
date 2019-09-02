#ifndef __SWM320_LCD_H__
#define __SWM320_LCD_H__


typedef struct {
	uint8_t  Interface;		//LCD���ӿڣ�LCD_INTERFACE_RGB��LCD_INTERFACE_I80��LCD_INTERFACE_M68
	
	/* RGBͬ���ӿڲ��� */
	uint8_t  Dir;			//LCD_DIR_LANDSCAPE ����    LCD_DIR_PORTRAIT ����
	uint16_t HnPixel;		//ˮƽ�������ظ��������ȡֵ1024
	uint16_t VnPixel;		//��ֱ�������ظ��������ȡֵ 768
	uint8_t  Hfp;			//horizonal front porch�����ȡֵ32
	uint8_t  Hbp;			//horizonal back porch�� ���ȡֵ128
	uint8_t  Vfp;			//vertical front porch�� ���ȡֵ8
	uint8_t  Vbp;			//vertical back porch��  ���ȡֵ32
	uint8_t  ClkDiv;		//ϵͳʱ�Ӿ�ClkDiv��Ƶ�����DOCCLK��0 2��Ƶ    1 4��Ƶ    2 6��Ƶ    ... ...    31 64��Ƶ
	uint8_t  SamplEdge;		//��Ļ��DOTCLK���ĸ����ز������ݣ�LCD_SAMPLEDGE_RISE��LCD_SAMPLEDGE_FALL
	uint8_t  ClkAlways;		//1 һֱ���DOTCLK    0 ֻ�ڴ�������ʱ���DOTCLK
	uint8_t  HsyncWidth;	//HSYNC�͵�ƽ�������ٸ�DOTCLK��ȡֵ��LCD_HSYNC_1DOTCLK��LCD_HSYNC_2DOTCLK��LCD_HSYNC_3DOTCLK��LCD_HSYNC_4DOTCLK
	
	uint8_t  IntEOTEn;		//End of Transter��������ɣ��ж�ʹ��
} LCD_InitStructure;


#define LCD_INTERFACE_RGB	0
#define LCD_INTERFACE_I80	1
#define LCD_INTERFACE_M68	2

#define LCD_DIR_LANDSCAPE	0	//����
#define LCD_DIR_PORTRAIT	1	//����

#define LCD_SAMPLEDGE_RISE	0	//��Ļ��DOTCLK�������ز�������
#define LCD_SAMPLEDGE_FALL	1	//��Ļ��DOTCLK���½��ز�������

#define LCD_HSYNC_1DOTCLK	0	//1��DOTCLK
#define LCD_HSYNC_2DOTCLK	1
#define LCD_HSYNC_3DOTCLK	2
#define LCD_HSYNC_4DOTCLK	3

#define LCD_CLKDIV_2		0
#define LCD_CLKDIV_4		1
#define LCD_CLKDIV_6		2
#define LCD_CLKDIV_8		3
#define LCD_CLKDIV_10		4
#define LCD_CLKDIV_12		5
#define LCD_CLKDIV_14		6
#define LCD_CLKDIV_16		7
#define LCD_CLKDIV_18		8
#define LCD_CLKDIV_20		9
#define LCD_CLKDIV_22		10
#define LCD_CLKDIV_24		11
#define LCD_CLKDIV_26		12
#define LCD_CLKDIV_28		13
#define LCD_CLKDIV_30		14
#define LCD_CLKDIV_32		15
#define LCD_CLKDIV_34		16
#define LCD_CLKDIV_36		17
#define LCD_CLKDIV_38		18
#define LCD_CLKDIV_40		19
#define LCD_CLKDIV_42		20
#define LCD_CLKDIV_44		21
#define LCD_CLKDIV_46		22
#define LCD_CLKDIV_48		23
#define LCD_CLKDIV_50		24
#define LCD_CLKDIV_52		25
#define LCD_CLKDIV_54		26
#define LCD_CLKDIV_56		27
#define LCD_CLKDIV_58		28
#define LCD_CLKDIV_60		29
#define LCD_CLKDIV_62		30
#define LCD_CLKDIV_64		31


void LCD_Init(LCD_TypeDef * LCDx, LCD_InitStructure * initStruct);
void LCD_Start(LCD_TypeDef * LCDx);
uint32_t LCD_IsBusy(LCD_TypeDef * LCDx);

void LCD_INTEn(LCD_TypeDef * LCDx);
void LCD_INTDis(LCD_TypeDef * LCDx);
void LCD_INTClr(LCD_TypeDef * LCDx);
uint32_t LCD_INTStat(LCD_TypeDef * LCDx);


#endif //__SWM320_LCD_H__
