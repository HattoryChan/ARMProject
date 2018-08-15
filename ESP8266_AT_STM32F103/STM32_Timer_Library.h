/* Header for Timer Library
*
*
*
*/

#include "stm32f10x.h"

/*=======================================================================
                    Initiall TIM2 parts      
========================================================================*/   
void Init_STM32F1_Timer(); //Initiall TIM2


/*=======================================================================
                           Work with TIM2_Handler parts
========================================================================*/
void TIM2_Handler_millisec_counter(); //millisec counter
void TIM2_Handler_second_counter(); //second counter
void TIM2_Handler_minute_counter(); //minute counter
void TIM2_Handler_hour_counter(); //hour counter
void TIM2_Handler_day_counter(); //day counter

/*=======================================================================
                    Part for work with Time delay      
========================================================================*/   
void TIM2_Handler_date_update(); //Date array update
void Timer_LoadDataFromFlash();  //Load Date from flash
void Timer_SetDataToFlash();   //Set data to flash array
int Timer_micros(); //Return microsecond
int Timer_millis(); //Return millisecond
int Timer_second(); //Return second
int Timer_minute(); //Return minute
int Timer_hour(); //Return hour 
int Timer_day(); //Return day


void Timer_DelayMicros(int in_Micros); //Waiting for the specified numbers of microsecond
void Timer_DelayMillis(int in_Millis); //Waiting for the specified numbers of millissecond
void Timer_DelaySecond(int in_Second); //Waiting for the specified numbers of second
void Timer_DelayMinute(int in_Minute); //Waiting for the specified numbers of minute

/*=======================================================================
                    Data return Part
========================================================================*/ 
char Timer_DayReturn(uint8_t DaySymbNumb); //Return Day on char
char Timer_HourReturn(uint8_t HourSymbNumb); //Return Hour on char
char Timer_MinReturn(uint8_t MinSymbNumb); //Return Minute on char
char Timer_SecReturn(uint8_t SecSymbNumb); //Return Second on char
