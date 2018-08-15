#include "stm32f10x.h"
/*
This programm will be pront text on display MT-16S2H
Leg:
Port A:
 PA0 -> A0
 PA1 -> R/W
 PA2 -> E
 PA3 -> DB0 ~~~~~> PA10 -> DB7
 PA11 -> LED1 (led wiil be show ready display status)
*/
													
uint16_t TimCount = 0;           //  For count TIM interrupts
																//Interrupt function
void TIM2_IRQHandler(void)
{
																//Reset flag(bits)
	TIM2->SR &= ~TIM_SR_UIF;
	TimCount++;                    //plus one for TimCount   	
}											
void InitialALL()
{
															//Clocking for Port A and TIM2
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
															//Initial port A 
															//leg [0-11] - output, 50Mhz,push-pull
	GPIOA->CRL  |= 0x33333333;					//Mode [1;1]
	GPIOA->CRH  |= 0x3333;								//CNF [0;0]
	GPIOA->CRH &= ~0xCCCC;
	GPIOA->CRL &= ~0xCCCCCCCC;					
															//Initial Tim2 
														//Set prescaler timer (24kHz)
	TIM2->PSC = 23999;
														//Allowed interrupts if counter full
	TIM2->DIER |= TIM_DIER_UIE;			
														//Allowed interrupt TIM2 and his piority
	NVIC_EnableIRQ(TIM2_IRQn);
	NVIC_SetPriority(TIM2_IRQn,1);
/*
	if you need use this TIM2, you need sets milliseconds count:
	  TIM2->ARR = 500;
	and use starting accounts function
	  TIM2->CR1 |= TIM_CR1_CEN;
*/	
														
}
															//Function for DELAY(wait C count millisecond)
void Delay(uint16_t c) 					//Delay(ms)
{
	TIM2->ARR = c;						//Sets millisecond delay
	TIM2->CR1 |= TIM_CR1_CEN;				//Start count(accounted)
	while(TimCount == 0);						//Wait
	TimCount = 0;											//RESET TimCount
}

																//Made prepearing action for display
void PrepareForStartingLed()
{
																	//Why need this defined!?
 uint16_t i=0;		
	
	Delay(100);
																//Interface bit set (three for times)	
  GPIOA->BSRR = 0xFFFF0000;	
	for(i = 0; i<=3; i++)
	{
	GPIOA->BSRR = GPIO_BSRR_BS2; 			//Sets E bits [1]
  GPIOA->BSRR = (GPIO_BSRR_BR0 | GPIO_BSRR_BR1 |
								 GPIO_BSRR_BR7 | GPIO_BSRR_BR8 |
								 GPIO_BSRR_BS9 | GPIO_BSRR_BS10); //a0 ->db4 [000011]
	GPIOA->BSRR = GPIO_BSRR_BR2;
	Delay(100);
	}
																//Sets parameter
	GPIOA->BSRR = GPIO_BSRR_BS2; 			//Sets E bits [1]
  GPIOA->BSRR = (GPIO_BSRR_BR0 | GPIO_BSRR_BR1 |
								 GPIO_BSRR_BR3 | GPIO_BSRR_BR4 |
								 GPIO_BSRR_BS5 | GPIO_BSRR_BS6 |
								 GPIO_BSRR_BS7 | GPIO_BSRR_BR8 |
								 GPIO_BSRR_BR9 | GPIO_BSRR_BR10); //a0 ->db4 [0000111000]
	GPIOA->BSRR = GPIO_BSRR_BR2;
	GPIOA->BSRR = 0xFFFF0000;
	Delay(100);
																//Off display
	GPIOA->BSRR = GPIO_BSRR_BS2;
	GPIOA->BSRR = (GPIO_BSRR_BR0 | GPIO_BSRR_BR1 |
								 GPIO_BSRR_BR3 | GPIO_BSRR_BR4 |
								 GPIO_BSRR_BR5 | GPIO_BSRR_BR6 |
								 GPIO_BSRR_BS7 | GPIO_BSRR_BR8 |
								 GPIO_BSRR_BR9 | GPIO_BSRR_BR10); //a0 ->db4 [0000001000]
	GPIOA->BSRR = GPIO_BSRR_BR2;
	GPIOA->BSRR = 0xFFFF0000;
	Delay(100);
																//Clear Display
	GPIOA->BSRR = GPIO_BSRR_BS2;
	GPIOA->BSRR = (GPIO_BSRR_BR0 | GPIO_BSRR_BR1 |
								 GPIO_BSRR_BR3 | GPIO_BSRR_BR4 |
								 GPIO_BSRR_BR5 | GPIO_BSRR_BR6 |
								 GPIO_BSRR_BR7 | GPIO_BSRR_BR8 |
								 GPIO_BSRR_BR9 | GPIO_BSRR_BS10); //a0 ->db4 [00000001]
	GPIOA->BSRR = GPIO_BSRR_BR2;
	GPIOA->BSRR = 0xFFFF0000;
	Delay(100);
																//Set input data mode
	GPIOA->BSRR = GPIO_BSRR_BS2;
	GPIOA->BSRR = (GPIO_BSRR_BR0 | GPIO_BSRR_BR1 |
								 GPIO_BSRR_BR3 | GPIO_BSRR_BR4 |
								 GPIO_BSRR_BR5 | GPIO_BSRR_BR6 |
								 GPIO_BSRR_BR7 | GPIO_BSRR_BS8 |
								 GPIO_BSRR_BS9 | GPIO_BSRR_BR10); //a0 ->db4 [0000001000]
	GPIOA->BSRR = GPIO_BSRR_BR2;
	GPIOA->BSRR = 0xFFFF0000;
	Delay(200);
																	//LED indication "Display ready"
  GPIOA->BSRR = GPIO_BSRR_BS11;

}
													

int main(void)
{
	InitialALL();
	PrepareForStartingLed();
	
																			//Sets "1" symbol
			 GPIOA->BSRR = 0xFFFF0000;
	GPIOA->BSRR = GPIO_BSRR_BS11;
	   GPIOA->BSRR = GPIO_BSRR_BS0;
	   GPIOA->BSRR = (GPIO_BSRR_BS3 | GPIO_BSRR_BR4 |
										GPIO_BSRR_BR5 | GPIO_BSRR_BR6 |
										GPIO_BSRR_BS7 | GPIO_BSRR_BS8 |
										GPIO_BSRR_BR9 | GPIO_BSRR_BR10);
    GPIOA->BSRR = GPIO_BSRR_BR0;	
	
	while(1);
}
