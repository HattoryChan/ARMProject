#include "stm32f10x.h" 
/*=============================================================================================
This program will be use 74HC595 and on\off led array.
DS --> Pa0
ST_CP --> Pa1
SH_CP --> Pa2
MR --> Pa3
==============================================================================================*/
void delay(uint16_t sec);
void delayms(uint16_t ms);
void ShiftRegister(int prior);

void InitiallAll()
{
//		-------------------------------------- GPIO[A]
																										
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;												//Clocking for Port[A]
																														//PA[0,1,2,3] - output, push-pull, 10Mhz
	GPIOA->CRL |= GPIO_CRL_MODE0_1 | GPIO_CRL_MODE1_1 | GPIO_CRL_MODE2_1 | GPIO_CRL_MODE3_1;
	GPIOA->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_CNF1 | GPIO_CRL_CNF2 | GPIO_CRL_CNF3);
		
}
int main()
{
	uint8_t i;
	uint8_t n;							// for cycle array
	uint8_t array[] = {0,1,1,0,0,1,1,0};
	uint8_t ShiftArray[7];
	 InitiallAll();
	 	//Prepare to work
																	//Clear Register
	GPIOA->BSRR = GPIO_BSRR_BR3;													//MR --> Low
	delayms(10);
	GPIOA->BSRR = GPIO_BSRR_BS1;													//ST_CP --> High
  	delayms(20);
	
	n = 7;										//Number array last element
	while(1)
	{
	//------------------------------------- Shift array to 1 point right. n- array size	
 	for (i = 0; i < n; i++)											//Copy array to ShiftArray  	
		{
		 ShiftArray[i] = array[i];		
		}
	  array[0] = array[n];	 	
		 for(i = 0; i < n; i++)								//Paste array to array		
      {		 
			 array[i+1] = ShiftArray[i];		   
	    }
			
	//------------------------------------------ END Shift		
	for(i = 0; i < n; i++)
	{
		ShiftRegister(array[i]);		
	}
																//Click byte to register
	GPIOA->BSRR = GPIO_BSRR_BS1;                  //ST_CP --> High
			delayms(30);
	GPIOA->BSRR = GPIO_BSRR_BR1;             		//ST_CP --> low
			
	delayms(50);
  }
}

//------------------------------------------------    74HC595 function
void ShiftRegister(int prior)
{																									
																		//Work mode
	GPIOA->BSRR = GPIO_BSRR_BR1;                           //ST_CP --> Low
  GPIOA->BSRR = GPIO_BSRR_BS3;													//MR --> High
	GPIOA->BSRR = GPIO_BSRR_BR2;													//SH_CP --> Low	
	delayms(1);
		if(prior == 1)
		{
			GPIOA->BSRR = GPIO_BSRR_BS0;								//DS --> Set log.'1'
		}
    if(prior == 0)
			{
				GPIOA->BSRR = GPIO_BSRR_BR0;							//DS --> Set log '0'
			}	
		if(prior == 2)
		  {
				GPIOA->BSRR = GPIO_BSRR_BR3;							//MR --> Low (will be clear register)
			}			
	GPIOA->BSRR = GPIO_BSRR_BS2;													//SH_CP --> High
			delayms(1);					
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
