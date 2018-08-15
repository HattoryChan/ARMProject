/* Timer Library
*
*
*
*/

#include "STM32_Timer_Library.h"
#include "STM32_Flash.h"
#include "stdio.h"    // for ReturnDataOnChar function


int g_TIM2_ten_microsec_count = 0;
int g_TIM2_millisec_count = 0;
int g_TIM2_seconds_count = 0;
int g_TIM2_minutes_count = 0;
int g_TIM2_hours_count = 0;
int g_TIM2_day_count = 0;
int g_aData_count[4] = {0};  //[day;hour;minute;second]



/*=======================================================================
                           Work with TIM2_Handler parts
========================================================================*/
/*    Handler for TIM2
*     
*/
extern "C"
{
void TIM2_IRQHandler(void)        
 {  
      
    //Need correct TIMER
    g_TIM2_ten_microsec_count++;  // Add ten microsecond
    
    TIM2_Handler_millisec_counter();  // Add millisec   
    
  TIM2->SR &= ~TIM_SR_UIF; // Reset interrupt Flag
 }
} //Extern "C" end


/*  For TIM2_Handler: millisec counter
*   
*/
void TIM2_Handler_millisec_counter()
{
    if(g_TIM2_ten_microsec_count >= 100) //If we have millisecond
    {       
       g_TIM2_ten_microsec_count = 0;
       g_TIM2_millisec_count ++;
       TIM2_Handler_second_counter();  // Add second
    }    
}


/*  For TIM2_Handler: second counter
*   
*/
void TIM2_Handler_second_counter()
{
   if(g_TIM2_millisec_count >= 1000) //If we have second
   {              
      g_TIM2_millisec_count = 0;
      g_TIM2_seconds_count ++;  
      TIM2_Handler_minute_counter();  // Add minute
   }  
   TIM2_Handler_date_update();  // Update Date Array    
}


/*  For TIM2_Handler: minute counter
*   
*/
void TIM2_Handler_minute_counter()
{
   if(g_TIM2_seconds_count >= 60) // if we have minute
   {      
      g_TIM2_seconds_count = 0;
      g_TIM2_minutes_count ++;
      TIM2_Handler_hour_counter();  // Add hour
   }  
}


/*  For TIM2_Handler: hour counter
*   
*/
void TIM2_Handler_hour_counter()
{
   if(g_TIM2_minutes_count >= 60)  //if we have hour
   {      
      g_TIM2_minutes_count = 0;
      g_TIM2_hours_count ++;
      TIM2_Handler_day_counter(); // Add day
   }
}

/*  For TIM2_Handler: day counter
*   
*/
void TIM2_Handler_day_counter()
{
   if(g_TIM2_hours_count >= 24) // if we have day
   {          
      g_TIM2_hours_count = 0;      
      g_TIM2_day_count ++; 
      
	 if(g_TIM2_day_count == 999)    //999 - max day counter
	 {
		g_TIM2_day_count = 0;
	 }
   }
}


/*=======================================================================
                    Part for work with Time delay      
========================================================================*/   
/*  For TIM2_Handler: Add date to Date_Array
*   g_aData_count[4] = {day; hour; minute; second};
*/
void TIM2_Handler_date_update()
{  
  // Add value to Array
  g_aData_count[0] = g_TIM2_day_count;
  g_aData_count[1] = g_TIM2_hours_count;
  g_aData_count[2] = g_TIM2_minutes_count;
  g_aData_count[3] = g_TIM2_seconds_count;   
}

/*   Load date from flash and set him to g_TIM2_time_count
*
*/
void Timer_LoadDataFromFlash()
{
   g_TIM2_day_count = FLASH_ArrayGetValue(0)*100 + FLASH_ArrayGetValue(1) * 10 + FLASH_ArrayGetValue(2);   //Day
   g_TIM2_hours_count = FLASH_ArrayGetValue(3)*10 + FLASH_ArrayGetValue(4);     //hour
   g_TIM2_minutes_count = FLASH_ArrayGetValue(5)*10 + FLASH_ArrayGetValue(6);    //Minute
}

/*   Add time to FlashArray
*
*/
void Timer_SetDataToFlash()
{
   //Day
  FLASH_ArraySetValue(0, g_aData_count[0]/100);
  FLASH_ArraySetValue(1, (g_aData_count[0]/10)%10);
  FLASH_ArraySetValue(2, g_aData_count[0]%10);
  //Minute
  FLASH_ArraySetValue(3, g_aData_count[1]/10);
  FLASH_ArraySetValue(4, g_aData_count[1]%10);
  //Hour
  FLASH_ArraySetValue(5, g_aData_count[2]/10);
  FLASH_ArraySetValue(6, g_aData_count[2]%10);
   
}

/*  Return microsecond
*   
*/
int Timer_micros()
{
   return g_TIM2_ten_microsec_count;
}

/*  Return millisecond
*   
*/
int Timer_millis()
{
   return g_TIM2_millisec_count;
}

/*  Return second
*   
*/
int Timer_second()
{
   return g_TIM2_seconds_count;
}

/*  Return minute
*   
*/
int Timer_minute()
{
   return g_TIM2_minutes_count;
}

/*  Return hour
*   
*/
int Timer_hour()
{
   return g_TIM2_hours_count;
}

/*  Return day
*   
*/
int Timer_day()
{
   return g_TIM2_day_count;
}

/*  Waiting for the specified numbers of microsecond
*   
*/
void Timer_DelayMicros(int in_Micros)
{
   int Start_Value = Timer_micros();
   if((Start_Value + in_Micros) >= 999)      
    {  
      while(in_Micros > g_TIM2_ten_microsec_count);
    } 
   else
    {   
   while((Start_Value + in_Micros) > Timer_micros());
    }
}

/*  Waiting for the specified numbers of millissecond
*   
*/
void Timer_DelayMillis(int in_Millis)
{
   int Start_Value = Timer_millis(); 
   if((Start_Value + in_Millis) >= 999)			
    {  
      while(in_Millis > g_TIM2_millisec_count);
    } 
   else
    {     
   while((Start_Value + in_Millis) > Timer_millis());
    }
}

/*  Waiting for the specified numbers of second
*   
*/
void Timer_DelaySecond(int in_Second)
{
   int Start_Value = Timer_second();
   if((Start_Value + in_Second) >= 59) 
    {  
      while(in_Second > g_TIM2_seconds_count);
    } 
   else
    {   
   while((Start_Value + in_Second) > g_TIM2_seconds_count);
    }
}

/*  Waiting for the specified numbers of minute
*   
*/
void Timer_DelayMinute(int in_Minute)
{
   int Start_Value = Timer_minute();
   if((Start_Value + in_Minute) >= 59)		
    {  
      while(in_Minute > g_TIM2_minutes_count);
    } 
   else
    {      
   while((Start_Value + in_Minute) >  g_TIM2_minutes_count);
    }
}
/*=======================================================================
                    Initiall TIM2 parts      
========================================================================*/   
/*    Initiall Timer
*     TIM2 and TIM2_Handler initiall
*     Timer will be counted time, and be a clock
*/
void Init_STM32F1_Timer()
{
   RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; //Clocking TIM2
   
   TIM2->PSC = 2; //Prescale, counted how "value"+1
   TIM2->ARR = 79;       //overflow value, every ten microseconds
   TIM2->DIER |= TIM_DIER_UIE;  //Allow Interrupts
   
   NVIC_EnableIRQ(TIM2_IRQn);    //Add to NVIC table
   NVIC_SetPriority(TIM2_IRQn,0); //Set FIRST priority for TIM2   
   
   TIM2->CR1 |= TIM_CR1_CEN; //Start TIM2
}


/*=======================================================================
                    Data return Part
========================================================================*/ 
/*  Return Day on char
*   Input: Chose - 0,1,2 day symbol number
*/
char Timer_DayReturn(uint8_t DaySymbNumb)
{  
   if(g_aData_count[0] >= 100)
   {      
      char aDay[3];
      sprintf(aDay, "%d", g_aData_count[0]);
   
      switch(DaySymbNumb)
      {
         case 0:
            return aDay[0];
         case 1:
            return aDay[1];
         case 2: 
            return aDay[2];
      }
   }
   else if(g_aData_count[0] >= 10)
   {
      char aDay[2];
      sprintf(aDay, "%d", g_aData_count[0]);
   
      switch(DaySymbNumb)
      {
         case 0:
            return 48;
         case 1:
            return aDay[0];
         case 2:             
            return aDay[1];
      }
   }
   else
   {   
      switch(DaySymbNumb)
      {
         case 0:
            return 48;
         case 1:
            return 48;
         case 2:
            return g_aData_count[0] + 48;            
      }
   }
	 return 0;   //Input value not correct
}


/*  Return Hour on char
*   Input: HourSymbNumb - 0,1 day symbol number
*/
char Timer_HourReturn(uint8_t HourSymbNumb)
{   
   if(g_aData_count[1] >= 10)
   {
      char aHour[2];
      sprintf(aHour, "%d", g_aData_count[1]);
      switch(HourSymbNumb)
      {
         case 0:
            return aHour[0];
         case 1:
            return aHour[1];
      }
   }
   else 
   {
      switch(HourSymbNumb)
      {
         case 0:
            return 48;
         case 1:
            return g_aData_count[1] + 48;
      }
   }
	 return 0;   //Input value not correct
}


/*  Return Minute on char
*   Input: MinSymbNumb - 0,1 day symbol number
*/
char Timer_MinReturn(uint8_t MinSymbNumb)
{   
   if(g_aData_count[2] >= 10)
   {
      char aMin[2];
      sprintf(aMin, "%d", g_aData_count[2]);
      switch(MinSymbNumb)
      {
         case 0:
            return aMin[0];
         case 1:
            return aMin[1];
      }
   }
   else 
   {
      switch(MinSymbNumb)
      {
         case 0:
            return 48;
         case 1:
            return g_aData_count[2] + 48;
      }
   }
	 return 0;   //Input value not correct
}


/*  Return Seconds on char
*   Input: SecSymbNumb - 0,1 day symbol number
*/
char Timer_SecReturn(uint8_t SecSymbNumb)
{   
   char aSec[2];
   sprintf(aSec, "%d", g_aData_count[3]);
   switch(SecSymbNumb)
	 {
		 case 0:
        return aSec[0];
		 case 1:
			  return aSec[1];
	 }
	 return 0;   //Input value not correct
}
