#include "stm32f10x.h"

/*
This programm be use TIM2 timer and blinding led Port C [8]
*/
															//Interrupt TIM2 handler
void TIM2_IRQHandler(void)
{
															//Reset interrupt bits(flag)
	TIM2->SR &= ~TIM_SR_UIF; 
															//Reverses output state
  GPIOC->ODR ^= GPIO_ODR_ODR8;
}

void InitialAll()
{
															//Clocking port C and TIM2
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
															//Initial Port C [8] - output, fmax=50Mhz,push-pull
	GPIOC->CRH |= GPIO_CRH_MODE8_0;
	GPIOC->CRH &= ~GPIO_CRH_CNF8;
															//Initial TIM2
													//Preference prescaler timer
		TIM2->PSC = 2;
													//Sets the number of milliseconds
	  TIM2->ARR = 1;
	
/*
	//13.03.2017
	// Tim2 setting: 400Khz
	TIM2->PSC = 9;				
													//Sets the number of milliseconds
	TIM2->ARR = 2;
*/				

// 27.02.2017			Preference prescaler timer  
	TIM2->PSC = 2;   				//400Khz    2.6*10^-6 second
													//Sets the number of milliseconds
	TIM2->ARR = 1;
				
/*	
	//   Tim2 setting: 100KHz, 10us! 
	TIM2->PSC = 2;
													//Sets the number of milliseconds
	TIM2->ARR = 39;
	*/
	
	// Tim2 setting 100Hz, 10ms!
	TIM2->PSC = 1000;
													//Sets the number of milliseconds
	TIM2->ARR = 119;
	
	
	TIM2->PSC = 1000;
													//Sets the number of milliseconds
	TIM2->ARR = 200;
	
	

	//Allowed interrupts if counter full
	TIM2->DIER |= TIM_DIER_UIE;
													//Starts account
	TIM2->CR1 |= TIM_CR1_CEN;
														//Allowed Interrupts TIM2
	NVIC_EnableIRQ(TIM2_IRQn);
														//Interrupts priority
	NVIC_SetPriority(TIM2_IRQn,1);
}

int main()
{
	InitialAll();
	while(1)
   {
   //   GPIOC->ODR ^= GPIO_ODR_ODR8;
   //   GPIOC->BSRR = GPIO_BSRR_BR8;
    //  GPIOC->BSRR = GPIO_BSRR_BS8;
   }
}
