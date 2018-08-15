#include "stm32f10x.h" 
#include "Initial_Periphery.h"

#include <math.h>         // Mathematical action on program 
#include <stdio.h>        //sprintf()
#include <stdlib.h>       //atoi


/*================================================================
This program will be Send ADC result to Usart
PA[1] - ADC
PA[9] - TX
Termistor between +V and ADC1
================================================================*/
#define d_SecResInDivValue 10000.0  
#define d_ADCVrefValue 3.30
#define d_ADCSamplNumb 4096.0
#define d_CommonDivideRes25 5000.0

#define d_TermCoef_B 4300.0
#define d_KelvinZeroT 273.15
#define d_Kelvin25T 298.15

void TemperatureToUart(uint16_t ADC_Res);                         //Send temperature to USART
void BreakUint(uint16_t);														   //Splits message and send to Uart
void VoltageToUsart(uint16_t ADC_Res);                               //Send temperature to USART
void ResistanceToUsart(uint16_t ADC_Res);                            //Send resistance to USART


//========================================  Main Function
int main()
{	
	uint16_t ADC_Res;							// Adc Result value	
	Initial_Periphery();			
																									
   USART1_Send_String("ready\r\n");          //Cycle by send TXdata character on sendidg function.		
   delay(1);                                //Delay to check usart
																		
	ADC1->CR2 |= ADC_CR2_ADON;                //ADC ON																
		
while(1)
	{
      ADC1->CR2 |= ADC_CR2_JSWSTART;      //Start external ADC1
																	
      while(!(ADC1->SR & ADC_SR_JEOC));      //Wait till the first external complited
																											
      while(!(USART1->SR & USART_SR_TC)){};					//Verification of data transfer is complete	
          ADC_Res = ADC1->JDR1;      									//12 bit Register = 4096 value
     TemperatureToUart(ADC_Res);
   //VoltageToUsart(ADC_Res);
  // ResistanceToUsart(ADC_Res);
         delay(1);		                              //Send message every second
	}
}

//======================================================== Splits message into two and send him to Uart
void BreakUint(uint16_t ADC_Res)
{
	   uint8_t HiByteADC, LowByteADC;		//For Hi and Low byte ADC  
     LowByteADC =	(ADC_Res & 0xFF);								//Make 2 mess Hi and Low byte
    HiByteADC = ((ADC_Res & 0xFF00) >> 8);		
																						
																					//Send Data adc to uart on 2 message Dec
		while(!(USART1->SR & USART_SR_TC)){};
     USART1->DR = HiByteADC;
	  while(!(USART1->SR & USART_SR_TC)){};							//Verification of data transfer is complete
     USART1->DR = LowByteADC;   
			USART1_Send_String("\r\n");
}

//======================================================= Get ADC message, counted him on Temperature and send to Usart
void TemperatureToUart(uint16_t ADC_Res)
{	 
	uint16_t ToPoint, AfterPoint;		//For Hi and Low byte ADC
	char mess[8];
	double Temp, Resist, CalcToPoint, CalcAfterPoint;	    
		 
       Resist = d_SecResInDivValue -(((ADC_Res * d_ADCVrefValue / d_ADCSamplNumb) * d_SecResInDivValue) / d_ADCVrefValue);   
	    Temp = (1.0/((1.0/d_Kelvin25T)+((1.0/d_TermCoef_B)*log(Resist/d_CommonDivideRes25)))) - d_KelvinZeroT;
		             
 //==================------------  Prepare Temp to send Usart
      CalcAfterPoint = modf(Temp,&CalcToPoint);    //To point part in CalcToPoint, After point part in CalcAfterPoint																																																												//Modf not working, maybe need uint16_t
   	ToPoint = CalcToPoint;
      AfterPoint = CalcAfterPoint*100;		       //4 numbers after point
 //==================-------------   Send Data
		  sprintf(mess, "%d",ToPoint);							//To point value to char																					
		 	USART1_Send_String(mess);
		     USART1_Send('.');
		 sprintf(mess, "%d", AfterPoint);					//To point value to char		
		 USART1_Send_String(mess);   
       USART1_Send_String("\r\n"); 
}

//======================================================= Get ADC message, counted him on voltage and send to Usart
void VoltageToUsart(uint16_t ADC_Res)
{
	
	uint16_t ToPoint, AfterPoint;		//For Hi and Low byte ADC
	char mess[8];
	double Volt,CalcToPoint, CalcAfterPoint;			  	                  
	     				
		   Volt = ADC_Res*d_ADCVrefValue/d_ADCSamplNumb;   			
   
		  CalcAfterPoint = modf(Volt,&CalcToPoint);    //To point part in CalcToPoint, After point part in CalcAfterPoint																																																												//Modf not working, maybe need uint16_t
   		ToPoint = CalcToPoint;
      AfterPoint = CalcAfterPoint*100;		       //4 numbers after point
																												//Send Data
		  sprintf(mess, "%d",ToPoint);							//To point value to char																					
		  	USART1_Send_String(mess);
		    USART1_Send('.');
		  sprintf(mess, "%d", AfterPoint);					//To point value to char		
		    USART1_Send_String(mess);
       USART1_Send_String("\r\n");	
}

//======================================================= Get ADC message, counted him on resistance and send to Usart
void ResistanceToUsart(uint16_t ADC_Res)
{
	
	uint16_t ToPoint, AfterPoint;		//For Hi and Low byte ADC
	char mess[8];
	double Resist,CalcToPoint, CalcAfterPoint;			  	                  
	    
		  Resist = d_SecResInDivValue -(((ADC_Res * d_ADCVrefValue / d_ADCSamplNumb) * d_SecResInDivValue) / d_ADCVrefValue);   //between 3.3 and Analog leg in divider
      //Resist = ((ADC_Res * d_ADCVrefValue / d_ADCSamplNumb) * d_SecResInDivValue) / d_ADCVrefValue;   //between GND and Analog leg in divider
   
		  CalcAfterPoint = modf(Resist,&CalcToPoint);    //To point part in CalcToPoint, After point part in CalcAfterPoint																																																												//Modf not working, maybe need uint16_t
   		ToPoint = CalcToPoint;
         AfterPoint = CalcAfterPoint*100;		       //4 numbers after point
																												//Send Data
		  sprintf(mess, "%d",ToPoint);							//To point value to char																					
		  	USART1_Send_String(mess);
		    USART1_Send('.');
		  sprintf(mess, "%d", AfterPoint);					//To point value to char		
		    USART1_Send_String(mess);
       USART1_Send_String("\r\n");	
}


