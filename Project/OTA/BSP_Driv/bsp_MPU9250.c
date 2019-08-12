#include "bsp_MPU9250.h"
#include "bsp_IIC.h"
#include "bsp_uart.h"
#include "bsp_systick.h"

/****************************************************************************************************************************************** 
* 函数名称:	MPU_WrByte()
* 功能说明:	IIC写一个字节 
* 输    入: addr器件IIC地址,reg寄存器地址,data数据
* 输    出: 0,正常     其他,失败
* 注意事项: 
******************************************************************************************************************************************/
uint8_t MPU_WrByte(uint8_t addr,uint8_t reg,uint8_t data)
{
    I2C0->MSTDAT = (addr << 1) | 0; 				// 发送器件地址+写命令
	I2C0->MSTCMD = (1 << I2C_MSTCMD_STA_Pos) |		
				   (1 << I2C_MSTCMD_WR_Pos);		// 发送起始位和从机地址
	while(I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk) ;		// 等待发送完成
	if(I2C0->MSTCMD & I2C_MSTCMD_RXACK_Msk)
	{
		printf("Slave send NACK for address\r\n");
		return 1;
	}
	I2C0->MSTDAT = reg;								// 写一个寄存器地址
	I2C0->MSTCMD = (1 << I2C_MSTCMD_WR_Pos);
	while(I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk);
	if(I2C0->MSTCMD & I2C_MSTCMD_RXACK_Msk)
	{
		printf("Slave send NACK for reg\r\n");
		return 1;
	}
	I2C0->MSTDAT = data;							// 写一个数据
	I2C0->MSTCMD = (1 << I2C_MSTCMD_WR_Pos);
	while(I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk);
	if(I2C0->MSTCMD & I2C_MSTCMD_RXACK_Msk)
	{
		printf("Slave send NACK for data\r\n");
		return 1;
	}
	I2C0->MSTCMD = (1 << I2C_MSTCMD_STO_Pos);
	while(I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk);
	if(I2C0->MSTCMD & I2C_MSTCMD_RXACK_Msk)
	{
		printf("Slave send NACK for data\r\n");
		return 1;
	}	
    return 0;
}

//IIC读一个字节 
//reg:寄存器地址 
//返回值:读到的数据
uint8_t MPU_RdByte(uint8_t addr,uint8_t reg)
{
    uint8_t res;
	
    I2C0->MSTDAT = (addr << 1) | 0; 				// 发送器件地址+写命令
	I2C0->MSTCMD = (1 << I2C_MSTCMD_STA_Pos) |		
		           (1 << I2C_MSTCMD_WR_Pos);		// 发送起始位和从机地址
	while(I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk) ;		// 等待发送完成
	if(I2C0->MSTCMD & I2C_MSTCMD_RXACK_Msk)
	{
		printf("Slave send NACK for addr\r\n");
		return 1;
	}
	I2C0->MSTDAT = reg;								// 写一个寄存器地址
	I2C0->MSTCMD = (1 << I2C_MSTCMD_WR_Pos);
	while(I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk);
	if(I2C0->MSTCMD & I2C_MSTCMD_RXACK_Msk)
	{
		printf("Slave send NACK for reg\r\n");
		return 1;
	}
	I2C0->MSTDAT = (addr << 1) | 1; 				// 发送器件地址+读命令
	I2C0->MSTCMD = (1 << I2C_MSTCMD_STA_Pos) |
				   (1 << I2C_MSTCMD_WR_Pos);		//发送Restart
	while(I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk);
	if(I2C0->MSTCMD & I2C_MSTCMD_RXACK_Msk)
	{
		printf("Slave send NACK for address\r\n");
		return 1;
	}
	I2C0->MSTCMD = (1 << I2C_MSTCMD_RD_Pos)  |
				   (1 << I2C_MSTCMD_ACK_Pos) |
				   (1 << I2C_MSTCMD_STO_Pos);		//读取数据、发送NACK响应，完成后发送STOP
	while(I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk) ;		//等待接收完成
	res = I2C0->MSTDAT;	
    return res;  
}

//设置MPU9250的数字低通滤波器
//lpf:数字低通滤波频率(Hz)
//返回值:0,设置成功
//    其他,设置失败 
uint8_t MPU_Set_LPF(uint16_t lpf)
{
	uint8_t data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return MPU_WrByte(MPU9250_ADDR,MPU_CFG_REG,data);//设置数字低通滤波器  
}

//设置MPU9250的采样率(假定Fs=1KHz)
//rate:4~1000(Hz)
//返回值:0,设置成功
//    其他,设置失败 
uint8_t MPU_Set_Rate(uint16_t rate)
{
	uint8_t data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=MPU_WrByte(MPU9250_ADDR,MPU_SAMPLE_RATE_REG,data);	//设置数字低通滤波器
 	return MPU_Set_LPF(rate/2);	//自动设置LPF为采样率的一半
}

uint8_t MPU9250_Init(void)
{
    uint8_t res=0;
    MPU_WrByte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0x80);		//复位MPU9250
    SWM_Delay(100);  										//延时100ms
    MPU_WrByte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0x00);		//唤醒MPU9250
    MPU_WrByte(MPU9250_ADDR,MPU_GYRO_CFG_REG,0x18);			//陀螺仪传感器,±2000dps
	MPU_WrByte(MPU9250_ADDR,MPU_GYRO_CFG_REG,0x00);			//加速度传感器,±2g
    MPU_Set_Rate(50);						       	 		//设置采样率50Hz
    MPU_WrByte(MPU9250_ADDR,MPU_INT_EN_REG,0X00);   		//关闭所有中断
	MPU_WrByte(MPU9250_ADDR,MPU_USER_CTRL_REG,0X00);		//I2C主模式关闭
	MPU_WrByte(MPU9250_ADDR,MPU_FIFO_EN_REG,0X00);			//关闭FIFO
	MPU_WrByte(MPU9250_ADDR,MPU_INTBP_CFG_REG,0X82);		//INT引脚低电平有效，开启bypass模式，可以直接读取磁力计
    res = MPU_RdByte(MPU9250_ADDR,MPU_DEVICE_ID_REG);  		//读取MPU6500的ID
    if(res==MPU6500_ID) //器件ID正确
    {
        MPU_WrByte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X01);  	//设置CLKSEL,PLL X轴为参考
        MPU_WrByte(MPU9250_ADDR,MPU_PWR_MGMT2_REG,0X00);  	//加速度与陀螺仪都工作
		    MPU_Set_Rate(50);						       	//设置采样率为50Hz   
    }
	//else return 1;
 
    res = MPU_RdByte(AK8963_ADDR,MAG_WIA);    				//读取AK8963 ID   
    if(res==AK8963_ID)
    {
        MPU_WrByte(AK8963_ADDR,MAG_CNTL1,0X11);				//设置AK8963为单次测量模式
    }else return 1;

    return 0;
}

