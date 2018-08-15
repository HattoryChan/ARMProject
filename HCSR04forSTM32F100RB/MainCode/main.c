#include "stm32f10x.h"
#include <string.h>
#include <math.h>         // Mathematical action on program 
#include <stdio.h>        //sprintf()
#include <stdlib.h>       //atoi
//==========================================  define

#define F_CPU 72000000UL 
#define TimerTick F_CPU/202000-1	//  1us
/*
This programm can send distanse(HcSr04) to  USART1.
USART:
Port A [9] - tx 
HCSR-04   
Port A [2] - trig
Port A [1] - echo, input
*/

void InitialAll(void);
void delay(uint16_t sec);
void delayms(uint16_t ms);
void delayus(uint16_t us);
void USART1_Send_String(char* str);
void USART1_Send(char chr);

uint16_t impulseTime;
uint8_t changeAction = 0;   			//switch for upper and lowwer impulse
char Range[10];									//sm

//-------------------------------------------     EXTI2 Handler
void EXTI1_IRQHandler(void)
{	
	USART1_Send_String("Get Signal\r\n");	
			
    if(changeAction == 1)													// pulse end
		{
		 sprintf(Range, "%d",impulseTime/58);            //calculate distance on sm
		USART1_Send_String(Range);	
				USART1_Send_String("\r\n<====  Value\r\n");
			changeAction = 0;															//refresh
		}
	 if(changeAction == 0) 
	{
		USART1_Send_String("Start and give Value ===>\r\n");
	changeAction++;						//pulse start
		 SysTick->VAL = TimerTick;					//RESET value SysTik
		impulseTime = 0;	
		
	}
	
	//Clear Interrupts flag
	EXTI->PR |= EXTI_PR_PR1;																							
}

//--------------------------------------------     SysTick Interrupt
void SysTick_Handler(void)
{
 impulseTime++;
}	
//=-=--=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-
//-------------------------------------------------  Main Function
int main()
{
	InitialAll();	
	//USART1_Send_String("Ready");
																											//create impulse for TRIG pins
		GPIOA->BSRR = GPIO_BSRR_BS2;    //On
		delayus(200);
		GPIOA->BSRR = GPIO_BSRR_BR2;    //Off
		
}
//======================================================= Initiall All Ports and Periph
void InitialAll()
{
//---------------------------- GpioA	
																					//Clocking for USART1 and Port A and Interrupts and TIM2
  RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN | RCC_APB2ENR_AFIOEN | RCC_APB1ENR_TIM2EN); 
																					// Usart Output
																			// Port A[9] - output, 50Mhz. Alternative mode, push-pul
																			// Mode9[1,1]  CNF[1,0]
  GPIOA->CRH |= (GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9); 
																					//Trig output
																	//PA[2] - output, push-pull, 2Mhz
	GPIOA->CRL |= GPIO_CRL_MODE2;
	GPIOA->CRL &= ~GPIO_CRL_CNF2;
																					// Echo input
																	//PA[1] - input, Push-pull for PA1, with RES
	GPIOA->CRL &= ~GPIO_CRL_MODE1;
																															//Cleat bits (default value cnf[0,1])
	GPIOA->CRL &= ~GPIO_CRL_CNF1;													
	GPIOA->CRL |= GPIO_CRL_CNF1_1;	

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
	
//--------------------------------------------					EXTI1
	//---------------------------------------------         Allow global interrupts
	__enable_irq();
																						//Clocking AFIO(EXTI1)
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
																						//Setting EXTI1 for PA[1];
	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI1_PA;
	
	EXTI->RTSR |= EXTI_RTSR_TR1;											//Call it if get HIGH signal on PA1 leg
	EXTI->FTSR |= EXTI_FTSR_TR1;			          	   	//Call it if get LOW signal on PA1 leg
	//--------------------------------------------------        Enable this in CMSIS
	NVIC_EnableIRQ(EXTI1_IRQn);												//EXTI1
	//--------------------------------------------------        NVIC Priority Interrupts and Timer
  NVIC_SetPriority(EXTI1_IRQn,1);										//EXTI1	
																						//Enable interrupts on periphery
	EXTI->IMR |= EXTI_IMR_MR1;	
//-----------------------------------------------------     System Timer
	//Config SysTimer
														//Load TimerTisk(1kHz) value on SysTick
	SysTick->LOAD = TimerTick;  
	SysTick->VAL = TimerTick;					//RESET value
														//Reset Timer and flag
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
									SysTick_CTRL_TICKINT_Msk   |
									SysTick_CTRL_ENABLE_Msk;

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

//-----------------------------------------------    Delay Function (us)
void delayus(uint16_t us)
{
	uint32_t i;
 for(; us !=0; us--)
	{
		for(i = 0x11; i != 0; i--);											// 1us delay if Fmax = 72mHz
	}
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
