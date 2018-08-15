#include "stm32f10x.h"
/*==========================================================
This program will be Send and Get byte message from TX/RX and blinking PC[9] led
PC[9] - LED
PA[9]- TX
PA[10]- RX
==========================================================*/
void USART1_Send(char chr);											//Send one chr sunction
void USART1_Send_String(char* str);									//Send string function


//=============================================================  Usart1 Handler
void USART1_IRQHandler(void)
{
																	//what causind interruption, if byte went to RxD - treats
	if (USART1->SR & USART_SR_RXNE)
	{
																		//Reset Interription flag
		USART1->SR &= ~USART_SR_RXNE;
																				//Taked byte
		if(USART1->DR=='1') 
		{																		
			GPIOC->BSRR = GPIO_BSRR_BS9;                 //Led PC[9] - On
																		
		  USART1_Send_String("On\r\n");                 //Sending status line "On"
		}
		if(USART1->DR=='0')
		{
																		//Led PC[9] - off
			GPIOC->BSRR = GPIO_BSRR_BR9;
																		//Sending status Line "Off"
			USART1_Send_String("Off\r\n");
		}
		if(USART1->DR=='3')
		{
			if(USART1->DR=='1')
			{
				GPIOC->BSRR = GPIO_BSRR_BS9;
			}
		}
		
	}		
}

//======================================================== Initiall GpioA, GpioC, Usart
void InitAll()
{
//-----------------------------------------------   GpioC
																		//Clocking port C
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
																				//PC[9] - output, Push-pull, 2 Mhz
	GPIOC->CRH |= GPIO_CRH_MODE9_1;
	GPIOC->CRH &= ~GPIO_CRH_CNF9;
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
}

//=============================================================  MAin Function
int main(void)
{
	InitAll();
																					//Send Ready line - "Start"
	USART1_Send_String("Start\r\n");
	while(1);
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
