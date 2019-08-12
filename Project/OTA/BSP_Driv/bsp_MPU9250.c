#include "bsp_MPU9250.h"
#include "bsp_IIC.h"
#include "bsp_uart.h"
#include "bsp_systick.h"

/****************************************************************************************************************************************** 
* ��������:	MPU_WrByte()
* ����˵��:	IICдһ���ֽ� 
* ��    ��: addr����IIC��ַ,reg�Ĵ�����ַ,data����
* ��    ��: 0,����     ����,ʧ��
* ע������: 
******************************************************************************************************************************************/
uint8_t MPU_WrByte(uint8_t addr,uint8_t reg,uint8_t data)
{
    I2C0->MSTDAT = (addr << 1) | 0; 				// ����������ַ+д����
	I2C0->MSTCMD = (1 << I2C_MSTCMD_STA_Pos) |		
				   (1 << I2C_MSTCMD_WR_Pos);		// ������ʼλ�ʹӻ���ַ
	while(I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk) ;		// �ȴ��������
	if(I2C0->MSTCMD & I2C_MSTCMD_RXACK_Msk)
	{
		printf("Slave send NACK for address\r\n");
		return 1;
	}
	I2C0->MSTDAT = reg;								// дһ���Ĵ�����ַ
	I2C0->MSTCMD = (1 << I2C_MSTCMD_WR_Pos);
	while(I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk);
	if(I2C0->MSTCMD & I2C_MSTCMD_RXACK_Msk)
	{
		printf("Slave send NACK for reg\r\n");
		return 1;
	}
	I2C0->MSTDAT = data;							// дһ������
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

//IIC��һ���ֽ� 
//reg:�Ĵ�����ַ 
//����ֵ:����������
uint8_t MPU_RdByte(uint8_t addr,uint8_t reg)
{
    uint8_t res;
	
    I2C0->MSTDAT = (addr << 1) | 0; 				// ����������ַ+д����
	I2C0->MSTCMD = (1 << I2C_MSTCMD_STA_Pos) |		
		           (1 << I2C_MSTCMD_WR_Pos);		// ������ʼλ�ʹӻ���ַ
	while(I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk) ;		// �ȴ��������
	if(I2C0->MSTCMD & I2C_MSTCMD_RXACK_Msk)
	{
		printf("Slave send NACK for addr\r\n");
		return 1;
	}
	I2C0->MSTDAT = reg;								// дһ���Ĵ�����ַ
	I2C0->MSTCMD = (1 << I2C_MSTCMD_WR_Pos);
	while(I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk);
	if(I2C0->MSTCMD & I2C_MSTCMD_RXACK_Msk)
	{
		printf("Slave send NACK for reg\r\n");
		return 1;
	}
	I2C0->MSTDAT = (addr << 1) | 1; 				// ����������ַ+������
	I2C0->MSTCMD = (1 << I2C_MSTCMD_STA_Pos) |
				   (1 << I2C_MSTCMD_WR_Pos);		//����Restart
	while(I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk);
	if(I2C0->MSTCMD & I2C_MSTCMD_RXACK_Msk)
	{
		printf("Slave send NACK for address\r\n");
		return 1;
	}
	I2C0->MSTCMD = (1 << I2C_MSTCMD_RD_Pos)  |
				   (1 << I2C_MSTCMD_ACK_Pos) |
				   (1 << I2C_MSTCMD_STO_Pos);		//��ȡ���ݡ�����NACK��Ӧ����ɺ���STOP
	while(I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk) ;		//�ȴ��������
	res = I2C0->MSTDAT;	
    return res;  
}

//����MPU9250�����ֵ�ͨ�˲���
//lpf:���ֵ�ͨ�˲�Ƶ��(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
uint8_t MPU_Set_LPF(uint16_t lpf)
{
	uint8_t data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return MPU_WrByte(MPU9250_ADDR,MPU_CFG_REG,data);//�������ֵ�ͨ�˲���  
}

//����MPU9250�Ĳ�����(�ٶ�Fs=1KHz)
//rate:4~1000(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
uint8_t MPU_Set_Rate(uint16_t rate)
{
	uint8_t data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=MPU_WrByte(MPU9250_ADDR,MPU_SAMPLE_RATE_REG,data);	//�������ֵ�ͨ�˲���
 	return MPU_Set_LPF(rate/2);	//�Զ�����LPFΪ�����ʵ�һ��
}

uint8_t MPU9250_Init(void)
{
    uint8_t res=0;
    MPU_WrByte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0x80);		//��λMPU9250
    SWM_Delay(100);  										//��ʱ100ms
    MPU_WrByte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0x00);		//����MPU9250
    MPU_WrByte(MPU9250_ADDR,MPU_GYRO_CFG_REG,0x18);			//�����Ǵ�����,��2000dps
	MPU_WrByte(MPU9250_ADDR,MPU_GYRO_CFG_REG,0x00);			//���ٶȴ�����,��2g
    MPU_Set_Rate(50);						       	 		//���ò�����50Hz
    MPU_WrByte(MPU9250_ADDR,MPU_INT_EN_REG,0X00);   		//�ر������ж�
	MPU_WrByte(MPU9250_ADDR,MPU_USER_CTRL_REG,0X00);		//I2C��ģʽ�ر�
	MPU_WrByte(MPU9250_ADDR,MPU_FIFO_EN_REG,0X00);			//�ر�FIFO
	MPU_WrByte(MPU9250_ADDR,MPU_INTBP_CFG_REG,0X82);		//INT���ŵ͵�ƽ��Ч������bypassģʽ������ֱ�Ӷ�ȡ������
    res = MPU_RdByte(MPU9250_ADDR,MPU_DEVICE_ID_REG);  		//��ȡMPU6500��ID
    if(res==MPU6500_ID) //����ID��ȷ
    {
        MPU_WrByte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X01);  	//����CLKSEL,PLL X��Ϊ�ο�
        MPU_WrByte(MPU9250_ADDR,MPU_PWR_MGMT2_REG,0X00);  	//���ٶ��������Ƕ�����
		    MPU_Set_Rate(50);						       	//���ò�����Ϊ50Hz   
    }
	//else return 1;
 
    res = MPU_RdByte(AK8963_ADDR,MAG_WIA);    				//��ȡAK8963 ID   
    if(res==AK8963_ID)
    {
        MPU_WrByte(AK8963_ADDR,MAG_CNTL1,0X11);				//����AK8963Ϊ���β���ģʽ
    }else return 1;

    return 0;
}

