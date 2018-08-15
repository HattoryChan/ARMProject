/*  MAIN code file
*
*
*
*/

#include "main.h"

#define WriteToFlashMinTime 12   //Write to flash first hour time
#define WriteToFlashMaxTime 23   //Write to flash second hour time

int g_ValueWaterGNDUsed = 0;    //WaterGND usage counter
char g_CRC[5] = {0};            //CRC global value
char *g_pCRC = &g_CRC[0];     
int g_SendDataTime = 0;         //Last send data to ESP8266 time(minute) 
bool g_FlashWriteFlag = false;  //Write to FLASH flag


/*=======================================================================
                           Main Function
========================================================================*/
int main(void)
{  
   Setup_STM32Periphery(); //Initiall periphery
   
     ESP8266_NullInBuffer();  //Null buffer
  //   while(ESP8266_SearchStringBuffer("00") == 513);   //ESP8266 ready
       GPIOC->ODR ^= GPIO_ODR_ODR8;  // Ready module check
    
     ESP8266_SendMessWithPrefixOne();
     //This place for "Check return CRC" function  
     Timer_DelayMillis(100); //ESP8266 can't get in time
     
     CallTimer_WaterLevel();
     ESP8266_SendMessWithPrefixTwo();  
     //This place for "Check return CRC" function  
     Timer_DelayMillis(100); //ESP8266 can't get in time

     
       GPIOC->ODR ^= GPIO_ODR_ODR8;  //Ready coming to endless loop
    
 while(1)
 {     
   if((g_SendDataTime < Timer_minute()) && (Timer_minute() < 59 ))   //Send data every minute
   {
     g_SendDataTime = Timer_minute();     //Save time(minute) value   
      if(g_SendDataTime == 60)
      {
         g_SendDataTime = 0;        
      }
     
       //Froze indication      
     GPIOC->ODR ^= GPIO_ODR_ODR9;      
      
     ESP8266_SendMessWithPrefixOne();  // Send data      
      Timer_DelayMillis(100);
      
     ESP8266_SendMessWithPrefixTwo();
      Timer_DelayMillis(100);
      
     ESP8266_NullInBuffer();  //Null buffer
     ESP8266_RepealArrayWritePos();  //Repeal write pos
    }
   
    if((Timer_minute() == 59 || Timer_minute() == 0) && g_SendDataTime != 0)
      g_SendDataTime = 0;
        
/*===========================================================================
              Check Relay status Part
===========================================================================*/  
     CallTimer_WaterLevel();
     CallTimer_Light();
     CallTimer_WaterAir();     
    if(CallTimer_WaterGND() == 1)   
      g_ValueWaterGNDUsed++;    
   
    
/*===========================================================================
              Write to FLASH Part
===========================================================================*/
   
 if((Timer_hour() == WriteToFlashMinTime || Timer_hour() == WriteToFlashMaxTime)&& g_FlashWriteFlag == false)
   {
    Timer_SetDataToFlash();  //Set data to Flash array
     //Write to flash part
    FLASH_Unlock();
    FLASH_Clear();
    FLASH_WritePage();   //Write to flash
    FLASH_Lock();
      
    g_FlashWriteFlag = true; //Get up Flag
      GPIOC->ODR ^= GPIO_ODR_ODR8;  //Ready coming to endless loop 
   } 
   else if((Timer_hour() == 13 || Timer_hour() == 0) && g_FlashWriteFlag == true)   
         g_FlashWriteFlag = false; //get down flag
   
      
   Timer_DelaySecond(1); //Delay
   
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
   ClockHSETry();             //Init output high speed generator
   ClockPPLHSETry();          //Init output high speed generator freq multiplier
   Init_Led();                //Init Led
   Init_STM32ToESP8266Uart(); //Init USART PA[9,10]
   FLASH_ReadPage();          //Read data from flash
   Init_STM32F1_Timer();      //Init Timer
   Timer_LoadDataFromFlash(); //Sets time from flash
   Init_STM32F1_ADC();        //Init ADC
   Init_RelayWaterGND();      //Init Relay Water GND
   Init_RelayWaterAir();      //Init Relay Water Air
   Init_RelayLight();         //Init Relay Light
   Init_RelayTankMixing();    //Init Tank Mixing Relay
   ShiftReg_74HC595_Init();   //74HCSR595 initiall
 
}

/*  Initiall LED PC[8,9]
*   
*/
void Init_Led()
{  
//From SMT32F100RB in LQFP64     
/*    Initiall  led
*     PC[8] - output, push-pull.
*     PC[9] - output, push-pull.
*     GPIOC->BSRR = GPIO_BSRR_BS8;	// ON
*     GPIOC->BSRR = GPIO_BSRR_BR8;	// Off 
*/ /*
   RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
   GPIOC->CRH &= ~GPIO_CRH_CNF8;  
   GPIOC->CRH &= ~GPIO_CRH_CNF9;
   GPIOC->CRH |= GPIO_CRH_MODE8_1;
   GPIOC->CRH |= GPIO_CRH_MODE9_1;
   */
   
//From SMT32F100C8 in LQFP48   
/*    Initiall  led
*     PB[8] - output, push-pull.
*     PB[9] - output, push-pull.
*     GPIOB->BSRR = GPIO_BSRR_BS8;	// ON
*     GPIOB->BSRR = GPIO_BSRR_BR8;	// Off 
*/
   RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
   GPIOB->CRH &= ~GPIO_CRH_CNF8;  
   GPIOB->CRH &= ~GPIO_CRH_CNF9;
   GPIOB->CRH |= GPIO_CRH_MODE8_1;
   GPIOB->CRH |= GPIO_CRH_MODE9_1;
}
/*===========================================================================
              ESP8266 Send Data Part
===========================================================================*/ 
/* Create packet with prefix '1' to send ESP8266
* Packet structure: [head][prefix][temperature][day]
*                   [minute][second][CRC]
*/
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
   
    //CRC calculated
    ESP8266_Send(TempFirstSymb);   //First CRC symb
  //  g_CRC[0] = TempFirstSymb;
    ESP8266_Send(DayFirstSymb);    //Second CRC symb
  //  g_CRC[1] = DayFirstSymb;
    ESP8266_Send(HourFirstSymb);   //Third CRC symb
  //  g_CRC[2] = HourFirstSymb;
    ESP8266_Send(MinuteFirstSymb);   //Fourth CRC symb
  //  g_CRC[3] = MinuteFirstSymb;
    ESP8266_Send(Prefix);           //Third CRC symb
  //  g_CRC[4] = Prefix;
    
}

/*  Create packet with prefix '2' to send ESP8266
*   Packet structure: [head][prefix][waterLvL][WateringAllCount]
*                     [lastWatering][WateringCountPerDay][CRC]
*/
void ESP8266_SendMessWithPrefixTwo()
{   
   char WaterLevelFirstSymb = ShiftReg_WaterLevelReturn(0);
   char WaterLevelSecondSymb = ShiftReg_WaterLevelReturn(1);   
   char Prefix = 2 + 48;   
  
      
   ESP8266_SendStr("8266");
   ESP8266_Send(Prefix);
   
   ESP8266_Send(WaterLevelFirstSymb);
   ESP8266_Send(WaterLevelSecondSymb);
   // 8266 2 Wl1Wl2 365 60 50 Wl1 3 6 5 2
   ESP8266_SendStr("3656050");
   
   //CRC
   ESP8266_Send(WaterLevelFirstSymb);  //First CRC symb
   ESP8266_SendStr("3652");            //Next CRC symb
}

/*===========================================================================
                    Worked with sended data part
===========================================================================*/ 

 
/*=======================================================================
                    Work with HTML Part(NOT USED)
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

/*  ESP8266 module activated function
*
*/
void ESP8266_Activated()
{
   //Will be function to activated module
}
