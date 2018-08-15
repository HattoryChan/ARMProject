#include "TIM4_Init.h"
// TIM4 PWM can use PB[8,9]


/* Init PWM
*
* Input:
* Output:
*/
void PWM_TIM4_Init()
{
																							//Clocking TIM4 and Port B
  RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;	
																					//Allowed TIM4 use PB[8,9]
  TIM4->CCER |= TIM_CCER_CC3E | TIM_CCER_CC4E;
																						// PB[9] - Inversion PWM
                                       //PB[8] - Normal PWM																							
  TIM4->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC4M;
	TIM4->CCMR2 &= ~TIM_CCMR2_OC3M_0;
																					//Start TIM4
  //TIM4->CR1 |= TIM_CR1_CEN;
}


/* Off/On PWM
*
* Input: int Code: 0 - Off, 1 - On, 2 - status
* Output: int Status: 0 - Off, 1 - On, 2 - ERROR
*/
int PWM_Control(int Code)
{
   int State;
   if (Code != 2)
      State = Code;
   
   switch(Code)
   {
      case 0:
         TIM4->CR1 &= ~TIM_CR1_CEN;
         break;
      
      case 1:
         TIM4->CR1 |= TIM_CR1_CEN;
         break;
      
      case 2:
         return State;
         break;
   }
   return 2;
}

/* Set Sett for PWM
*  Value must be 0 - 65535
* Input: int Numb: 1 - First PWM, 2 - Secon PWM
*        int Value: 1 - 65535 int value
* Output: int Status: 0 - Error, 1 - Ok
*/
int PWM_Setting(int Numb, int Value)
{
   if(Value > 0 || Value < 65535)
      return 0;   
   
   switch (Numb)
   {
      case 1:
         TIM4->CCR3 = Value;
         return 1;
         break;
      
      case 2:
         TIM4->CCR4 = Value;
         return 1;
         break;
   }
}