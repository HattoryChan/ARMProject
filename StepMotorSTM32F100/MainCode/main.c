#include "stm32f10x.h"
#include "delay.h"
#include "UsartInitAndRX_TXFunction.h"
/*-----------------------------------------------------------------------------------
this function will be controll stepMotor.
Port C:
PC[8,9] - indication leds
							--->    X motor
PC[10,11] - Contacts the first winding
PC[12,13] - contacts the seconds winding 
              <---   This pins will have switch transistor 
							
							--->    Y motor
PC[4,5]  - Contacts the first winding
PC[6,7]  - Contacts the seconds winding
							<---   This pins must have CMOS transistor(PNP and NPN), because L293d(driver) have high temperature ;(
Port A:
PA[9] - TX
PA[10] - RX
*/
void StepXClockWise(int NumberStep);           //Work with bipolar two winding
void StepXCounterSlockWise(int NumberStep);    //Work with bipolar two winding
void StepYClockWise(int NumberStep);           //Work with bipolar two winding
void StepYCounterSlockWise(int NumberStep);    //Work with bipolar two winding

int MessUart[5];   // coordinate maybe 0 - non, 1 - right, 2 - lenght
int LenghtMess;

//================================================= Usart Interrupt
void USART1_IRQHandler(void)
{  
	if (USART1->SR & USART_SR_RXNE)				//what causind interruption, if byte went to RxD - treats
	{	
		  USART1->SR &= ~USART_SR_RXNE;					//Reset Interription 
	//	  	RxStatus = 0;																//Set status to busy and write data to MessGSM
	//			PreMessGsm = USART1->DR;											//Write Usart data on variable
	 // 	MessUart[LenghtMess] = USART1->DR;										//Write valiable on your place in array
						//unput message: "XYZ\n"
	//if( MessUart[LenghtMess] == '\r')							//If we have "end symbol" in they Array,
	//	{						
	//		USART1->DR = '1';
       LenghtMess = 0;     //Refresh mass
   // if(MessUart[0] == 1)    //X coordinate
		if(USART1->DR == '1')
		{
		    StepXClockWise(40);
		}// else if(MessUart[0] == 2)
		 else if(USART1->DR == '2')
		 {
			 StepXCounterSlockWise(40);
		 } 
		 if(MessUart[1] == 1)    //Y coordinate
		{
		    StepYClockWise(1);
		} else if(MessUart[1] == 2)
		 {
			 StepYCounterSlockWise(1);
		 }    
			
//			RxStatus = 1;															//SET '1' when first string complited, if we get second string, 
//																								//      this value refresh and set to '1' when second string ready
	//	}
	     LenghtMess++;	                      //GSM array lenght, you must refresh this valiable in next function
	}  

	
}
//======================================   Initiall Legs
void InitiallAll()
{
	//--------------------------------------------					GPIO
																																									//clocking Port [A,C]
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;  															
																																												//PC[8,9,10,11,12,13] - outputs, Fmax = 2Mhz
	GPIOC->CRH &= ~(GPIO_CRH_CNF8 | GPIO_CRH_CNF9 | GPIO_CRH_CNF10 | GPIO_CRH_CNF11 | GPIO_CRH_CNF12 | GPIO_CRH_CNF13);	
	GPIOC->CRH |= GPIO_CRH_MODE8_0 | GPIO_CRH_MODE9_0 | GPIO_CRH_MODE10_0 | GPIO_CRH_MODE11_0 | GPIO_CRH_MODE12_0 | GPIO_CRH_MODE13_0;
	GPIOC->CRL &= ~(GPIO_CRL_CNF4 | GPIO_CRL_CNF5 | GPIO_CRL_CNF6 | GPIO_CRL_CNF7);
  GPIOC->CRL |= GPIO_CRL_MODE4_0 | GPIO_CRL_MODE5_0 | GPIO_CRL_MODE6_0 | GPIO_CRL_MODE7_0;	
}

int main(void)
{
	InitiallAll();
	
USART1_Send_String("H\r\n");
		delay(1);
	StepXClockWise(10);
	delay(1);
	StepYClockWise(10);
	while(1);
			
//	StepXClockWise(50);
//		delayms(400);
//	GPIOC->ODR ^= GPIO_ODR_ODR9;
//	StepXCounterSlockWise(50);
//		delayms(400);
		
//		StepYClockWise(50);
//		delayms(400);
//	GPIOC->ODR ^= GPIO_ODR_ODR8;
//	StepYCounterSlockWise(50);
//		delayms(400);
	
	
}

//===========================================  on the FOUR steps for X motor
void StepXClockWise(int NumberStep)   //Doing four step on ClockWise 
{ /*
	  PC[10,11] -> First  winding
	  PC[12,13] -> Second winding
	*/
		int i;
		for(i = 0; i <= NumberStep; i++)
	{                         //Than made 1 step(four small steps)
		//----------------------------------------------------
		GPIOC->BSRR = GPIO_BSRR_BS11;  
		GPIOC->BSRR = GPIO_BSRR_BR10;  
    delayms(2);
		
		GPIOC->BSRR = GPIO_BSRR_BR11; 
		delayms(1);
		//--------------------------------------------------
		GPIOC->BSRR = GPIO_BSRR_BR12; 
		GPIOC->BSRR = GPIO_BSRR_BS13; 
		delayms(2);
		
		GPIOC->BSRR = GPIO_BSRR_BR13; 
		delayms(1);    		
		//-------------------------------------------------------
		GPIOC->BSRR = GPIO_BSRR_BR11; 
		GPIOC->BSRR = GPIO_BSRR_BS10;     		
		delayms(2);
		
		GPIOC->BSRR = GPIO_BSRR_BR10; 
		delayms(1);
		//---------------------------------------------------
		GPIOC->BSRR = GPIO_BSRR_BS12; 
		GPIOC->BSRR = GPIO_BSRR_BR13; 
		delayms(2);		
		
		GPIOC->BSRR = GPIO_BSRR_BR12; 
		delayms(1);
		
	}	

}

//============================================== Doing four steps counter-clockwise for X motor
void StepXCounterSlockWise(int NumberStep)
{ /*
	  PC[10,11] -> First  winding
	  PC[12,13] -> Second winding
	*/
		int i;
		for(i = 0; i <= NumberStep; i++)
	{                         //Than made 1 step(four small steps)
		//----------------------------------------------------
		GPIOC->BSRR = GPIO_BSRR_BS10;   //ON
		GPIOC->BSRR = GPIO_BSRR_BR11;   //OFF
    delayms(2);
		
		GPIOC->BSRR = GPIO_BSRR_BR10; //OFF
		delayms(1);
		//--------------------------------------------------
		GPIOC->BSRR = GPIO_BSRR_BR12;   //OFF
		GPIOC->BSRR = GPIO_BSRR_BS13;   //ON
		delayms(2);
		
		GPIOC->BSRR = GPIO_BSRR_BR13; //OFF
		delayms(1);    		
		//-------------------------------------------------------
		GPIOC->BSRR = GPIO_BSRR_BR10;   //OFF
		GPIOC->BSRR = GPIO_BSRR_BS11;   //ON    		
		delayms(2);
		
		GPIOC->BSRR = GPIO_BSRR_BR11; //OFF
		delayms(1);
		//---------------------------------------------------
		GPIOC->BSRR = GPIO_BSRR_BS12;   //ON
		GPIOC->BSRR = GPIO_BSRR_BR13;   //OFF
		delayms(2);		
		
		GPIOC->BSRR = GPIO_BSRR_BR12; //OFF
		delayms(1);
		
	}	
}

//====================================================  clock wise step for Y motor
void StepYClockWise(int NumberStep)   //Doing four step on ClockWise 
{ /*
	  PC[4,5] -> First  winding
	  PC[6,7] -> Second winding
	*/
		int i;
		for(i = 0; i <= NumberStep; i++)
	{                         //Than made 1 step(four small steps)
		//----------------------------------------------------
		GPIOC->BSRR = GPIO_BSRR_BS5;  
		GPIOC->BSRR = GPIO_BSRR_BR4;  
    delayms(2);
		
		GPIOC->BSRR = GPIO_BSRR_BR5; 
		delayms(1);
		//--------------------------------------------------
		GPIOC->BSRR = GPIO_BSRR_BR6; 
		GPIOC->BSRR = GPIO_BSRR_BS7; 
		delayms(2);
		
		GPIOC->BSRR = GPIO_BSRR_BR7; 
		delayms(1);    		
		//-------------------------------------------------------
		GPIOC->BSRR = GPIO_BSRR_BR5; 
		GPIOC->BSRR = GPIO_BSRR_BS4;     		
		delayms(2);
		
		GPIOC->BSRR = GPIO_BSRR_BR4; 
		delayms(1);
		//---------------------------------------------------
		GPIOC->BSRR = GPIO_BSRR_BS6; 
		GPIOC->BSRR = GPIO_BSRR_BR7; 
		delayms(2);		
		
		GPIOC->BSRR = GPIO_BSRR_BR6; 
		delayms(1);
		
	}	

}

//============================================== Doing four steps counter-clockwise for Y motor
void StepYCounterSlockWise(int NumberStep)
{ /*
	  PC[4,5] -> First  winding
	  PC[6,7] -> Second winding
	*/
		int i;
		for(i = 0; i <= NumberStep; i++)
	{                         //Than made 1 step(four small steps)
		//----------------------------------------------------
		GPIOC->BSRR = GPIO_BSRR_BS4;   //ON
		GPIOC->BSRR = GPIO_BSRR_BR5;   //OFF
    delayms(2);
		
		GPIOC->BSRR = GPIO_BSRR_BR4; //OFF
		delayms(1);
		//--------------------------------------------------
		GPIOC->BSRR = GPIO_BSRR_BR6;   //OFF
		GPIOC->BSRR = GPIO_BSRR_BS7;   //ON
		delayms(2);
		
		GPIOC->BSRR = GPIO_BSRR_BR7; //OFF
		delayms(1);    		
		//-------------------------------------------------------
		GPIOC->BSRR = GPIO_BSRR_BR4;   //OFF
		GPIOC->BSRR = GPIO_BSRR_BS5;   //ON    		
		delayms(2);
		
		GPIOC->BSRR = GPIO_BSRR_BR5; //OFF
		delayms(1);
		//---------------------------------------------------
		GPIOC->BSRR = GPIO_BSRR_BS6;   //ON
		GPIOC->BSRR = GPIO_BSRR_BR7;   //OFF
		delayms(2);		
		
		GPIOC->BSRR = GPIO_BSRR_BR6; //OFF
		delayms(1);
		
	}	
}
