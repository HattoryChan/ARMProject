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
unsigned short Timer_micros(); //Return microsecond
unsigned short Timer_millis(); //Return millisecond
unsigned short Timer_second(); //Return second
unsigned short Timer_minute(); //Return minute
unsigned short Timer_hour(); //Return hour 
unsigned short Timer_day(); //Return day

void Timer_DelayMicros(unsigned short in_Micros); //Waiting for the specified numbers of microsecond
void Timer_DelayMillis(unsigned short in_Millis); //Waiting for the specified numbers of millissecond
void Timer_DelaySecond(unsigned short in_Second); //Waiting for the specified numbers of second
void Timer_DelayMinute(unsigned short in_Minute); //Waiting for the specified numbers of minute

/*=======================================================================
                    Data return Part
========================================================================*/ 
char Timer_DayReturn(uint8_t DaySymbNumb); //Return Day on char
char Timer_HourReturn(uint8_t HourSymbNumb); //Return Hour on char
char Timer_MinReturn(uint8_t MinSymbNumb); //Return Minute on char
char Timer_SecReturn(uint8_t SecSymbNumb); //Return Second on char
