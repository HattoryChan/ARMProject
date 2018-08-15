/*==========================================================
This library will be Send and Get byte message from TX/RX, you need create Usart1_IRQHandler
PA[9]- TX
PA[10]- RX
PA[8] - DHT11 LEG
==========================================================*/

#include "stm32f10x.h"
#include "Periphery_Init.h"

/*
// Handler for Usart
void USART1_IRQHandler(void)
{
			
}
*/

//============------------====== Initiall GpioA, Usart
//   * in_u32BaudRate   - value for calculate BoudRate
void InitPeriphery(uint32_t in_u32BaudRate)
{
   
/*-----------------------------------------
   Usart Initialization   
-----------------------------------------*/
   /*
   Port A:
   [9,10] - tx,rx for Usart1
   */
 
//------------------------------------------  GpioA
																		//Clocking port A
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
   RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
																		//PA[9] - output,Alternative function,pish-pull
																		//PA[10] - input,Hi-z
	GPIOA->CRH &= ~GPIO_CRH_CNF9;
	GPIOA->CRH |= GPIO_CRH_CNF9_1 |GPIO_CRH_MODE9_0 | GPIO_CRH_CNF10_0; 
   
   
//----------------------------------------------     USART
																		//Clocking USART and Alternative function
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_AFIOEN;
																							//Calculate boud rate
	
  USART1->BRR = F_CPU / (float)in_u32BaudRate;                    
																							//On TxD and RxD
  USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
																							//Start USART module
  USART1->CR1 |= USART_CR1_UE;
																			//Allow interrupt recieve message RX
  USART1->CR1 |= USART_CR1_RXNEIE;
																			//external NVIC handler Usart1
  NVIC_EnableIRQ(USART1_IRQn);		

/*------------------------------------------------
TIM2 will be create delay for signal control of DHT11
------------------------------------------------*/
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; //Clocking timer
  // Prescaler and overflow preference for: 100Khz, 10us
  TIM2->PSC = 2;  
  TIM2->ARR = 39;
  
  //Allowed interrupts if counter(overflow) full
  TIM2->DIER |= TIM_DIER_UIE;
  NVIC_EnableIRQ(TIM2_IRQn);  //Alowed interrupts in NVIC
  
  //Start TIM2
  TIM2->CR1 |= TIM_CR1_CEN;  
  
  __enable_irq ();   //Interrupts allow
}

/*------------------------------------------------  
TIM2 start couting
------------------------------------------------*/
void TIM2_Start()
{
   //Start TIM2
   TIM2->CR1 |= TIM_CR1_CEN;  
}

/*------------------------------------------------  
TIM2 stop couting
------------------------------------------------*/
void TIM2_Stop()
{
   //Stop TIM2
   TIM2->CR1 &= ~TIM_CR1_CEN;    
}


/*------------------------------------------------  
PA[8] for DHT11 output setting
------------------------------------------------*/
void DHT11_OutLeg_Setting()
{   
   GPIOC->CRH |= GPIO_CRH_CNF8_0;     //PA[8] - output, 50Mhz
   GPIOC->CRH &= ~GPIO_CRH_MODE8;
}

/*------------------------------------------------  
PA[8] for DHT11 input setting
------------------------------------------------*/
void DHT11_InLeg_Setting()
{
   GPIOC->CRH &= ~GPIO_CRH_CNF8;  //PA[8] - output, 50Mhz
   GPIOC->CRH |= GPIO_CRH_MODE8; 
   
}
/*------------------------------------------------  
PA[8] for DHT11 input setting
------------------------------------------------*/
int DHT11_LegStatus()
{   
  return(GPIOA->IDR & GPIO_IDR_IDR8);
}

//=======================================================  Send TX One Char Function
void USART1_Send(char chr)
{
																											//Verification of data transfer is complete
	while(!(USART1->SR & USART_SR_TC));
																												//Send Data
	USART1->DR = chr;
}

//=========================================================  Send TX string Function
void USART1_Send_String(char* str) 
{
 int i = 0;
																								//Perfomed while i < str.lenght
	while(str[i])
																													//Send symbol str[i], next cycle - i+1
		USART1_Send(str[i++]);	
}
