#include "stm32f10x.h"
#define F_CPU 72000000UL 
#define TimerTick F_CPU/1000-1    //Give 1kHz on SysTimer => 1ms

/*
This programm work with interrupts. We made 1KHz interrupts, and start/stop
 lighting with led on Port C[8];
*/

													//SysTimer count value
int timestamp = 0;

											//SysTick Interrupt
void SysTick_Handler(void)
{
	timestamp++;			
}	

											//Initialization function
void InitialAll(void)
{	
											//Clockind for Port C
 RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;	
											//Config Porct C[8] - output, fmax = 50Mhz,Normal push-pull
															//CNF8[0,0];
	GPIOC->CRH &= ~(GPIO_CRH_CNF8); 
															//Mode8[1,1]
	GPIOC->CRH &= ~(GPIO_CRH_MODE8);
	GPIOC->CRH |= GPIO_CRH_MODE8_1;																  					
	
															//Config SysTimer
														//Load TimerTisk(1kHz) value on SysTick
	SysTick->LOAD = TimerTick;  
	SysTick->VAL = TimerTick;
														//Reset Timer and flag
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
									SysTick_CTRL_TICKINT_Msk   |
									SysTick_CTRL_ENABLE_Msk;
}

int main(void)
{
	InitialAll();
	
	while(1)
	{															
																//If timer value == 1000 -> Low voltage on Potr C[8]
																// Blinking on 3 second
		if (timestamp <= 1000) 
			{
																	//Stop Lighting and ...
				GPIOC->BSRR = GPIO_BSRR_BR8;																		
		} else
			{
				              //Start Lighting on Port C[8] leg
	            	GPIOC->BSRR = GPIO_BSRR_BS8;
				if (timestamp >= 2000)
				{
					                 //Reload timer
			     	timestamp = 0;
				}
			}
	}
}
