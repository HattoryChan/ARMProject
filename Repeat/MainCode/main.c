#include "stm32f10x.h"
/*
PA0 - Button (on/off led pc8)
PA1 - Intettrupts EXTI1 (long off/on led PC9)
PA0 - on/off TIM2
PC[8,9,10] - LED
PB[8,9] - PWM on TIM4 
*/

void PWMtoLED(void);
void delay(uint16_t sec);
void delayms(uint16_t ms);
//-------------------------------------------     EXTI1 Handler
void EXTI1_IRQHandler(void)
{	
																											//On|off LED PC9
	GPIOC->ODR ^= GPIO_ODR_ODR9;											
																											//On|Off TIM3 count	
	TIM3->CR1 ^= TIM_CR1_CEN;														
																										//Clear Interrupts flag
	EXTI->PR |= EXTI_PR_PR1;																							
}
//--------------------------------------------	  TIM2 Handler    
void TIM2_IRQHandler(void)        
{  
																							//Clear Flag
	TIM2->SR &= ~TIM_SR_UIF;
	
	GPIOC->ODR ^= GPIO_ODR_ODR9;									//On|off LED PC9
}					
//---------------------------------------------   TIM3 Handler
void TIM3_IRQHandler(void)
{
																								//Clear Flag
	TIM3->SR &= ~TIM_SR_UIF;											
																								//Off|ON led PC10	
	GPIOC->ODR ^= GPIO_ODR_ODR10;										
}
//--------------------------------------------------------------      INITIAL FUNCTION
void InitAll(void)
{	
	//--------------------------------------------					GPIO
																																									//clocking Port [A,C]
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPAEN ;  															
																																												//PC[8,9,10] - outputs, Fmax = 2Mhz
	GPIOC->CRH &= ~(GPIO_CRH_CNF8 | GPIO_CRH_CNF9 | GPIO_CRH_CNF10);	
	GPIOC->CRH |= GPIO_CRH_MODE8_0 | GPIO_CRH_MODE9_0 | GPIO_CRH_MODE10_0;
																																										//PA[1,0] - input, Push-pull for PA1, Hi-z for PA0
	GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_MODE1);
																															//Cleat bits (default value cnf[0,1])
	GPIOA->CRL &= ~(GPIO_CRL_CNF1 | GPIO_CRL_CNF0);													
	GPIOA->CRL |= GPIO_CRL_CNF0_0 | GPIO_CRL_CNF1_1;	
																																						//On Pull up to PA1 leg
	GPIOA->BSRR = GPIO_BSRR_BS1;
	//--------------------------------------------					EXTI1
	//---------------------------------------------         Allow global interrupts
	__enable_irq();
																						//Clocking AFIO(EXTI1)
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
																						//Setting EXTI1 for PA[1];
	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI1_PA;
																					//Call it if get HIGH signal on PA1 leg
	EXTI->RTSR |= EXTI_RTSR_TR1;
	//--------------------------------------------------        Enable this in CMSIS
	NVIC_EnableIRQ(EXTI1_IRQn);												//EXTI1
	NVIC_EnableIRQ(TIM2_IRQn);												//TIM2
	NVIC_EnableIRQ(TIM3_IRQn);												//TIM3
	//--------------------------------------------------        NVIC Priority Interrupts and Timer
  NVIC_SetPriority(TIM2_IRQn, 1);										//TIM2											
	NVIC_SetPriority(EXTI1_IRQn,2);										//EXTI1
	NVIC_SetPriority(TIM3_IRQn, 3);										//TIM3
																						//Enable interrupts on periphery
	EXTI->IMR |= EXTI_IMR_MR1;	
 //-------------------------------------------------			Timer TIM2
	/*original value 23999 get 1 sec if value 1000 */    
																								//Clocking TIM2
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;																								
																							//Preference prescaler TIM2
  TIM2->PSC = 23999;                					//1000 = 1sec
																							//Allowed TIM2Handler
  TIM2->DIER |= TIM_DIER_UIE;																							
	
  TIM2->ARR = 5000;       									//5 sec 
	//--------------------------------------------------    Timer TIM3
	/*original value 2399 get 1 ms if value 1000 */
																							//Clocking TIM3
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;																							
																							//Preference prescaler TIM3
	TIM3->PSC = 2399;
																							//Allow handler
	TIM3->DIER |= TIM_DIER_UIE;
																							//Delay times
  TIM3->ARR = 10000;
	//--------------------------------------------------- PMW Timer TIM4
	// TIM4 PWM can use PB[8,9]
																							//Clocking TIM4 and Port B
  RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
																					//PB[8,9]- ountput, alternative mode,2Mhz
  GPIOB->CRH |= GPIO_CRH_MODE8_1 | GPIO_CRH_MODE9_1;
	GPIOB->CRH |= GPIO_CRH_CNF8_1 | GPIO_CRH_CNF9_1;
																					//Allowed TIM4 use PB[8,9]
  TIM4->CCER |= TIM_CCER_CC3E | TIM_CCER_CC4E;
																						// PB[9] - Inversion PWM
                                       //PB[8] - Normal PWM																							
  TIM4->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC4M;
	TIM4->CCMR2 &= ~TIM_CCMR2_OC3M_0;
																					//Start TIM4
  TIM4->CR1 |= TIM_CR1_CEN;
																					//PB[8] - on Ground
  TIM4->CCR3 = 65535;																						
	//---------  Write brightness to TIM4->CCRx [max 65535]
	return;	
}	
int main(void)
{
	InitAll();	
	while(1)
	{		
      if((GPIOA->IDR & 0x01)== 0x01)
		{																											
		GPIOC->BSRR = GPIO_BSRR_BS8;												//On Led PC8
		delay(1);			
																											//On|Off TIM2 count	
		TIM2->CR1 ^= TIM_CR1_CEN;   
		PWMtoLED();
           		
		} else 
		{
		GPIOC->BSRR = GPIO_BSRR_BR8;										//Off Led PC8    
	}
 }
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
//-----------------------------------------------    Delay Function (ms)
void delayms(uint16_t ms)
{
	uint32_t i;
 for(; ms !=0; ms--)
	{
		for(i = 0x2710; i != 0; i--);											// 1ms delay if Fmax = 72mHz
	}
}
//-----------------------------------------------   Brightness function
// PWMtoLED - smooth brightness LED[8,9]
void PWMtoLED()
{
																										// Brightness Array
	uint32_t pwm_arr[]={0,0,6553,13107,19660,26214,32768,39321,
		                      45875,52875,58982,65535};
	uint8_t i;  
 for (i=11;i>=1;i--) {
		TIM4->CCR3=pwm_arr[i];
		 delayms(80); 
  }	  
 for (i=1;i<=11;i++) {
		TIM4->CCR3=pwm_arr[i];
		 delayms(80);
  }
	for (i=1;i<=10;i++) {
		TIM4->CCR4=pwm_arr[i];
		 delayms(80);
  }
	for (i=11;i>=1;i--) {
		TIM4->CCR4=pwm_arr[i];
		 delayms(80);
  }		
}
