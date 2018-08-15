/*=============================================================================
*  This program will be control step motor after get comand-message on Uart
*
 =============================================================================*/

#include "main.h"

int main()
{
   StepMotor_Init();
   
   while(1)
   {      
      StepMotor_StepSecondForward(10);     
       delay(1);
      
      StepMotor_StepFirstForward(50);     
       delay(1);
      
      StepMotor_StepSecondBack(10);
       delay(1);
      
      StepMotor_StepFirstBack(50);
       delay(1);
      
   }
}
