2016/3/8
#define ADC_CTRL_TRIG_Pos			14		//ת��������ʽ��0 �������ת��	  1 PWM����   2 TIMR2����    3 TIMR3����
#define ADC_CTRL_TRIG_Msk			(0x03 << ADC_CTRL_TRIG_Pos)
����Ϊ��
#define ADC_CTRL_TRIG_Pos			14		//ת��������ʽ��0 �������ת��	  1 PWM����
#define ADC_CTRL_TRIG_Msk			(0x01 << ADC_CTRL_TRIG_Pos)


void PORT_Init(uint32_t PORTx, uint32_t n, uint32_t func)
����Ϊ��
void PORT_Init(uint32_t PORTx, uint32_t n, uint32_t func, uint32_t digit_in_en)
����������뿪�ع���

2016/3/9
case SYS_CLK_XTAL:			//3 �ⲿ����������2-30MHz��
    SYS->XTALCR = (1 << SYS_XTALCR_EN_Pos);
    
    SYS->CLKSEL |= (1 << SYS_CLKSEL_HFCK_Msk);		//HFCK  <=  XTAL
    SYS->CLKSEL |= (1 << SYS_CLKSEL_SYS_Pos);		//SYS_CLK  <= HFCK
����Ϊ��
case SYS_CLK_XTAL:			//3 �ⲿ����������2-30MHz��
    SYS->XTALCR = (1 << SYS_XTALCR_EN_Pos);
    
    SYS->CLKSEL |= (1 << SYS_CLKSEL_HFCK_Pos);		//HFCK  <=  XTAL
    SYS->CLKSEL |= (1 << SYS_CLKSEL_SYS_Pos);		//SYS_CLK  <= HFCK

2016/3/10
���PLL���á�ADCѡ��VCOʱ����Ϊ����ʱ��

2016/3/14
����д1���жϱ�־����|=�����жϸ���Ϊ��=�����ж�

2016/3/16
PORT_Init�е���PORT_SEL��FUNC_MUX�Ĵ����ĸ�ֵǰ��˳��ʹ�ø�����

����I2C�Ŀ⺯��������

2016/3/28
������CRC_CSR��LCD_CR1_HBP��RTC_DAYHURAL�����Ĵ�����λ�������

2016/4/6
TIMR������TIRM_Init��ĺ������TIRM4��TIMR5������֧

2016/4/12
����ADC->CH[n].STAT�Ĵ�����λ����

2016/4/15
WDT_INTClr()�������WDTx->IF = 0;����ΪWDTx->IF = 1;

����WDT_Init()���壬����WDT�ж�����

2016/4/27
���CAN��������Ͳ�������
���CRC��������Ͳ�������

2016/4/28
���DMA��������
���LCD�������������

2016/5/4
��EXTI_Init()������
if(mode & 0x10)
    GPIOx->INTRISEEN |= (0x01 << n);			//�ߵ�ƽ����
else
    GPIOx->INTRISEEN &= ~(0x01 << n);			//�͵�ƽ����
��
if(mode & 0x10)
    GPIOx->INTRISEEN |= (0x01 << n);			//�����ش���
else
    GPIOx->INTRISEEN &= ~(0x01 << n);			//�½��ش���

����Ϊ��
if(mode & 0x01)
    GPIOx->INTRISEEN |= (0x01 << n);			//�ߵ�ƽ����
else
    GPIOx->INTRISEEN &= ~(0x01 << n);			//�͵�ƽ����
��
if(mode & 0x01)
    GPIOx->INTRISEEN |= (0x01 << n);			//�����ش���
else
    GPIOx->INTRISEEN &= ~(0x01 << n);			//�½��ش���

��ӡ�TIMR\SimplCounter������

2016/5/5
���RTC�������������

2016/5/23
���SDIO��дSD������


2016/6/23
SPIģ�������������������
void SPI_WriteWithWait(SPI_TypeDef * SPIx, uint32_t data)

uint32_t SPI_ReadWrite(SPI_TypeDef * SPIx, uint32_t data)������ӡ�ע�����

2016/8/16
��system_SWM320.c�ļ���void SystemInit(void)������
SYS->XTALCR = (1 << SYS_XTALCR_EN_Pos);�����
for(i = 0; i < 20000; i++);

��system_SWM320.c�ļ���void PLLInit(void)������
SYS->XTALCR = (1 << SYS_XTALCR_EN_Pos);�����
for(i = 0; i < 20000; i++);

2016/8/30
UART��������SWM320_uart.c����Ӷ������º���
uint32_t UART_IsTXBusy(UART_TypeDef * UARTx)

����������fputc(int ch, FILE *f)������ʵ����
while(UART_IsTXFIFOFull(UART0));
UART_WriteByte(UART0, ch);
�ĳ�
UART_WriteByte(UART0, ch);
while(UART_IsTXBusy(UART0));

2016/10/9
��SWM320_gpio.c�ļ���GPIO_Init()������
if(dir == 1)
{
	PORT_Init(PORTA, n, 0, 0);			//PORTA.PINn��������ΪGPIO���ܣ���������ر�
			
	GPIOA->DIR |= (0x01 << n);
}
else
{
	PORT_Init(PORTA, n, 0, 1);			//PORTA.PINn��������ΪGPIO���ܣ��������뿪��
			
	GPIOA->DIR &= ~(0x01 << n);
}
����Ϊ��
PORT_Init(PORTA, n, 0, 1);				//PORTA.PINn��������ΪGPIO���ܣ��������뿪��
if(dir == 1)
{			
	GPIOA->DIR |= (0x01 << n);
}
else
{
	GPIOA->DIR &= ~(0x01 << n);
}
������Ŵ������л�Ϊ������������0�������Ǹ����ⲿ״̬����������

�����GPIOB��GPIOC��GPIOM��GPION��GPIOP����ͬ����

2016/10/10
UART��ADC��I2C��PWM��CAN��SPI��RTC��LCDģ��ĳ�ʼ�����������NVIC_DisableIRQ()���÷�֧

2016/11/16
��UART������
uint8_t UART_ReadByte(UART_TypeDef * UARTx);
uint32_t UART_IsDataValid(UART_TypeDef * UARTx);
���������ϲ���һ����
uint32_t UART_ReadByte(UART_TypeDef * UARTx, uint32_t * data)
��ΪUART���ݺ�Valid��־��ͬһ���Ĵ����У��������������в������UART_DATA�Ĵ��������Σ�����FIFO������

2017/3/3
ɾ���Զ���ͷ�ļ�typedef.h�����ñ�׼��ͷ�ļ�stdint.h�ж����������������

2017/3/8
����system_SWM320.c�ļ���void SystemInit(void)������SYS_CLK_32KHz��֧�����ϵͳʱ���л���32KHz�󲻹���������

2017/3/9
���������ʼ�������е�NVIC_EnableIRQ()���������Ƶ���ʼ��������ĩβ��

���SPI1��ض��塢ɾ��UART4��ض���

UARTģ�����LIN��Ӳ�����ء��Զ������ʵȹ��ܣ�������λλ����У��λ��ֹͣλλ���趨

ADCģ���޸�

2017/4/25
CSL\SWM320_StdPeriph_Driver\SWM320_pwm.c�ļ���PWM_Init()�����ڽ�
PWMG->IF = ((0x01 << bit_offset) | (0x01 << (bit_offset+1))  | (0x01 << (bit_offset+12)) | (0x01 << (bit_offset+13)));
����Ϊ��
PWMG->IRS = ((0x01 << bit_offset) | (0x01 << (bit_offset+1))  | (0x01 << (bit_offset+12)) | (0x01 << (bit_offset+13)));
��ΪPWMG->IF��������жϣ�������PWMG->IRS���

else
{
	NVIC_DisableIRQ(PWMNC_IRQn);
}
����Ϊ��
else if((PWMG->IE & (~((0x01 << bit_offset) | (0x01 << (bit_offset+1))  | (0x01 << (bit_offset+12)) | (0x01 << (bit_offset+13))))) == 0)
{
	NVIC_DisableIRQ(PWM_IRQn);
}
��ֹ��ʼ���ڶ���PWM�����ж�ʱ�ѵ�һ��PWM�����жϸ���ر�

PWM_IntNCycleClr()��PWM_IntHEndClr()������PWMG->IF��ΪPWMG->IRS��������PWMG->IRS��������жϱ�־

2017/5/27
����CAN_SWJ_1tq�Ⱥ궨��ΪCAN_SJW_1tq

2017/7/4
CSL\SWM320_StdPeriph_Driver\SWM320_exti.c�ļ���EXTI_State()�����ڽ�
GPIOx->INTSTAT & (0x01 << n)
����Ϊ��
(GPIOx->INTSTAT >> n) & 0x01

CSL\SWM320_StdPeriph_Driver\SWM320_exti.c�ļ���EXTI_RawState()�����������޸�

2017/9/20
���趨ϵͳʱ�ӵ��Ӻ궨��� system_SWM320.h �ᵽ system_SWM320.c

2017/10/23
CSL\SWM320_StdPeriph_Driver\SWM320_timr.c�ļ���TIMR_INTStat()�����ڽ�
return (TIMRG->IF & TIMRG_IF_TIMR0_Msk);
����Ϊ��
return (TIMRG->IF & TIMRG_IF_TIMR0_Msk) ? 1 : 0;

2017/10/25
CSL\SWM320_StdPeriph_Driver\SWM320_dma.c�ļ��й���IE��IM������������ʹ�ò�ʹ���ж�Ҳ����ͨ����ѯ�ж�״̬ʹ��DMA

2017/12/22
CSL\SWM320_StdPeriph_Driver\SWM320_norflash.c�ļ���ӣ�NOR Flash��������

2018/1/9
CSL\SWM320_StdPeriph_Driver\SWM320_sdram.c�ļ���ӣ�SDRAM��������

2018/1/25
CSL\SWM320_StdPeriph_Driver\SWM320_can.c�ļ���CAN_Init()������AMR��ACR�Ĵ����趨����
CANx->FILTER.AMR[0] = initStruct->FilterMask32b & 0xFF;
CANx->FILTER.AMR[1] = (initStruct->FilterMask32b >>  8) & 0xFF;
... ...
����Ϊ��
CANx->FILTER.AMR[3] = initStruct->FilterMask32b & 0xFF;
CANx->FILTER.AMR[2] = (initStruct->FilterMask32b >>  8) & 0xFF;
... ...

2018/3/23
CSL\CMSIS\DeviceSupport\SWM320.h�ļ��У�
���RTCBKP_ISO��RTCWKEN�����Ĵ�������
�޸�RTC->LOAD�Ĵ������壬�Լ�RTC�������ж�LOAD��ʹ��

CSL\SWM320_StdPeriph_Driver\SWM320_rtc.c�ļ���RTC_AlarmSetup()������������һ��
if(alarmStruct->AlarmIEn)  NVIC_EnableIRQ(RTC_IRQn);

2018/4/11
TIMR0--5��NVIC�е��ж������1���ĳ�6������ÿ��TIMRһ���ж����
ADC����Ӳ��У׼���ܡ�������ͨ��ת�����⡢����ֻ��ʹ��PLLʱ���޷�ʹ��HIRC����
SPI���FIFO�������������ж�
I2C��Ӵӻ�����
PWM��������趨��6λ�ĳ�10λ
SYS_LRCCR_ON ��Ϊ SYS_LRCCR_OFF

2018/4/19
UART->DATA�Ĵ������PAERR��żУ�����ָʾλ
CSL\SWM320_StdPeriph_Driver\SWM320_uart.c�ļ���UART_ReadByte()�޸ģ������ż������

2018/4/25
ADCģ��ɾ��У�����̡�ɾ����������

2018/5/17
ADCģ�����Ӳ��У��
�ж���������ANAC_Handler����ΪBOD_Handler�������һ��BOD�ж�����

2018/5/23
CSL\CMSIS\DeviceSupport\SWM320.h�ļ��У�
����
#define __NVIC_PRIO_BITS          4
����Ϊ��
#define __NVIC_PRIO_BITS          3

2018/5/30
CSL\SWM320_StdPeriph_Driver\SWM320_gpio.c�ļ������ȷ�����š���-��-д������ԭ���Ե�GPIO_Atomic���ͺ���

2018/6/27
CSL\SWM320_StdPeriph_Driver\SWM320_can.h�ļ���struct CAN_RXMessage�Ķ��������format�ֶΣ��������յ��������Ϣ�Ǳ�׼֡������չ֡
CSL\SWM320_StdPeriph_Driver\SWM320_can.c�ļ���CAN_Receive()�����и�CAN_RXMessage->format��ȷ��ֵ
CAN\SimplCAN_RX_Interrupt�����д�ӡ��Ϣʱ��ӡ��CAN_RXMessage->format�ֶ�

2018/7/3
CSL\CMSIS\DeviceSupport\system_SWM320.c�ļ���SystemInit()�����е�ʱ���л����붼��װ��switchCLK_20MHz()���Ƶĺ����У����������ط�����

2018/7/26
CSL\SWM320_StdPeriph_Driver\SWM320_adc.h�ļ���struct ADC_InitStructure�Ķ��������pga_ref�ֶΣ�����ADC�����׼ѡ��

2018/8/17
CSL\SWM320_StdPeriph_Driver\SWM320_uart.c�ļ���UART_SetBaudrate()�����У�����
UARTx->BAUD |= ((SystemCoreClock/16/baudrate) << UART_BAUD_BAUD_Pos);
����Ϊ��
UARTx->BAUD |= ((SystemCoreClock/16/baudrate - 1) << UART_BAUD_BAUD_Pos);

2018/9/14
CSL\CMSIS\DeviceSupport\SWM320.h�ļ�������UART->LINCR�Ĵ�����λ�������

2018/9/25
CSL\CMSIS\DeviceSupport\SWM320.h�ļ���struct SYS_TypeDef������ɾ��HRC20M��HRC40M��BGTRIM��TEMPCR��4��У׼�Ĵ���

2018/10/16
���CSL\SWM320_StdPeriph_Driver\SWM320_sram.c�ļ�

2018/10/29
CSL\SWM320_StdPeriph_Driver\SWM320_can.c�ļ���CAN_SetFilter32b()��CAN_SetFilter16b()�����У�����
CANx->FILTER.AMR[0] = initStruct->FilterMask32b & 0xFF;
CANx->FILTER.AMR[1] = (initStruct->FilterMask32b >>  8) & 0xFF;
... ...
����Ϊ��
CANx->FILTER.AMR[3] = initStruct->FilterMask32b & 0xFF;
CANx->FILTER.AMR[2] = (initStruct->FilterMask32b >>  8) & 0xFF;

2018/11/6
CSL\SWM320_StdPeriph_Driver\SWM320_sram.h�ļ���struct SRAM_InitStructure������ӷ�Ƶ����ClkDiv
CSL\SWM320_StdPeriph_Driver\SWM320_sdram.h�ļ���struct SDRAM_InitStructure�������ʱ�����

2018/12/17
CSL\CMSIS\DeviceSupport\system_SWM320.c�ļ���PLLInit()�����У�����
if(SYS_PLL_SRC == SYS_CLK_20MHz)
{
    SYS->HRCCR = (0 << SYS_HRCCR_OFF_Pos) |
                 (0 << SYS_HRCCR_DBL_Pos);      //HRC = 20MHz
����Ϊ��
if(SYS_PLL_SRC == SYS_CLK_20MHz)
{
    SYS->HRCCR &= ~(0 << SYS_HRCCR_OFF_Pos);    //HRC On
��Ϊ���ϵͳʱ��ѡ��40MHz��ADC_Init()�е���PLLInit()����PLLʱ��ԴΪSYS_CLK_20MHzʱ��֮ǰ�������ϵͳʱ�Ӹ��޸ĳ�20Mhz������ϵͳ����ʱ��������ʱ�Ӳ�һ��

2019/01/11
CSL\SWM320_StdPeriph_Driver\SWM320_flash.c �ļ������Flash_Param_at_120MHz()��������Flash�������ó�120MHz��Ƶ������ʱ����Ĳ������˺���������RAM��ִ��

2019/01/15
CSL\CMSIS\DeviceSupport\startup\iar\startup_SWM320.s ����������

2019/01/19
CSL\CMSIS\DeviceSupport\system_SWM320.c�ļ���PLLInit()�У�����
SYS->HRCCR &= ~(0 << SYS_HRCCR_OFF_Pos);	//HRC On
����Ϊ��
SYS->HRCCR = (0 << SYS_HRCCR_OFF_Pos) |
			 (0 << SYS_HRCCR_DBL_Pos);		//HRC = 20MHz

2019/02/13
CSL\SWM320_StdPeriph_Driver\SWM320_exti.c �ļ��� EXTI_RawState() �У���
return (GPIOx->INTRAWSTAT >> 1) & 0x01;
����Ϊ��
return (GPIOx->INTRAWSTAT >> n) & 0x01;

2019/02/13
CSL\SWM320_StdPeriph_Driver\SWM320_gpio.c �ļ��У���
bits = 0xFFFF >> (16 - w);
����Ϊ��
bits = 0xFFFFFF >> (24 - w);

2019/03/04
CSL\SWM320_StdPeriph_Driver\SWM320_uart.c �ļ��� UART_GetBaudrate() �н���
return (UARTx->BAUD & UART_BAUD_BAUD_Msk);
����Ϊ��
return SystemCoreClock/16/(((UARTx->BAUD & UART_BAUD_BAUD_Msk) >> UART_BAUD_BAUD_Pos) + 1);

2019/07/03
����Delay�⺯�����ɵ���о�ɿ���ϯ����ʦ���澸��ʿ��д��
