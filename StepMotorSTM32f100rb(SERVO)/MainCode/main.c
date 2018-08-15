/*===========================================================
PA[1] - PWM TIM2 
===========================================================*/
#include "stm32f10x.h"

void delay(uint16_t sec);

int8_t	  ChannelDir[2]={1,-1};
//volatile uint16_t  *DutyCycle[2]={&TIM2->CCR3,&TIM2->CCR4};
volatile uint16_t  *DutyCycle[1]={&TIM2->CCR3};


void SysTick_Handler()
{                                                  
	uint8_t i;
	for(i=0;i<1;i++)						
	{
		*DutyCycle[i]+=ChannelDir[i];										//+1\-1 k znacheniy DutyCycle[i]
		if(*DutyCycle[i]<0x2A2)													//Esli DutyCycle bolshe 0 gradysov[0x50]
			   ChannelDir[i]=1;														// to pribavliaem 1 v pervoi stroke
   	if(*DutyCycle[i]>0x1388)													//Esli DutyCycle ,jkmit 180 gradysov[0x150]
				ChannelDir[i]=-1;														//To pribavliaem -1 v pervoi stroke
	}
}

void InitAll()
{
//--------------------------------------------------     TIM4 for PWM			
	// TIM4 PWM can use PB[8]
																							//Clocking TIM4 and Port B
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	
	GPIOA->CRL  |=GPIO_CRL_MODE2;
	GPIOA->CRL	&=~GPIO_CRL_CNF2_0;
	GPIOA->CRL	|=GPIO_CRL_CNF2_1;
 
																						// PB[9] - Inversion PWM
                                       //PB[8] - Normal PWM																							
  TIM2->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;
	TIM2->CCMR2 &= ~TIM_CCMR2_OC3M_0;
	TIM2->CCER |= TIM_CCER_CC3E;
	TIM2->ARR = 0x0FFF;
																					//Set prescaler value "6", that give us 50Hz
	TIM2->PSC = 0x00A7;
   TIM2->CCR3 = 0xFF8;
																					//Start TIM4
  TIM2->CR1 |= TIM_CR1_CEN;		
   SysTick_Config(SystemCoreClock/512);	
  	
}
int main(void)
{
	InitAll();
while(1);
 
}

//---------------------------------------------         Delay function (sec)
void delay(uint16_t sec)
{
	uint32_t i;
 for (; sec != 0; sec--)
	{
		for(i = 0x5B8D80; i !=0; i--);										// 1 second delay if Fmax = 72 Mhz
	}
}
