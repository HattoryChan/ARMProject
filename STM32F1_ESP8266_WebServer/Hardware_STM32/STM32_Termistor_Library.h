#include "stm32f10x.h"
#include <math.h>         // Mathematical action on program 

#define d_SecResInDivValue 10000.0  
#define d_ADCVrefValue 3.30
#define d_ADCSamplNumb 4096.0
#define d_CommonDivideRes25 5000.0

#define d_TermCoef_B 4300.0
#define d_KelvinZeroT 273.15
#define d_Kelvin25T 298.15


/*=======================================================================
                    Initiall ADC1 parts      
========================================================================*/ 
void Init_STM32F1_ADC(); //Initiall ADC1


/*=======================================================================
                    Work with ADC1 taking value     
========================================================================*/ 
int ADC1_GetResults(); //Take resistant of ADC1
int Termistor_GetTemperature(int ADC_Res); //Calculate voltage value and convert this to Temperature

/*=======================================================================
                    Termistor value return part      
========================================================================*/ 
char Termistor_GetTemperatureChar(uint8_t TempSymbNumb); //Return Temperature on char
