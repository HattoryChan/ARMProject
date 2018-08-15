

#include "main.h"

int g_ValueWaterGNDUsed = 0;
char g_CRC[5] = {0};
char *g_pCRC = &g_CRC[0];

/*    NEEDED!
*  1. Restruct TIM2 and time counter
*  2. Correct timer on oscilloscope

*/
/*extern "C"
{
//-------------------------------------------     EXTI1 Handler PA[1]
void EXTI1_IRQHandler(void)
{	
   HTML_DebugButton();
   EXTI->PR |= EXTI_PR_PR1;		//Clear Interrupts flag																					
}
}*/
int main(void)
{
/*    Initiall debug led
*     PC[8] - output, push-pull.
*    
*     GPIOC->BSRR = GPIO_BSRR_BS8;	// ON
*     GPIOC->BSRR = GPIO_BSRR_BR8;	// Off 
*/
   RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
   GPIOC->CRH &= ~GPIO_CRH_CNF8;  
   GPIOC->CRH &= ~GPIO_CRH_CNF9;
   GPIOC->CRH |= GPIO_CRH_MODE8_1;
   GPIOC->CRH |= GPIO_CRH_MODE9_1;
      
   Setup_STM32Periphery(); //Initiall periphery
   
/*    Debug part
*
*
*/      
 
   
     ESP8266_NullInBuffer();  //Null buffer
     while(ESP8266_SearchStringBuffer("00") == 513);   //ESP8266 ready
     GPIOC->ODR ^= GPIO_ODR_ODR8; 
    // ESP8266_SendStr("8266120265605022651");
      ESP8266_SendMessWithPrefixOne();
     Timer_DelayMillis(100);
   //  if(ESP8266_SearchStringBuffer("22") != 513)
 //    {
     Timer_DelayMillis(100);
       ESP8266_SendStr("8266227365605023652"); 
     Timer_DelayMillis(100);
   //      if(ESP8266_SearchStringBuffer("33") != 513)
    //     {
            GPIOC->ODR ^= GPIO_ODR_ODR8; 
    //     }
   //  }
   while(1)
   {     
              
      //Froze indication
      Timer_DelaySecond(1);
      GPIOC->ODR ^= GPIO_ODR_ODR9; 
      Timer_DelaySecond(1);
      
      //ESP8266_SendMessWithPrefixOne();                 
      //Timer_DelayMicros(100);
      
  //    ESP8266_NullInBuffer();  //Null buffer
/*===========================================================================
              Check Relay status Part
===========================================================================*/  
      CallTimer_Light();
      CallTimer_WaterAir();
      if(CallTimer_WaterGND() == 1)
         g_ValueWaterGNDUsed++;
     
       
      
   }
}


/*=======================================================================
                    Setup STM32 Part   
========================================================================*/ 
/*  Setup STM32Periphery
*   
*/
void Setup_STM32Periphery()
{
   Init_STM32ToESP8266Uart(); //Init USART PA[9,10]
   Init_STM32F1_Timer();      //Init Timer
   Init_STM32F1_ADC();        //Init ADC
   Init_RelayWaterGND();      //Init Relay Water GND
   Init_RelayWaterAir();      //Init Relay Water Air
   Init_RelayLight();         ////Init Relay Light
 //  Setup_EXTI1_Debug();       //EXTI1 PA[1] - initiall
}

/*  EXTI[1] on PA[1] leg
*
*//*
void Setup_EXTI1_Debug()
{
   	//Clocking AFIO(EXTI1)
	RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN);   
												//Port A[0,1] - input with res(CNF[1,0], Mode[0,0])
	GPIOA->CRL &= ~GPIO_CRL_MODE1;
  
  GPIOA->CRL &= ~GPIO_CRL_CNF1;
  GPIOA->CRL |= GPIO_CRL_CNF1_1;			
   
   
																						//Setting EXTI1 for PA[1];
	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI1_PA;
																					//Call it if get HIGH signal on PA1 leg
	EXTI->RTSR |= EXTI_RTSR_TR1;
	//--------------------------------------------------        Enable this in CMSIS
	NVIC_EnableIRQ(EXTI1_IRQn);												//EXTI1
   NVIC_SetPriority(EXTI1_IRQn, 5);
   //Enable interrupts on periphery
	EXTI->IMR |= EXTI_IMR_MR1;	
}
*/


/*===========================================================================
              ESP8266 Send Data Part
===========================================================================*/ 
void ESP8266_SendMessWithPrefixOne()
{   
   char TempFirstSymb = Termistor_GetTemperatureChar(0);
   char TempSecondSymb = Termistor_GetTemperatureChar(1);
   
   char DayFirstSymb = Timer_DayReturn(0);
   char DaySecondSymb = Timer_DayReturn(1);   
   char DayThirdSymb = Timer_DayReturn(2);
   
   char HourFirstSymb = Timer_HourReturn(0);
   char HourSecondSymb = Timer_HourReturn(1);
   
   char MinuteFirstSymb = Timer_MinReturn(0);
   char MinuteSecondSymb = Timer_MinReturn(1);
   
   char Prefix = 1 + 48;
   
    ESP8266_SendStr("8266");
    ESP8266_Send(Prefix);
   
    ESP8266_Send(TempFirstSymb);
    ESP8266_Send(TempSecondSymb);
   
    ESP8266_Send(DayFirstSymb);
    ESP8266_Send(DaySecondSymb);
    ESP8266_Send(DayThirdSymb);
    
    ESP8266_Send(HourFirstSymb);
    ESP8266_Send(HourSecondSymb);
    
    ESP8266_Send(MinuteFirstSymb);
    ESP8266_Send(MinuteSecondSymb);
   
    //CRC
    ESP8266_Send(TempFirstSymb);
    g_CRC[0] = TempFirstSymb;
    ESP8266_Send(DayFirstSymb);
    g_CRC[1] = DayFirstSymb;
    ESP8266_Send(HourFirstSymb);
    g_CRC[2] = HourFirstSymb;
    ESP8266_Send(MinuteFirstSymb);
    g_CRC[3] = MinuteFirstSymb;
    ESP8266_Send(Prefix);
    g_CRC[4] = Prefix;
    
}


/*=======================================================================
                    Work with HTML Part   
========================================================================*/ 
/*  Send HTML page to connecting user
*
*
*/
void HTML_StartPage()
{
    if(ESP8266_InConnectIdentifier() != 513)          //if we have input message
      {   
        if(ESP8266_UnreadFlagStatus() == true)        // and we don't read this message
        {  
           ESP8266_UnreadResetFlag();           
           AT_TCPServerCreateConn(ESP8266_InConnectIdentifier(),"120");  
           
           ESP8266_NullInBuffer();                    
					                                               //Send Start HTML part
																												 //Send Temperature part
           AT_TCPServerSendHTML("<p><b>Temperature:</b> ");
					// AT_TCPServerSendHTMLSymb(Termistor_GetTemperatureChar(0));   //Send First Temperature symbol to ESP8266
				//	 AT_TCPServerSendHTMLSymb(Termistor_GetTemperatureChar(1));   //Send Second Temperature symbol to ESP8266
					 AT_TCPServerSendHTML("&#176;<br /><br />");
																												//Send Water level part
           AT_TCPServerSendHTML("<b>Water level:</b>");
           //AT_TCPServerSendHTML( Return Water level on persent Function);
           AT_TCPServerSendHTMLSymb(g_ValueWaterGNDUsed + 48);
           AT_TCPServerSendHTML(" %<br /><br />");
					                                              //Send Data part
           AT_TCPServerSendHTML("<b>Time:</b>");
           
					 AT_TCPServerSendHTMLSymb(Timer_DayReturn(0));      //Send First Day symbol to ESP8266
					 AT_TCPServerSendHTMLSymb(Timer_DayReturn(1));      //Send Second Day symbol to ESP8266
					 AT_TCPServerSendHTMLSymb(Timer_DayReturn(2));      //Send Third Day symbol to ESP8266
					
					 AT_TCPServerSendHTML(":");					
										
					 AT_TCPServerSendHTMLSymb(Timer_HourReturn(0));      //Send First Hour symbol to ESP8266
					 AT_TCPServerSendHTMLSymb(Timer_HourReturn(1));      //Send Second Hour symbol to ESP8266
					 					 
           AT_TCPServerSendHTML(":");
					 					
					 AT_TCPServerSendHTMLSymb(Timer_MinReturn(0));      //Send First Minute symbol to ESP8266
					 AT_TCPServerSendHTMLSymb(Timer_MinReturn(1));      //Send Second Minute symbol to ESP8266					 					 
           AT_TCPServerSendHTML("<br /><br /></p>");
																												//Send Ended HTML part
           AT_TCPServerSendHTML("                                  ");           
           Timer_DelaySecond(1);
           ESP8266_SendStr("AT+CIPCLOSE=0\r\n");       //ESP8266 to server connection close
        }
     }
}
        
/*  Refresh All AT flag and array
*
*/
void HTML_DebugButton()
{
   ESP8266_NullInBuffer();
   ESP8266_RepealArrayWritePos();
   ESP8266_UnreadResetFlag();
   GPIOC->ODR ^= GPIO_ODR_ODR8; 
   Timer_DelaySecond(1);
}
