
#include "STM32_Termistor_Library.h"
#include "stdio.h"    // for ReturnDataOnChar function



/*  Initiall ADC1 on PA[1]
*   Input: 
*   Output:
*/
void Init_STM32F1_ADC()
{																										
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;  //Clocking ADC
										//Calibration tho the ADC part
	ADC1->CR2 |= ADC_CR2_CAL;		//Start preparing ADC1
	while(!(ADC1->CR2 & ADC_CR2_CAL));		//Wait calibration ending
									//Set the sampling duration
  ADC1->SMPR2 |= (ADC_SMPR2_SMP1_2 | ADC_SMPR2_SMP1_1 | ADC_SMPR2_SMP1_0);
										//Conversion of the injected group
																	
	ADC1->CR2 |= ADC_CR2_JEXTSEL;   //Start with set JSWSTART bit																	
	ADC1->CR2 |= ADC_CR2_JEXTTRIG;	//Allow start external injected group																	
	ADC1->CR2 |= ADC_CR2_CONT;  		//external start in series																	
	ADC1->CR2 |= ADC_CR1_JAUTO;     //Allow start injected group after regular group																	
	ADC1->JSQR |= (1<<15);	        //Sets chanel Number(ADC1) 
																	// Result >>
	ADC1->CR2 &= ~ADC_CR2_ALIGN;
	ADC1->CR2 |= ADC_CR2_ADON;                //ADC ON																
}


/*  Take resistant of ADC1
*   Input: 
*   Output: ADC1 resistant on INT
*/
int ADC1_GetResults()
{
	  ADC1->CR2 |= ADC_CR2_JSWSTART;      //Start external ADC1
																	
      while(!(ADC1->SR & ADC_SR_JEOC));      //Wait till the first external complited
																											
      while(!(USART1->SR & USART_SR_TC)){};					//Verification of data transfer is complete	
          return  ADC1->JDR1;      									//12 bit Register = 4096 value
}


/*  Calculate voltage value and convert this to Temperature
*   Return temperature on int
*   Input: ADC_Res - ADC resistance value
*   Output - temperature on int
*/
int Termistor_GetTemperature(int ADC_Res)
{
	double Temp, Volt;
		 Volt = 0.000732421875*ADC_Res;       
	 //   Temp = 1/((1/4300)*log((Volt*6712)/10000)+1/(25+273))-273;   				 //ADC value to Voltage
		Temp = 46.4/Volt;             //36.4 - temperature coefficient	
	return Temp;
}


/*  Return Temperature on char
*   Input: TempSymbNumb - 0,1 temperature symbol number
*   Output: temperature symbol value
*/
char Termistor_GetTemperatureChar(uint8_t TempSymbNumb)
{
	char aTemp[2];
	sprintf(aTemp, "%d" , Termistor_GetTemperature(ADC1_GetResults()));
	
	switch(TempSymbNumb)
	{
		case 0:
			 return aTemp[0];		
		case 1:
			 return aTemp[1];
	}
	return 0;   //Input value not correct
}

