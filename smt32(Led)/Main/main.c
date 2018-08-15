#include "stm32f10x.h"

void InitAll() {
	  //including timing of the port
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; 
	  //Output on leg LED
	 GPIOC->CRH |= GPIO_CRH_MODE9_0;
	GPIOC->CRH &= ~GPIO_CRH_CNF9;
   
	GPIOC->CRH &= ~GPIO_CRH_CNF8;
	GPIOC->CRH |= GPIO_CRH_CNF8_1;
}

int main(void) {
InitAll();
	while(1)
	{
      if((GPIOC->IDR & GPIO_IDR_IDR8) == 1)   //Button Press?
   {
		GPIOC->BSRR= GPIO_BSRR_BS9;   //On LED
	} 
   else 
   {
		GPIOA->BSRR= GPIO_BSRR_BR9;  //Off LED
	}
}
}
