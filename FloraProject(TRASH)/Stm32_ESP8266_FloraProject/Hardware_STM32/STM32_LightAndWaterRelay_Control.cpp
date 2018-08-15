/*
*   Using LEG:
*   Port B:
*     [0] - WaterGND
*     [1] - WaterAir
*     [2] - Light
*     [3] - TankFilling
*     [4] - TankHeating
*/


#include "STM32_LightAndWaterRelay_Control.h"
#include "STM32_Timer_Library.h"

bool g_RelayLightFlag = false;  //Relay setting flag
bool g_IncludedFlagWaterGnd = false; //Water GND relay flag
bool g_IncludedFlagLight = false;

/*=======================================================================
                          Call timer relay part
========================================================================*/
/*   Water-GND relay call and control function
*
*    Output: '1' - It's OK, '0' -it's not time yet
*/
bool CallTimer_WaterGND()
{   
  
	if(Timer_minute() == 0 || Timer_minute() == 15 || Timer_minute() == 30 || Timer_minute() == 45) 
	{
      if(g_IncludedFlagWaterGnd == false) //WaterGND need on and we dont have included on this hour
      {
         int Time = Timer_second();  //Remember time
         int RelayTime = 15;    //15 second delay
		
         if((Time + RelayTime) > 60)   //if we go out of range
               Time = 0;           			//we set start time == 0
		
         Relay_WaterGND(1);      //On Water GND relay
	
         while((Time + RelayTime) > Timer_second())  //Wait delay time
         {
         }
         //   break;     //go out of cycle	
		
         Relay_WaterGND(0);    //Off Water GND relay
         
         g_IncludedFlagWaterGnd = true;         
         
            return 1;   //All OK, we have waterring
      }      
   }
   if(Timer_minute() == 1 || Timer_minute() == 16 || Timer_minute() == 31 || Timer_minute() == 46) 
      g_IncludedFlagWaterGnd = false; //we need included
     
            return 0;   // it's not time yet
}


/*  Water Air relay call and control function
*
*   Output: '0' - it's not time yet, '1' - It's OK
*/
bool CallTimer_WaterAir()
{
   bool IncludedFlag;
   
   if(Timer_hour() == 12)   //one time per day
   {
    if(IncludedFlag == 0)
    {
      int Time = Timer_second(); //Remember time
      int RelayTime = 7;  //7 second delay
      
      if((Time + RelayTime) > 60)   //If we go out of range
         Time = 0;                  //we set start time = 0;
     
      Relay_WaterAir(1);   //On Water Air relay
      
      while((Time + RelayTime) > Timer_second()); //Wait delay time
       //  break;   //go out of cycle
      
      Relay_WaterAir(0);   //Off water air relay
      IncludedFlag = true;      
      return 1;  //All Ok, we have waterring
    } 
   }
   if(Timer_hour() == 13) 
      IncludedFlag = false; // we need included
   
   return 0; //It's not time yet
}


/*  Light relay call and control function
*
*   Output: '0' - it's not time yet, '1' - It's OK
*/
bool CallTimer_Light()
{
   
   
   if(Timer_hour() == 12 || Timer_hour() == 0)
   {
      if(g_RelayLightFlag == 1 && g_IncludedFlagLight == 0)   //Light ON and we dont have included on this hour
      {
         Relay_Light(0);  //Light OFF
         g_IncludedFlagLight = true; //We have included on this hour
         return 1;      //It's OK
      }
      else if(g_RelayLightFlag == 0 || g_IncludedFlagLight == 0)   //Light OFF and we dont have included on this hour
      {
         Relay_Light(1); //Light ON
         g_IncludedFlagLight = true; //We have included on this hour
         return 1; //It's OK
      }
   }   
   if(Timer_hour() == 1 || Timer_hour() == 13)  //we need included
      g_IncludedFlagLight = false;
   
   return 0; //It's not time yet
}

/*=======================================================================
                          Relay Initiall part
========================================================================*/
/*  Initialization Relay leg for 
*     Watering of roots
*     Port B[0]
*/
void Init_RelayWaterGND()
{
   RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; //clocking for Port B
   GPIOB->CRL &= ~GPIO_CRL_CNF0;     //output, push-pull
   GPIOB->CRL |= GPIO_CRL_MODE0_1;   // Port B[0]   
}


/*  Initialization Relay leg for 
*    Watering of leaves
*    Port B[1]
*/
void Init_RelayWaterAir()
{
     RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; //clocking for Port B
   GPIOB->CRL &= ~GPIO_CRL_CNF1;     //output, push-pull
   GPIOB->CRL |= GPIO_CRL_MODE1_1;   // Port B[1]   
}


/*  Initialization Relay leg for 
*    tank filling(adding water to
*                 the tank)
*    Port B[3]
*/
void Init_RelayTankFilling()
{
     RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; //clocking for Port B
   GPIOB->CRL &= ~GPIO_CRL_CNF3;     //output, push-pull
   GPIOB->CRL |= GPIO_CRL_MODE3_1;   // Port B[3]   
}


/*  Initialization Relay leg for 
*     Light 
*    Port B[2]
*/
void Init_RelayLight()
{
   RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; //clocking for Port B
   GPIOB->CRL &= ~GPIO_CRL_CNF2;     //output, push-pull
   GPIOB->CRL |= GPIO_CRL_MODE2_1;   // Port B[2]   
}


/*  Initialization Relay leg for 
*     Tank Heating
*    Port B[4]
*/
void Init_RelayTankHeating()
{
   RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; //clocking for Port B
   GPIOB->CRL &= ~GPIO_CRL_CNF4;     //output, push-pull
   GPIOB->CRL |= GPIO_CRL_MODE4_1;   // Port B[4]   
}


/*=======================================================================
                           Work with Relay part
========================================================================*/
/*  Set logical value to Relay WaterGND
*     Port B[0]
*   Output: '0' - OFF, '1' - ON
*/
void Relay_WaterGND(bool OnOff)
{
   if(OnOff)  //if true
   {
      GPIOB->BSRR = GPIO_BSRR_BS0;  //ON
      
   } 
   else      //if false
   {
      GPIOB->BSRR = GPIO_BSRR_BR0; // OFF
     
   }
}


/*  Set logical value to Relay WaterAir
*     Port B[1]
*   Output: '0' - OFF, '1' - ON
*/
void Relay_WaterAir(bool OnOff)
{
   if(OnOff)  //if true
   {
      GPIOB->BSRR = GPIO_BSRR_BS1;  //ON
     
   } 
   else      //if false
   {
      GPIOB->BSRR = GPIO_BSRR_BR1; // OFF
      
   }
}


/*  Set logical value to Relay TankFilling
*     Port B[2]
*   Output: '0' - OFF, '1' - ON
*/
void Relay_TankFilling(bool OnOff)
{
   if(OnOff)  //if true
   {
      GPIOB->BSRR = GPIO_BSRR_BS3;  //ON
      
   } 
   else      //if false
   {
      GPIOB->BSRR = GPIO_BSRR_BR3; // OFF
      
   }
}


/*  Set logical value to Relay Light
*     Port B[3]
*   Output: '0' - OFF, '1' - ON
*/
void Relay_Light(bool OnOff)
{
   if(OnOff)  //if true
   {
      GPIOB->BSRR = GPIO_BSRR_BS2;  //ON
      g_RelayLightFlag = 1;
   } 
   else      //if false
   {
      GPIOB->BSRR = GPIO_BSRR_BR2; // OFF
      g_RelayLightFlag = 0;
   }
}


/*  Set logical value to Relay TankHeating
*     Port B[4]
*   Output: '0' - OFF, '1' - ON
*/
void Relay_TankHeating(bool OnOff)
{
   if(OnOff)  //if true
   {
      GPIOB->BSRR = GPIO_BSRR_BS4;  //ON
     
   } 
   else      //if false
   {
      GPIOB->BSRR = GPIO_BSRR_BR4; // OFF
      
   }
}
