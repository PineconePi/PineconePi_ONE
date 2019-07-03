#include "SWM320.h"
#include "swm320_delay.h"
void SerialInit(void);
void helloICKEY(void);	
 
int main(void)
{	
	SystemInit();
	SerialInit();
	printf("Hello World\n");
	GPIO_Init(GPIOA, PIN0, 1, 0, 0);			//Êä³ö£¬½ÓLED
	Delay_Init();
	
	helloICKEY();
	
	while(1==1)
	{
		printf("%d\n",GetTick());
		GPIO_InvBit(GPIOA,PIN0);
		delay(100);
	}
}


void SerialInit(void)
{
	UART_InitStructure UART_initStruct;
	
	PORT_Init(PORTA, PIN2, FUNMUX0_UART0_RXD, 1);	
	PORT_Init(PORTA, PIN3, FUNMUX1_UART0_TXD, 0);	
 	UART_initStruct.Baudrate = 38400;
	UART_initStruct.DataBits = UART_DATA_8BIT;
	UART_initStruct.Parity = UART_PARITY_NONE;
	UART_initStruct.StopBits = UART_STOP_1BIT;
	UART_initStruct.RXThresholdIEn = 0;
	UART_initStruct.TXThresholdIEn = 0;
	UART_initStruct.TimeoutIEn = 0;
 	UART_Init(UART0, &UART_initStruct);
	UART_Open(UART0);
}

int fputc(int ch, FILE *f)
{
	UART_WriteByte(UART0, ch);
	
	while(UART_IsTXBusy(UART0));
 	
	return ch;
}

void helloICKEY(void)
{
	int sum1;
	float sum2;
	int a=12345,b=56789;
	float aa=12345.678,bb=56789.012;
	int n=1000000;
	int i;
	uint32_t time_start;
	uint32_t time;
	
	time_start=GetTick();
	for(i=0;i<n;i++)
	{
		sum1=a+b;
	}
	time=GetTick()-time_start;
	printf("int add test 1m times: %d ms\n",time);
	
	time_start=GetTick();
	for(i=0;i<n;i++)
	{
		sum2=aa+bb;
	}
	time=GetTick()-time_start;
	printf("float add test 1m times: %d ms\n",time);
	
	time_start=GetTick();
	for(i=0;i<n;i++)
	{
		sum1=a*b;
	}
	time=GetTick()-time_start;
	printf("int mul test 1m times: %d ms\n",time);
	
	time_start=GetTick();
	for(i=0;i<n;i++)
	{
		sum2=aa*bb;
	}
	time=GetTick()-time_start;
	printf("float mul test 1m times: %d ms\n",time);
	
	time_start=GetTick();
	for(i=0;i<n;i++)
	{
		sum2=aa/bb;
	}
	time=GetTick()-time_start;
	printf("float div test 1m times: %d ms\n",time);
	
  sum1++;
	sum2++;
}
