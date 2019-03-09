#include "SWM320.h"


int main(void)
{
	PWM_InitStructure  PWM_initStruct;
	
	SystemInit();
	
	GPIO_Init(GPIOA, PIN8, 1, 0, 0);			//反转PA8，指示PWM新周期开始中断位置
	GPIO_Init(GPIOA, PIN9, 1, 0, 0);			//反转PA9，指示PWM高电平结束中断位置
	
	PWM_initStruct.clk_div = PWM_CLKDIV_8;		//F_PWM = 20M/8 = 2.5M
	
	PWM_initStruct.mode = PWM_MODE_INDEP;		//A路和B路独立输出					
	PWM_initStruct.cycleA = 10000;				//2.5M/10000 = 250Hz			
	PWM_initStruct.hdutyA =  2500;				//2500/10000 = 25%
	PWM_initStruct.initLevelA = 1;
	PWM_initStruct.cycleB = 10000;
	PWM_initStruct.hdutyB =  5000;				//5000/10000 = 50%
	PWM_initStruct.initLevelB = 1;
	PWM_initStruct.HEndAIEn = 1;
	PWM_initStruct.NCycleAIEn = 1;
	PWM_initStruct.HEndBIEn = 0;
	PWM_initStruct.NCycleBIEn = 0;
	
	PWM_Init(PWM0, &PWM_initStruct);
	
	PWM_initStruct.HEndAIEn = 0;
	PWM_initStruct.NCycleAIEn = 0;
	
	PWM_Init(PWM1, &PWM_initStruct);
	
	PORT_Init(PORTA, PIN4, FUNMUX0_PWM0A_OUT, 0);
	PORT_Init(PORTA, PIN6, FUNMUX0_PWM0B_OUT, 0);
	PORT_Init(PORTA, PIN5, FUNMUX1_PWM1A_OUT, 0);
	PORT_Init(PORTA, PIN7, FUNMUX1_PWM1B_OUT, 0);
	
	PWM_Start(PWM0, 1, 1);
	PWM_Start(PWM1, 1, 1);
	
	while(1==1)
	{
	
	}
}

void PWM_Handler(void)
{
	if(PWM_IntNCycleStat(PWM0, PWM_CH_A))
	{
		PWM_IntNCycleClr(PWM0, PWM_CH_A);
		
		GPIO_InvBit(GPIOA, PIN8);
	}
	else if(PWM_IntHEndStat(PWM0, PWM_CH_A))
	{
		PWM_IntHEndClr(PWM0, PWM_CH_A);
		
		GPIO_InvBit(GPIOA, PIN9);
	}
}
