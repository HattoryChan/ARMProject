#include "stm32f10x.h"
/*
This programm will be sending bits to  radio module, and get to usart
SPI1 - master
Port A:
PA7 - MOSI
PA6 - MISO
PA5 - sck
PA4 - NSS
*/

void Transmit(char);

																

																		//Initial function
void InitGPIO()
{
																		//SPI1 line- master Port A[4,5,6,7]
																		//Clocking to PORT a and alternative function
	RCC->APB2ENR |= (RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN);
																		//Sets CNF and MODE bits.
                     	//Port A[4,5,7]: PA7(MOSI),PA5(SCK), PA4(NSS) -AF, Output, PP	
	GPIOA->CRL |= GPIO_CRL_MODE7 | GPIO_CRL_MODE5 | GPIO_CRL_MODE4;
	GPIOA->CRL |= GPIO_CRL_CNF7_1 | GPIO_CRL_CNF5_1 | GPIO_CRL_CNF4_1;
												//Port A[6]: PA6(MISO) - input, normal, open collector
	GPIOA->CRL &= ~GPIO_CRL_MODE6;
	GPIOA->CRL |= GPIO_CRL_CNF6_0;																	
	
}
																	//Initialization SPI1-Master
void InitSPI1()
{
/*
	8 bits, MSB sets first, hardware mode NSS(PA4 - allow use output)
*/
																	//Clocking SPI module
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
																	//Baud rate = Fpclk/256; 
																	//[Br1,Br2,Br3 = 1,1,1]
  SPI1->CR1 |= SPI_CR1_BR;
																	//Polar clock signal
	SPI1->CR1 &= ~SPI_CR1_CPOL;
																	//Phase clock signal
	SPI1->CR1 &= ~SPI_CR1_CPHA;
																	//MSB transmited first
	SPI1->CR1 &= ~SPI_CR1_LSBFIRST;
																	//Programming NSS input
	SPI1->CR1 |= SPI_CR1_SSM;
																	//NSS High Voltage
	SPI1->CR1 |= SPI_CR1_SSI;
																	//Nss- control Slave select
	SPI1->CR2 |= SPI_CR2_SSOE;
																	//Master Mode
  SPI1->CR1 |= SPI_CR1_MSTR;
																	// On SPI1
  SPI1->CR1 |= SPI_CR1_SPE;																	
}
																	//Initial USART
void InitUSART()
{
																				//Clocking for USART1 and Port A
	RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN); 
																				// Port A[9] - output, 50Mhz. Alternative mode, push-pul
																					// Mode9[1,1]  CNF[1,0]
  GPIOA->CRH |= (GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9); 
																				//USART1 On
  USART1->CR1 |= USART_CR1_UE; 
																				//The dimension word - 8 bit
  USART1->CR1 &= ~USART_CR1_M; 
																				// Stop bit - 1
  USART1->CR2 &= ~USART_CR2_STOP; 
																				// Bit rate - 9600,Fbus = 25Mhz
  USART1->BRR = 0xA2C; 
																				//On for transfer USART1
  USART1->CR1 |= USART_CR1_TE;  
	
}
																		//Function ExchangeSpiToUsart
void ExchangeSpi()
{
																		//Wait for sending SPI/UART complited
	while(!(SPI1->SR & SPI_SR_RXNE));	
	while(!(USART1->SR & USART_SR_TC));
	Transmit(SPI1->DR);
}
																					//Send Data to Usart
void Transmit(char data)
{
																				//Verification of data transfer is complite
  while(!(USART1->SR & USART_SR_TC)); 
																				//Send data
  USART1->DR = data; 
}

int main(void)
{
	InitGPIO();
	InitSPI1();
	InitUSART();
	Transmit('1');	
																				//R_Register setup
	SPI1->DR = 0x20;             //0b00100000	
	SPI1->DR = 0x0;                  //0b00000000	
																						//PWR_UP to 1
	SPI1->DR = 0x2;										//0b00000010;
																		//NRF by standby mode
	 
	USART1->DR = SPI1->DR;
	while(1)
	{
	//	ExchangeSpi();
		
	}
}
