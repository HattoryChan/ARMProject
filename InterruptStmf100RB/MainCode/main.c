#include "stm32f10x.h"   
/*
This program is interrupts training. 3 led on Port C[6,7,8] and
  2 button on Port A[0,1] leg. Push button and another led start/stop lighting.
	Blinking LEd on Port C[6] leg. 
	
	Interrupts --> { If Port A[1] -> to ground. On Led to Port C[7,8]
	           --> { If Port A[0] -> To ground. Off Led to Port C[7,8]
	
*/

															//ad Delay function
void Delay(uint16_t Val);

															//EXTI1 handler. This on Port A[1] Leg
void EXTI1_IRQHandler(void)	
{
														//Lighting LEd Port C[7] 
	GPIOC->BSRR = GPIO_BSRR_BS7;	
	GPIOC->BSRR = GPIO_BSRR_BS8;
														//Reset interrupt flag
	EXTI->PR |= EXTI_PR_PR1;
}

															//EXTI0 handler. This on Port A[0] Leg
void EXTI0_IRQHandler(void)
{	
														//Stop Lighting LED Port C[7] 
	GPIOC->BSRR = GPIO_BSRR_BR7;
  GPIOC->BSRR = GPIO_BSRR_BR8;	
														//Reset Interrupt Flag
	EXTI->PR |= EXTI_PR_PR0;
}

														//Delay function
void Delay(uint16_t Val)
{
	for( ;Val !=0; Val--)
	{
		__nop();
	}
}

															//Initial Function
void InitialAll(void)
{
												//Clocking for Port [A,C]
	RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN);
												//Port C [6,7,8] - output, fmax=50Mhz,Normal push-pull
	GPIOC->CRL &= ~(GPIO_CRL_CNF6 | GPIO_CRL_CNF7);       //CRL -> [0-7]   
	GPIOC->CRH &= ~(GPIO_CRH_CNF8); 
	
  GPIOC->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_MODE7);
	GPIOC->CRH &= ~(GPIO_CRH_MODE8);
											  //Mode6[1;1] Mode7[1;1]
	GPIOC->CRL |= GPIO_CRL_MODE6_1 | GPIO_CRL_MODE7_1 | GPIO_CRL_MODE6_1 | GPIO_CRL_MODE7_1;
	GPIOC->CRH |= GPIO_CRH_MODE8_1 ;
												//Port A[0,1] - input with res(CNF[1,0], Mode[0,0])
	GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_MODE1);
  
  GPIOA->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_CNF1);
  GPIOA->CRL |= GPIO_CRL_CNF0_1 | GPIO_CRL_CNF1_1;			
												//Set high logical 1 on Port A[0,1]
	GPIOA->BSRR = GPIO_BSRR_BS0 | GPIO_BSRR_BS1;
												
	
												//Enable Global interrupts
	__enable_irq();
												//Setting EXTI1 and EXTI2 how Port A[0,1]
	AFIO->EXTICR[1] |= AFIO_EXTICR1_EXTI0_PA | AFIO_EXTICR1_EXTI1_PA;
												//Go to interrupt when Port A[0,1] leg on LOW voltage
  EXTI->FTSR   |=(EXTI_FTSR_TR1 | EXTI_FTSR_TR0);
												//CMSIS function allowed interrupts in NVIC
  NVIC_EnableIRQ(EXTI1_IRQn);
	NVIC_EnableIRQ(EXTI0_IRQn);
												//Set priority level for EXTI1 and EXTI2
  NVIC_SetPriority(EXTI1_IRQn, 2);
												//Enable interrupts on Peroph
	EXTI->IMR |=(EXTI_IMR_MR1 | EXTI_IMR_MR0);
}

int main(void)
{
	InitialAll();
												//blincing LED Port C[6]
 while(1)
 {
	 GPIOC->BSRR = GPIO_BSRR_BS6;
	 Delay(65535);
	 GPIOC->BSRR = GPIO_BSRR_BR6;
	 Delay(65535);
 }	 
	
}
