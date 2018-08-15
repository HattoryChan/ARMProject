/* Timer Library
*
*
*
*/

#include "STM32_Timer_Library.h"
#include "stdio.h"    // for ReturnDataOnChar function

volatile unsigned short g_TIM2_ten_microsec_count = 0;
volatile unsigned short g_TIM2_millisec_count = 0;
volatile unsigned short g_TIM2_seconds_count = 0;
volatile unsigned short g_TIM2_minutes_count = 0;
volatile unsigned short g_TIM2_hours_count = 0;
volatile unsigned short g_TIM2_day_count = 0;
volatile unsigned short g_aData_count[4] = {0};  //[day;hour;minute;second]


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
    g_TIM2_ten_microsec_count ++;  // Add microsecond
    
      TIM2_Handler_millisec_counter();  // Add millisec
      TIM2_Handler_second_counter();  // Add second
      TIM2_Handler_minute_counter();  // Add minute
    //  TIM2_Handler_hour_counter();  // Add hour
   //   TIM2_Handler_day_counter(); // Add day
    
    TIM2_Handler_date_update();  // Update Date Array    
    
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
       //Count how many millisec we have
       g_TIM2_millisec_count += g_TIM2_ten_microsec_count / 100; 
       //Reset microsec count, and counted how many microsec we have
       g_TIM2_ten_microsec_count = g_TIM2_ten_microsec_count % 100;       
    }    
}


/*  For TIM2_Handler: second counter
*   
*/
void TIM2_Handler_second_counter()
{
   if(g_TIM2_millisec_count >= 1000) //If we have second
   {
      //Count how many second we have
      g_TIM2_seconds_count += g_TIM2_millisec_count / 1000;
      //Reset millisec count, and counted how many millisec we have
      g_TIM2_millisec_count = g_TIM2_millisec_count % 1000;
   }    
}


/*  For TIM2_Handler: minute counter
*   
*/
void TIM2_Handler_minute_counter()
{
   if(g_TIM2_seconds_count >= 60) // if we have minute
   {
     //Count how many minute we have
     g_TIM2_minutes_count += g_TIM2_millisec_count / 60;
     //Reset second count, and counted how many second we have
     g_TIM2_seconds_count = g_TIM2_seconds_count % 60;     
   }  
}


/*  For TIM2_Handler: hour counter
*   
*/
void TIM2_Handler_hour_counter()
{
   if(g_TIM2_minutes_count >= 60)  //if we have hour
   {
      //Count how many hour we have
      g_TIM2_hours_count += g_TIM2_minutes_count / 60;
      //Reset hours count, and counted how many minute we have
      g_TIM2_minutes_count = g_TIM2_minutes_count % 60;
   }
}

/*  For TIM2_Handler: day counter
*   
*/
void TIM2_Handler_day_counter()
{
   if(g_TIM2_hours_count >= 24) // if we have day
   {
      //Count how many day we have
      g_TIM2_day_count += g_TIM2_hours_count / 60;      
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

/*  Return microsecond
*   
*/
unsigned short Timer_micros()
{
   return g_TIM2_ten_microsec_count;
}

/*  Return millisecond
*   
*/
unsigned short Timer_millis()
{
   return g_TIM2_millisec_count;
}

/*  Return second
*   
*/
unsigned short Timer_second()
{
   return g_TIM2_seconds_count;
}

/*  Return minute
*   
*/
unsigned short Timer_minute()
{
   return g_TIM2_minutes_count;
}

/*  Return hour
*   
*/
unsigned short Timer_hour()
{
   return g_TIM2_hours_count;
}

/*  Return day
*   
*/
unsigned short Timer_day()
{
   return g_TIM2_day_count;
}

/*  Waiting for the specified numbers of microsecond
*   
*/
void Timer_DelayMicros(unsigned short in_Micros)
{
   unsigned short Start_Value = Timer_micros();
   while((Start_Value + in_Micros) > Timer_micros());
}

/*  Waiting for the specified numbers of millissecond
*   
*/
void Timer_DelayMillis(unsigned short in_Millis)
{
   unsigned short Start_Value = Timer_millis();
   while((Start_Value + in_Millis) > Timer_millis());
}

/*  Waiting for the specified numbers of second
*   
*/
void Timer_DelaySecond(unsigned short in_Second)
{
   unsigned short Start_Value = Timer_second();
   while((Start_Value + in_Second) > Timer_second());
}

/*  Waiting for the specified numbers of minute
*   
*/
void Timer_DelayMinute(unsigned short in_Minute)
{
   unsigned short Start_Value = Timer_minute();
   while((Start_Value + in_Minute) > Timer_minute());
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
   TIM2->ARR = 39;       //overflow value, every ten microseconds
   TIM2->DIER |= TIM_DIER_UIE;  //Allow Interrupts
   
   NVIC_EnableIRQ(TIM2_IRQn);    //Add to NVIC table
//   NVIC_SetPriority(TIM2_IRQn,0); //Set FIRST priority for TIM2   
   
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
            return g_aData_count[2] + 48;
         case 1:
            return 48;
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
