#include "stm32f10x.h" 

/*
This programm use to Connect Stm32VL-DISCOVERY -> Usart
			We send message "STM32F103RN"
*/
#define F_CPU 24000000L
													//Advert initial function GPIO and USART1
void Init(void); 
													// Advert function sending message(one char symbol)
void Transmit(char); 

int main()
{
															//i for cycle delay
	uint32_t i = 0;
															// c for Send message cycle 
	int16_t c = 0;
															//Calling function initialization
  Init();
   
  while(1)
  {
															//Calling function seding single character on char massive.
		char TXdata[] = { 'S', 'T', 'M', '3', '2', 'F', '1', '0', '0',
											'r', 'n'};  				//Transfer position in leftmost position [r],new line [n]
															//Cycle by send TXdata character on sendidg function.											
   for(c = 0; sizeof(TXdata)/sizeof(int)+1 >= c ; c++)
	{ 
		Transmit(TXdata[c]);
	}		
															//REset sending Cicle
	  c = 0;		
    for(i=0; i<0x000FFFFF; i++); 
  }
}

void Init()
{
   uint32_t Temp = 9600;
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
   
   USART1->BRR = F_CPU / Temp;
	
																			// Bit rate - 9600,Fbus = 25Mhz
 // USART1->BRR = 0xA2C; 
																				//On for transfer USART1
  USART1->CR1 |= USART_CR1_TE;  
}
																					//Send Data to Usart
void Transmit(char data)
{
																				//Verification of data transfer is complete
  while(!(USART1->SR & USART_SR_TC)); 
																				//Send data
  USART1->DR = data;  
}
