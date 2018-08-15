#include "GPIO_Init.h"

   
   
 void GPIO_Init()
 {    
 //-------------------- LED Init --------------------------------- 
   RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;  															
																																												//PC[8,9,10] - outputs, Fmax = 2Mhz
	GPIOC->CRH &= ~(GPIO_CRH_CNF8 | GPIO_CRH_CNF9);	
	GPIOC->CRH |= GPIO_CRH_MODE8_0 | GPIO_CRH_MODE9_0;
    
 //-------------------- Uart1 ------------------------------------																		
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;                 //Clocking port A																		
																		
	GPIOA->CRH &= ~GPIO_CRH_CNF9;                      //PA[9] - output,Alternative function,pish-pull
	GPIOA->CRH |= GPIO_CRH_CNF9_1 |GPIO_CRH_MODE9_0 | GPIO_CRH_CNF10_0;   //PA[10] - input,Hi-z 
  
 //-------------------- PWM TIM4 -----------------------------------   
   RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
																					//PB[8,9]- ountput, alternative mode,2Mhz
   GPIOB->CRH |= GPIO_CRH_MODE8_1 | GPIO_CRH_MODE9_1;
	GPIOB->CRH |= GPIO_CRH_CNF8_1 | GPIO_CRH_CNF9_1;    
 }