#include "stm32f10x.h" 

/*
Initiall periphery for work with ADC and UART?, and function for send message to USART. And delay function
*/
/*================================================================
This program will be Send ADC result to Usart
PA[1] - ADC
PA[9] - TX

================================================================*/

void Initial_Periphery()
{
//---------------------------- GpioA	
																					//Clocking for USART1 and Port A
  RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN); 
																					// Usart Output
																			// Port A[9] - output, 50Mhz. Alternative mode, push-pul
																			// Mode9[1,1]  CNF[1,0]
  GPIOA->CRH |= (GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9); 
																					// ADC input
																			//PA[1]- input, Hi-z
	GPIOA->CRL &= GPIO_CRL_MODE1;
	GPIOA->CRL |= GPIO_CRL_CNF1_0;
	
//------------------------------------    ADC
																										//Clocking ADC
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
																									//Calibration ADC
																							//Start
	ADC1->CR2 |= ADC_CR2_CAL;
																				//Wait calibration ending
	while(!(ADC1->CR2 & ADC_CR2_CAL)){}		//<---- This litle MAGIC
	
																//Set the sampling duration
  ADC1->SMPR2 |= (ADC_SMPR2_SMP1_2 | ADC_SMPR2_SMP1_1 | ADC_SMPR2_SMP1_0);
																						//Conversion of the injected group
																	//Strart with set JSWSTART bit
	ADC1->CR2 |= ADC_CR2_JEXTSEL;
																	//Allow start external injected group
	ADC1->CR2 |= ADC_CR2_JEXTTRIG;	
																	//external start in series
	ADC1->CR2 |= ADC_CR2_CONT;  		
																	//Allow start injected group after regular group
	ADC1->CR2 |= ADC_CR1_JAUTO;
																	//Sets chanel Number(ADC1)
	ADC1->JSQR |= (1<<15);	
																	// Result >>
	ADC1->CR2 &= ~ADC_CR2_ALIGN;

//--------------------------------------		USART		
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


//---------------------------------------------         Delay function (sec)
void delay(uint16_t sec)
{
	uint32_t i;
 for (; sec != 0; sec--)
	{
		for(i = 0x5B8D80; i !=0; i--);										// 1 second delay if Fmax = 72 Mhz
	}
}

