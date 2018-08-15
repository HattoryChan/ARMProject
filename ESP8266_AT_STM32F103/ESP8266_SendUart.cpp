/*==========================================================
This library will be Send and Get byte message from TX/RX, you need create Usart1_IRQHandler
PA[9]- TX
PA[10]- RX
==========================================================*/
#include "stm32f10x.h"
/*
//=============================================================  Usart1 Handler
void USART1_IRQHandler(void)
{
			
}
*/
/*=======================================================================
                          Initiall and work with UART part
========================================================================*/
void Init_STM32ToESP8266Uart()
{
//------------------------------------------  GpioA
																		//Clocking port A
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
																		//PA[9] - output,Alternative function,pish-pull
																		//PA[10] - input,Hi-z
	GPIOA->CRH &= ~GPIO_CRH_CNF9;
	GPIOA->CRH |= GPIO_CRH_CNF9_1 |GPIO_CRH_MODE9_0 | GPIO_CRH_CNF10_0;
//----------------------------------------------     USART
																		//Clocking USART and Alternative function
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_AFIOEN;
																							//Clocking register, 9600 bit rate
	USART1->BRR = 0xA2C;
																							//On TxD and RxD
	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
																							//Start USART module
   USART1->CR1 |= USART_CR1_UE;
																			//Allow interrupt recieve message RX
   USART1->CR1 |= USART_CR1_RXNEIE;
																			//external NVIC handler Usart1
   NVIC_EnableIRQ(USART1_IRQn);	
  NVIC_SetPriority(USART1_IRQn,1);   
}

//=======================================================  Send TX One Char Function
void ESP8266_Send(char chr)
{
																											//Verification of data transfer is complete
	while(!(USART1->SR & USART_SR_TC));
																												//Send Data
	USART1->DR = chr;
}

//=========================================================  Send TX string Function
void ESP8266_SendStr(char* str) 
{
 int i = 0;
																								//Perfomed while i < str.lenght
	while(str[i])
																													//Send symbol str[i], next cycle - i+1
		ESP8266_Send(str[i++]);	
}


