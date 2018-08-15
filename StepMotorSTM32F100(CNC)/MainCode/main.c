#include "stm32f10x.h"
#include "delay.h"
#include "UsartInitAndRX_TXFunction.h"

//Last update: 4.11.2016

/*-----------------------------------------------------------------------------------
this function will be controll stepMotor.
Port C:
PC[8,9] - indication LED
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
void StepXCounterClockWise(int NumberStep);    //Work with bipolar two winding
void StepYClockWise(int NumberStep);           //Work with bipolar two winding
void StepYCounterClockWise(int NumberStep);    //Work with bipolar two winding
void HandlerForUart(void);                     //Give uart command to step motor
int CheckValue(int StartNumb);   							 //Get Array numb and return Int value(" xxxXxxxYxxxZ+'\n'")
void HandlerForUartV2(void);  								 //Give uart command to step motor(add new command structure " xxxXxxxYxxxZ+'\n'")
	//  <- Don't work ;(

uint16_t MessUart[20];   // coordinate maybe 0 - non, 1 - right, 2 - lenght
int LenghtMess = 0;
int RxStatus = 0;						//That flag: 0 - uart busy, 1 - mess ready

//================================================= Usart Interrupt
void USART1_IRQHandler(void)
{  
	if (USART1->SR & USART_SR_RXNE)				//what causind interruption, if byte went to RxD - treats
	{	
		  USART1->SR &= ~USART_SR_RXNE;					//Reset Interription 
		 RxStatus = 0;                              //input message		
		
		MessUart[LenghtMess] = USART1->DR;         //Write input data on MessUart
		
		 if(MessUart[LenghtMess] == '\n')     //If we have end message
		 {
			 RxStatus = 1; 														//Message ready
				LenghtMess = 0;												//Refresh count
			 HandlerForUart();											//Mess structure: "XYZ'\r''\n'"
		 } else		 
				LenghtMess ++;														//Add one point to count		
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
	InitiallAll();											//Initiall GPIO
	InitUART();																//Init UART                
	
 USART1_Send_String("Start\r\n");
		delay(1);
	StepXClockWise(20);
		delay(1);
	StepYClockWise(20);
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
void StepXCounterClockWise(int NumberStep)
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
void StepYCounterClockWise(int NumberStep)
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

//============================================== Uart message Handler
void HandlerForUart()														
{
 	if(MessUart[1] == '1')
	{
		GPIOC->ODR ^= GPIO_ODR_ODR8;
			delayms(10);
		
		StepXClockWise(1);
		
	} else  if(MessUart[1] == '2')
	{
		GPIOC->ODR ^= GPIO_ODR_ODR8;
			delayms(10);
		
		StepXCounterClockWise(1);
		
	}
	if(MessUart[2] == '1')
	{
		GPIOC->ODR ^= GPIO_ODR_ODR9;
			delayms(10);
		
		StepYClockWise(1);
		
	} else if(MessUart[2] == '2')
	{
		GPIOC->ODR ^= GPIO_ODR_ODR9;
			delayms(10);
		
		StepYCounterClockWise(1);
		
	}
}

//================================================ //Try new message structure: " xxxXxxxYxxxZ'\n'"
void HandlerForUartV2()
{
	// Input message structure: " 012X123Y000Z+'\n'"    'X' - 4, 'Y' - 8, 'Z' - 12, '+' or '-' - 13 (set step vector) 
		//Maybe add in program End message transmition interrupts and call this function
	if(MessUart[4] == 'X' && MessUart[13] == '+')   				//make step for clockwise X
	{
		StepXClockWise(CheckValue(1));
	}
	if(MessUart[4] == 'X' && MessUart[13] == '-')						//make step for counter clock wise X
	{
		StepXCounterClockWise(CheckValue(1));
	}
	if(MessUart[8] == 'Y' && MessUart[13] == '+')           //make step for clock wise Y
	{
		StepYClockWise(CheckValue(5));
	}
	if(MessUart[8] == 'Y' && MessUart[13] == '-')						//make Step for counter clock wise Y
	{
		StepYCounterClockWise(CheckValue(5));
	}
																															//And we don't have pins and another function for Z. This end
}


int CheckValue(int StartNumb) 															//Will be check Message and return value in INT
{
	int V,V1,V2;
																						//Check array to NOT Zero value
		if(MessUart[StartNumb] != 0)											//First
		{
			V = MessUart[StartNumb];										
		} 
		if(MessUart[StartNumb+1] != 0)										//Second
		{
			V1 = MessUart[StartNumb+1];
		}
		if(MessUart[StartNumb+2] != 0)										//Third
		{
			V2 = MessUart[StartNumb+2];
		} 
		if(MessUart[StartNumb] == 0 && MessUart[StartNumb+1] == 0 && MessUart[StartNumb+2] == 0)	//if All value equals zero, return 0
		{
			return 0;
		}
	  return(V*100 + V1*10 + V2);             //Summ all value to Integer and return it	
}
