
#include "bsp_IIC.h"

void I2C_Mst_Init(void)
{
	I2C_InitStructure I2C_initStruct;
	
	PORT_Init(PORTA, PIN4, FUNMUX0_I2C0_SCL, 1);	//GPIOA.4����ΪI2C0 SCL����
	PORT->PORTA_PULLU |= (1 << PIN4);				//����ʹ������������ģ�⿪©
	PORT_Init(PORTA, PIN5, FUNMUX1_I2C0_SDA, 1);	//GPIOA.5����ΪI2C0 SDA����
	PORT->PORTA_PULLU |= (1 << PIN5);				//����ʹ������������ģ�⿪©
	
	I2C_initStruct.Master = 1;
	I2C_initStruct.Addr7b = 1;
	I2C_initStruct.MstClk = 10000;
	I2C_initStruct.MstIEn = 0;
	I2C_Init(I2C0, &I2C_initStruct);
	
	I2C_Open(I2C0);
}

