





#include "STM32_Timer_Library.h"
#include "ESP8266_AT.h"
#include "STM32_Termistor_Library.h"
#include "STM32_LightAndWaterRelay_Control.h"


/*=======================================================================
                    Setup STM32 Part   
========================================================================*/ 
void Setup_STM32Periphery();  //Setup STM32Periphery  
void Setup_EXTI1_Debug();   //EXTI1 Initiall(PA[1])

/*=======================================================================
                    Work with HTML Part   
========================================================================*/ 
void HTML_StartPage(); //Send HTML page to connecting user
void HTML_DebugButton(); //Refresh All AT flag and array

/*===========================================================================
              ESP8266 Send Data Part
===========================================================================*/ 
void ESP8266_SendMessWithPrefixOne();
