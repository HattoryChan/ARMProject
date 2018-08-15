/* Header for main.cpp
*
*
*
*
*
*/


#include "STM32_Timer_Library.h"
#include "ESP8266_AT.h"
#include "STM32_Termistor_Library.h"
#include "STM32_LightAndWaterRelay_Control.h"
#include "STM32_Flash.h"
#include "74HC595_InitAndWork.h"

/*=======================================================================
                    Setup STM32 Part   
========================================================================*/ 
void Setup_STM32Periphery();  //Setup STM32Periphery  
void Init_Led();  //Initiall LED PC[8,9]

/*=======================================================================
                    Work with HTML Part   
========================================================================*/ 
void HTML_StartPage(); //Send HTML page to connecting user
void HTML_DebugButton(); //Refresh All AT flag and array

/*===========================================================================
              ESP8266 Send Data Part
===========================================================================*/ 
void ESP8266_SendMessWithPrefixOne(); //Create packet with prefix '1' to send ESP8266
void ESP8266_SendMessWithPrefixTwo(); //Create packet with prefix '2' to send ESP8266
