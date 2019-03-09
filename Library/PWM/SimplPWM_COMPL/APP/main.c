#include "SWM320.h"


int main(void)
{
	PWM_InitStructure  PWM_initStruct;
	
	SystemInit();
	
	PWM_initStruct.clk_div = PWM_CLKDIV_8;		//F_PWM = 20M/8 = 2.5M
	
	PWM_initStruct.mode = PWM_MODE_COMPL;		//A路和B路互补输出					
	PWM_initStruct.cycleA = 10000;				//2.5M/10000 = 250Hz			
	PWM_initStruct.hdutyA =  5000;				//5000/10000 = 50%
	PWM_initStruct.deadzoneA = 1000;
	PWM_initStruct.initLevelA = 1;
	PWM_initStruct.deadzoneB = 1000;
	PWM_initStruct.HEndAIEn = 0;
	PWM_initStruct.NCycleAIEn = 0;
	PWM_initStruct.HEndBIEn = 0;
	PWM_initStruct.NCycleBIEn = 0;
	
	PWM_Init(PWM0, &PWM_initStruct);
	
	PORT_Init(PORTA, PIN4, FUNMUX0_PWM0A_OUT, 0);
	PORT_Init(PORTA, PIN6, FUNMUX0_PWM0B_OUT, 0);
	
	PWM_Start(PWM0, 1, 1);
	
	while(1==1)
	{
	
	}
}
