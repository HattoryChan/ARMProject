#include "stm32f10x.h"

/*
   If button on port A leg 0 pressed,
    this programm will be give High lvl on Port C leg 9,8.
*/		
void InitAll(void)
{
								//Initial output Port C [9,8] leg and input Port A [0] leg.
															//Clocking for port [C,A];
	RCC->APB2ENR |= (RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPAEN);
															//Porc C [9,8] - output
	GPIOC->CRH |= 0x33; //0x33 = 0b00110011  
								//CNF9 [0;0] MODE9 [1;1] CNF8 [0;0] MODE 8 [1;1]
	           //Output f=50Mhz, Push-Pull mode.
	GPIOC->CRH &= ~0xCC; //0xCC = 0b11001100
	/*									//Port A [0] - input Hi-z mode	
	GPIOA->CRL |= 0x04; //0x04 = 0b100
	GPIOA->CRL &= ~0x11; // 0x11 = 0b10001	
   */
   // PA[0] - вход, Push-pull
   GPIOA->CRL &= ~GPIO_CRL_MODE0 ;																															
   GPIOA->CRL &= ~ GPIO_CRL_CNF0;													
   GPIOA->CRL |= GPIO_CRL_CNF0_1;	
}

int main(void)
{
   bool c = false;
	while(1)
	{
								// Initial Ports and Legs
		InitAll();
		
							//Ask leg Port A[0] whether there is a connect to Ground
	//	if((GPIOA->IDR & 0x01) == 0x01) 
      c = GPIOA->IDR & GPIO_IDR_IDR0;
      if(c)
		{
													//If connect give 5v to Porc C[9,8] leg
			GPIOC->BSRR = (GPIO_BSRR_BS9 | GPIO_BSRR_BS8);			
		}  
												//Else connect Port C[9,8] leg to Ground
		else 
		{
			GPIOC->BSRR = (GPIO_BSRR_BR9 | GPIO_BSRR_BR8);
		}
	}
	
}
