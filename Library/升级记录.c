2016/3/8
#define ADC_CTRL_TRIG_Pos			14		//转换触发方式：0 软件启动转换	  1 PWM触发   2 TIMR2触发    3 TIMR3触发
#define ADC_CTRL_TRIG_Msk			(0x03 << ADC_CTRL_TRIG_Pos)
更改为：
#define ADC_CTRL_TRIG_Pos			14		//转换触发方式：0 软件启动转换	  1 PWM触发
#define ADC_CTRL_TRIG_Msk			(0x01 << ADC_CTRL_TRIG_Pos)


void PORT_Init(uint32_t PORTx, uint32_t n, uint32_t func)
更改为：
void PORT_Init(uint32_t PORTx, uint32_t n, uint32_t func, uint32_t digit_in_en)
添加数字输入开关功能

2016/3/9
case SYS_CLK_XTAL:			//3 外部晶体振荡器（2-30MHz）
    SYS->XTALCR = (1 << SYS_XTALCR_EN_Pos);
    
    SYS->CLKSEL |= (1 << SYS_CLKSEL_HFCK_Msk);		//HFCK  <=  XTAL
    SYS->CLKSEL |= (1 << SYS_CLKSEL_SYS_Pos);		//SYS_CLK  <= HFCK
更改为：
case SYS_CLK_XTAL:			//3 外部晶体振荡器（2-30MHz）
    SYS->XTALCR = (1 << SYS_XTALCR_EN_Pos);
    
    SYS->CLKSEL |= (1 << SYS_CLKSEL_HFCK_Pos);		//HFCK  <=  XTAL
    SYS->CLKSEL |= (1 << SYS_CLKSEL_SYS_Pos);		//SYS_CLK  <= HFCK

2016/3/10
添加PLL设置、ADC选择VCO时钟作为采样时钟

2016/3/14
所有写1清中断标志将“|=”清中断更正为“=”清中断

2016/3/16
PORT_Init中调整PORT_SEL和FUNC_MUX寄存器的赋值前后顺序，使得更合理

增加I2C的库函数和例程

2016/3/28
更正了CRC_CSR、LCD_CR1_HBP、RTC_DAYHURAL三个寄存器的位定义错误

2016/4/6
TIMR驱动除TIRM_Init外的函数添加TIRM4、TIMR5条件分支

2016/4/12
更正ADC->CH[n].STAT寄存器的位定义

2016/4/15
WDT_INTClr()中语句由WDTx->IF = 0;更改为WDTx->IF = 1;

更改WDT_Init()定义，增加WDT中断例程

2016/4/27
添加CAN驱动程序和测试例程
添加CRC驱动程序和测试例程

2016/4/28
添加DMA驱动程序
添加LCD驱动程序和例程

2016/5/4
将EXTI_Init()定义中
if(mode & 0x10)
    GPIOx->INTRISEEN |= (0x01 << n);			//高电平触发
else
    GPIOx->INTRISEEN &= ~(0x01 << n);			//低电平触发
和
if(mode & 0x10)
    GPIOx->INTRISEEN |= (0x01 << n);			//上升沿触发
else
    GPIOx->INTRISEEN &= ~(0x01 << n);			//下降沿触发

更正为：
if(mode & 0x01)
    GPIOx->INTRISEEN |= (0x01 << n);			//高电平触发
else
    GPIOx->INTRISEEN &= ~(0x01 << n);			//低电平触发
和
if(mode & 0x01)
    GPIOx->INTRISEEN |= (0x01 << n);			//上升沿触发
else
    GPIOx->INTRISEEN &= ~(0x01 << n);			//下降沿触发

添加“TIMR\SimplCounter”例程

2016/5/5
添加RTC驱动程序和例程

2016/5/23
添加SDIO读写SD卡驱动


2016/6/23
SPI模块添加如下驱动函数：
void SPI_WriteWithWait(SPI_TypeDef * SPIx, uint32_t data)

uint32_t SPI_ReadWrite(SPI_TypeDef * SPIx, uint32_t data)函数添加“注意事项”

2016/8/16
在system_SWM320.c文件的void SystemInit(void)函数中
SYS->XTALCR = (1 << SYS_XTALCR_EN_Pos);后添加
for(i = 0; i < 20000; i++);

在system_SWM320.c文件的void PLLInit(void)函数中
SYS->XTALCR = (1 << SYS_XTALCR_EN_Pos);后添加
for(i = 0; i < 20000; i++);

2016/8/30
UART驱动程序SWM320_uart.c中添加定义如下函数
uint32_t UART_IsTXBusy(UART_TypeDef * UARTx)

将库中所有fputc(int ch, FILE *f)函数的实现由
while(UART_IsTXFIFOFull(UART0));
UART_WriteByte(UART0, ch);
改成
UART_WriteByte(UART0, ch);
while(UART_IsTXBusy(UART0));

2016/10/9
将SWM320_gpio.c文件中GPIO_Init()函数内
if(dir == 1)
{
	PORT_Init(PORTA, n, 0, 0);			//PORTA.PINn引脚配置为GPIO功能，数字输入关闭
			
	GPIOA->DIR |= (0x01 << n);
}
else
{
	PORT_Init(PORTA, n, 0, 1);			//PORTA.PINn引脚配置为GPIO功能，数字输入开启
			
	GPIOA->DIR &= ~(0x01 << n);
}
更改为：
PORT_Init(PORTA, n, 0, 1);				//PORTA.PINn引脚配置为GPIO功能，数字输入开启
if(dir == 1)
{			
	GPIOA->DIR |= (0x01 << n);
}
else
{
	GPIOA->DIR &= ~(0x01 << n);
}
解决引脚从输入切换为输出后输出总是0，而不是根据外部状态决定的问题

后面的GPIOB、GPIOC、GPIOM、GPION、GPIOP做相同处理

2016/10/10
UART、ADC、I2C、PWM、CAN、SPI、RTC、LCD模块的初始化函数中添加NVIC_DisableIRQ()调用分支

2016/11/16
将UART驱动中
uint8_t UART_ReadByte(UART_TypeDef * UARTx);
uint32_t UART_IsDataValid(UART_TypeDef * UARTx);
两个函数合并成一个：
uint32_t UART_ReadByte(UART_TypeDef * UARTx, uint32_t * data)
因为UART数据和Valid标志在同一个寄存器中，放在两个函数中操作会对UART_DATA寄存器读两次，导致FIFO读错误

2017/3/3
删除自定义头文件typedef.h，改用标准库头文件stdint.h中定义的整形数据类型

2017/3/8
修正system_SWM320.c文件的void SystemInit(void)函数中SYS_CLK_32KHz分支，解决系统时钟切换到32KHz后不工作的问题

2017/3/9
将各外设初始化函数中的NVIC_EnableIRQ()函数调用移到初始化函数的末尾处

添加SPI1相关定义、删除UART4相关定义

UART模块添加LIN、硬件流控、自动波特率等功能，及数据位位数、校验位、停止位位数设定

ADC模块修改

2017/4/25
CSL\SWM320_StdPeriph_Driver\SWM320_pwm.c文件中PWM_Init()函数内将
PWMG->IF = ((0x01 << bit_offset) | (0x01 << (bit_offset+1))  | (0x01 << (bit_offset+12)) | (0x01 << (bit_offset+13)));
更改为：
PWMG->IRS = ((0x01 << bit_offset) | (0x01 << (bit_offset+1))  | (0x01 << (bit_offset+12)) | (0x01 << (bit_offset+13)));
因为PWMG->IF清除不掉中断，必须用PWMG->IRS清除

else
{
	NVIC_DisableIRQ(PWMNC_IRQn);
}
更改为：
else if((PWMG->IE & (~((0x01 << bit_offset) | (0x01 << (bit_offset+1))  | (0x01 << (bit_offset+12)) | (0x01 << (bit_offset+13))))) == 0)
{
	NVIC_DisableIRQ(PWM_IRQn);
}
防止初始化第二个PWM不开中断时把第一个PWM开的中断给误关闭

PWM_IntNCycleClr()和PWM_IntHEndClr()函数中PWMG->IF改为PWMG->IRS，必须用PWMG->IRS才能清掉中断标志

2017/5/27
更正CAN_SWJ_1tq等宏定义为CAN_SJW_1tq

2017/7/4
CSL\SWM320_StdPeriph_Driver\SWM320_exti.c文件中EXTI_State()函数内将
GPIOx->INTSTAT & (0x01 << n)
改正为：
(GPIOx->INTSTAT >> n) & 0x01

CSL\SWM320_StdPeriph_Driver\SWM320_exti.c文件中EXTI_RawState()函数做类似修改

2017/9/20
将设定系统时钟的钟宏定义从 system_SWM320.h 搬到 system_SWM320.c

2017/10/23
CSL\SWM320_StdPeriph_Driver\SWM320_timr.c文件中TIMR_INTStat()函数内将
return (TIMRG->IF & TIMRG_IF_TIMR0_Msk);
更正为：
return (TIMRG->IF & TIMRG_IF_TIMR0_Msk) ? 1 : 0;

2017/10/25
CSL\SWM320_StdPeriph_Driver\SWM320_dma.c文件中关于IE和IM的设置修正，使得不使用中断也可以通过查询中断状态使用DMA

2017/12/22
CSL\SWM320_StdPeriph_Driver\SWM320_norflash.c文件添加，NOR Flash驱动程序

2018/1/9
CSL\SWM320_StdPeriph_Driver\SWM320_sdram.c文件添加，SDRAM驱动程序

2018/1/25
CSL\SWM320_StdPeriph_Driver\SWM320_can.c文件中CAN_Init()函数，AMR和ACR寄存器设定修正
CANx->FILTER.AMR[0] = initStruct->FilterMask32b & 0xFF;
CANx->FILTER.AMR[1] = (initStruct->FilterMask32b >>  8) & 0xFF;
... ...
修正为：
CANx->FILTER.AMR[3] = initStruct->FilterMask32b & 0xFF;
CANx->FILTER.AMR[2] = (initStruct->FilterMask32b >>  8) & 0xFF;
... ...

2018/3/23
CSL\CMSIS\DeviceSupport\SWM320.h文件中：
添加RTCBKP_ISO、RTCWKEN两个寄存器定义
修改RTC->LOAD寄存器定义，以及RTC驱动库中对LOAD的使用

CSL\SWM320_StdPeriph_Driver\SWM320_rtc.c文件中RTC_AlarmSetup()函数，最后添加一句
if(alarmStruct->AlarmIEn)  NVIC_EnableIRQ(RTC_IRQn);

2018/4/11
TIMR0--5在NVIC中的中断入口由1个改成6个，即每个TIMR一个中断入口
ADC增加硬件校准功能、修正多通道转换问题、修正只能使用PLL时钟无法使用HIRC问题
SPI添加FIFO清除、传输完成中断
I2C添加从机功能
PWM死区宽度设定由6位改成10位
SYS_LRCCR_ON 改为 SYS_LRCCR_OFF

2018/4/19
UART->DATA寄存器添加PAERR奇偶校验错误指示位
CSL\SWM320_StdPeriph_Driver\SWM320_uart.c文件中UART_ReadByte()修改，添加奇偶错误处理

2018/4/25
ADC模块删除校正例程、删除量程设置

2018/5/17
ADC模块添加硬件校正
中断向量表中ANAC_Handler改正为BOD_Handler，并添加一个BOD中断例程

2018/5/23
CSL\CMSIS\DeviceSupport\SWM320.h文件中：
将：
#define __NVIC_PRIO_BITS          4
改正为：
#define __NVIC_PRIO_BITS          3

2018/5/30
CSL\SWM320_StdPeriph_Driver\SWM320_gpio.c文件中添加确保引脚“读-改-写”操作原子性的GPIO_Atomic类型函数

2018/6/27
CSL\SWM320_StdPeriph_Driver\SWM320_can.h文件中struct CAN_RXMessage的定义中添加format字段，表明接收到的这个消息是标准帧还是扩展帧
CSL\SWM320_StdPeriph_Driver\SWM320_can.c文件中CAN_Receive()函数中给CAN_RXMessage->format正确赋值
CAN\SimplCAN_RX_Interrupt例程中打印信息时打印出CAN_RXMessage->format字段

2018/7/3
CSL\CMSIS\DeviceSupport\system_SWM320.c文件内SystemInit()函数中的时钟切换代码都封装进switchCLK_20MHz()类似的函数中，方便其他地方调用

2018/7/26
CSL\SWM320_StdPeriph_Driver\SWM320_adc.h文件内struct ADC_InitStructure的定义中添加pga_ref字段，用于ADC内外基准选择

2018/8/17
CSL\SWM320_StdPeriph_Driver\SWM320_uart.c文件内UART_SetBaudrate()函数中，将：
UARTx->BAUD |= ((SystemCoreClock/16/baudrate) << UART_BAUD_BAUD_Pos);
改正为：
UARTx->BAUD |= ((SystemCoreClock/16/baudrate - 1) << UART_BAUD_BAUD_Pos);

2018/9/14
CSL\CMSIS\DeviceSupport\SWM320.h文件中修正UART->LINCR寄存器的位定义错误

2018/9/25
CSL\CMSIS\DeviceSupport\SWM320.h文件中struct SYS_TypeDef定义中删除HRC20M、HRC40M、BGTRIM、TEMPCR这4个校准寄存器

2018/10/16
添加CSL\SWM320_StdPeriph_Driver\SWM320_sram.c文件

2018/10/29
CSL\SWM320_StdPeriph_Driver\SWM320_can.c文件中CAN_SetFilter32b()和CAN_SetFilter16b()函数中，将：
CANx->FILTER.AMR[0] = initStruct->FilterMask32b & 0xFF;
CANx->FILTER.AMR[1] = (initStruct->FilterMask32b >>  8) & 0xFF;
... ...
修正为：
CANx->FILTER.AMR[3] = initStruct->FilterMask32b & 0xFF;
CANx->FILTER.AMR[2] = (initStruct->FilterMask32b >>  8) & 0xFF;

2018/11/6
CSL\SWM320_StdPeriph_Driver\SWM320_sram.h文件中struct SRAM_InitStructure定义添加分频参数ClkDiv
CSL\SWM320_StdPeriph_Driver\SWM320_sdram.h文件中struct SDRAM_InitStructure定义添加时序参数

2018/12/17
CSL\CMSIS\DeviceSupport\system_SWM320.c文件中PLLInit()函数中，将：
if(SYS_PLL_SRC == SYS_CLK_20MHz)
{
    SYS->HRCCR = (0 << SYS_HRCCR_OFF_Pos) |
                 (0 << SYS_HRCCR_DBL_Pos);      //HRC = 20MHz
修正为：
if(SYS_PLL_SRC == SYS_CLK_20MHz)
{
    SYS->HRCCR &= ~(0 << SYS_HRCCR_OFF_Pos);    //HRC On
因为如果系统时钟选择40MHz，ADC_Init()中调用PLLInit()，且PLL时钟源为SYS_CLK_20MHz时，之前的语句会把系统时钟给修改成20Mhz，导致系统运行时钟与设置时钟不一致

2019/01/11
CSL\SWM320_StdPeriph_Driver\SWM320_flash.c 文件中添加Flash_Param_at_120MHz()函数，将Flash参数设置成120MHz主频下运行时所需的参数，此函数必须在RAM中执行

2019/01/15
CSL\CMSIS\DeviceSupport\startup\iar\startup_SWM320.s 修正向量表

2019/01/19
CSL\CMSIS\DeviceSupport\system_SWM320.c文件中PLLInit()中，将：
SYS->HRCCR &= ~(0 << SYS_HRCCR_OFF_Pos);	//HRC On
修正为：
SYS->HRCCR = (0 << SYS_HRCCR_OFF_Pos) |
			 (0 << SYS_HRCCR_DBL_Pos);		//HRC = 20MHz

2019/02/13
CSL\SWM320_StdPeriph_Driver\SWM320_exti.c 文件中 EXTI_RawState() 中，将
return (GPIOx->INTRAWSTAT >> 1) & 0x01;
修正为：
return (GPIOx->INTRAWSTAT >> n) & 0x01;

2019/02/13
CSL\SWM320_StdPeriph_Driver\SWM320_gpio.c 文件中，将
bits = 0xFFFF >> (16 - w);
修正为：
bits = 0xFFFFFF >> (24 - w);

2019/03/04
CSL\SWM320_StdPeriph_Driver\SWM320_uart.c 文件中 UART_GetBaudrate() 中将：
return (UARTx->BAUD & UART_BAUD_BAUD_Msk);
修正为：
return SystemCoreClock/16/(((UARTx->BAUD & UART_BAUD_BAUD_Msk) >> UART_BAUD_BAUD_Pos) + 1);

2019/07/03
新增Delay库函数（由电子芯吧客首席评测师徐珂靖博士编写）
