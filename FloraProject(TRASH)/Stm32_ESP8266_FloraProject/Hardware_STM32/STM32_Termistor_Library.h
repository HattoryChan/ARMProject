




#include "stm32f10x.h"

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
