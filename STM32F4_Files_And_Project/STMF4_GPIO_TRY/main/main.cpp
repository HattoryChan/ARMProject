#include "stm32f4xx.h"

int main()
{
   RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
   
   GPIOA->MODER |= GPIO_MODER_MODER5_0;
   
   GPIOA->OTYPER &= ~GPIO_OTYPER_OT_5;
   
   GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR5_0;
   
   while(1)
   {
      GPIOA->BSRRL = GPIO_BSRR_BS_5;
      
   }
}
