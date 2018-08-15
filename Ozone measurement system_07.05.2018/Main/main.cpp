#include "main.h"

int main()
{
   GPIO_Init();
   PWM_TIM4_Init();
   PWM_Control(1);
   PWM_Setting(1,60000);
   PWM_Setting(2,1000);
   
   while(1)
   {
      
   }
}
