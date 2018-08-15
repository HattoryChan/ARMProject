/*  
*   Using LEG:
*   Port B:
*     [0] - WaterGND
*     [1] - WaterAir
*     [2] - Light
*     [3] - WateringMotor
*     [4] - TankHeating
*/

/*
*
При включении мотора на waterGND или tankHeating выбираем положение клапана и включаем мотор(WateringMotor)
*
*/ 

#include "STM32_LightAndWaterRelay_Control.h"
#include "STM32_Timer_Library.h"
#include "stdio.h"    // for ReturnDataOnChar function

#define WaterGNDRelayTime 7
#define WaterAirRelayTime 15
#define TankMixingRelayTime 50
#define LightDayLenght 12

bool g_RelayLightFlag = false;  //Relay setting flag
bool g_RelayWaterGndFlag = false; //Water GND relay flag
bool g_RelayWaterAirFlag = false; //Water Air Relay flag
bool g_RelayTankMixing = false; //Tank Heating relay flag

int g_aLastWateringTime[2] = {0,0}; //[hour][minute]
int g_aLastTankFeelingTime[2] = {0,0}; //[hour][minute]
int g_aLastWaterAirTime[2] = {0,0}; //[hour][minute]
   

/*=======================================================================
                          Call timer relay part
========================================================================*/
/*   Water-GND relay call and control function
*
*    Output: '1' - It's OK, '0' -it's not time yet
*/
bool CallTimer_WaterGND()
{   
  
	if(Timer_minute() == 1 || Timer_minute() == 15 || Timer_minute() == 30 || Timer_minute() == 45) 
	{
      if(g_RelayWaterGndFlag == false) //WaterGND need on and we dont have included on this time part
      {         
         Relay_WaterGND(1);      //On Water GND valve
         Relay_WateringMotor(1);  //On WateringMotor
	
        Timer_DelaySecond(WaterGNDRelayTime);
		
         Relay_WaterGND(0);    //Off Water GND relay
         Relay_WateringMotor(0);  //Off WateringMotor
         
         g_RelayWaterGndFlag = true;         
         
         Watering_SetDate();
         
            return 1;   //All OK, we have waterring
      }      
   }
   if((Timer_minute() == 2 || Timer_minute() == 16 || Timer_minute() == 31 || Timer_minute() == 46)&&  g_RelayWaterGndFlag == true) 
   {      
        g_RelayWaterGndFlag = false; //we need included in next time part     
   }      
            return 0;   // it's not time yet
}


/*  Water Air relay call and control function
*
*   Output: '0' - it's not time yet, '1' - It's OK
*/
bool CallTimer_WaterAir()
{
   
   
   if(Timer_hour() == 12 && g_RelayWaterAirFlag == 0)   //one time per day
   {
    if(g_RelayWaterAirFlag == false)
    {
      Relay_WaterAir(1);   //On Water Air relay
      
     Timer_DelaySecond(WaterAirRelayTime);
      
      Relay_WaterAir(0);   //Off water air relay
      
      g_RelayWaterAirFlag = true;   

      WaterAir_SetDate();
       
      return 1;  //All Ok, we have waterring
    }
    
   }
   if(Timer_hour() == 13 && g_RelayWaterAirFlag == true) 
   {
      g_RelayWaterAirFlag = false; // we need included
   }
   return 0; //It's not time yet
}


/*  Light relay call and control function
*
*   Output: '0' - it's not time yet, '1' - It's OK
*/
bool CallTimer_Light()
{  
   if(Timer_hour() < LightDayLenght && g_RelayLightFlag == 0)  
   {
         Relay_Light(1);  //Light ON
         g_RelayLightFlag = true; //light ON flag
         return 1;      //It's OK
   }
   else if(Timer_hour() >= LightDayLenght && g_RelayLightFlag == 1)  
        {
            Relay_Light(0); //Light OFF
            g_RelayLightFlag = false; //Loght flag OFF
            return 1; //It's OK
        }
        
   return 0; //It's not time yet
}

/*  Tank Heating call and control function
*
*   Output: '0' - it's not time yet, '1' - It's OK
*/
bool CallTimer_TankMixing()
{
   if(Timer_minute() == 50)
   {
      if(g_RelayTankMixing == false) //We don't use TankMixing on this time part
      {
         Relay_TankMixing(1); //On TankMixing valve
         Relay_WateringMotor(1); //On Watering motor relay
         
         Timer_DelaySecond(TankMixingRelayTime);
         
         Relay_TankMixing(0); //Off TankMixing valve
         Relay_WateringMotor(0); //Off Watering motor relay
         
         g_RelayTankMixing = true;
         
         return 1; //All OK, we have heating
      }      
   }
   if(Timer_minute() == 51 && g_RelayTankMixing == true)
   {
      g_RelayTankMixing = false; //we need included in next part time
   }
      return 0; //It's not time yet
}

/*=======================================================================
                          Work with return data part
========================================================================*/
/*  LightDayLenght return function
*
*   Input: DayLenghtSymbNumb - 0,1 day symbol number
*/
char Light_LenghtReturn(uint8_t DayLenghtSymbNumb)
{
  if(LightDayLenght >= 10)
  {     
      char aHour[2];
      sprintf(aHour, "%d", LightDayLenght);
      switch(DayLenghtSymbNumb)
      {
         case 0:
            return aHour[0];
         case 1:
            return aHour[1];
      }
  }
  else
  {
     switch(DayLenghtSymbNumb)
     {
        case 0: 
           return 48;
        case 1:
           return LightDayLenght + 48;
     }
  }
       return 0;   //Input value not correct
}


/*  WaterGNDLenght return function
*
*   Input: SecondSymbNumb - 0,1 day symbol number
*/
char WaterGND_LenghtReturn(uint8_t SecondSymbNumb)
{
  if(WaterGNDRelayTime >= 10)
  {     
      char aSecond[2];
      sprintf(aSecond, "%d", WaterGNDRelayTime);
      switch(SecondSymbNumb)
      {
         case 0:
            return aSecond[0];
         case 1:
            return aSecond[1];
      }
  }
  else
  {
     switch(SecondSymbNumb)
     {
        case 0: 
           return 48;
        case 1:
           return WaterGNDRelayTime + 48;
     }
  }
       return 0;   //Input value not correct
}

/*  Set date on g_aLastWateringTime[2]
*
*
*/
void Watering_SetDate()
{
   g_aLastWateringTime[0] = Timer_hour();
   g_aLastWateringTime[1] = Timer_minute();
}

/*  Return Last Watering hour on char
*   Input: HourSymbNumb - 0,1 day symbol number
*/
char Watering_LastHourReturn(uint8_t HourSymbNumb)
{   
   if(g_aLastWateringTime[0] >= 10)
   {
      char aHour[2];
      sprintf(aHour, "%d", g_aLastWateringTime[1]);
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
            return g_aLastWateringTime[0] + 48;
      }
   }
	 return 0;   //Input value not correct
}

/*  Return Last Watering minute on char
*   Input: MinSymbNumb - 0,1 day symbol number
*/
char Watering_LastMinReturn(uint8_t MinSymbNumb)
{   
   if(g_aLastWateringTime[1] >= 10)
   {
      char aMin[2];
      sprintf(aMin, "%d", g_aLastWateringTime[1]);
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
            return g_aLastWateringTime[1] + 48;
      }
   }
	 return 0;   //Input value not correct
}

/*  Set date on g_aLastTankFeelingTime[2]
*
*
*/
void TankFeeling_SetDate()
{
   g_aLastTankFeelingTime[0] = Timer_hour();
   g_aLastTankFeelingTime[1] = Timer_minute();
}

/*  Return Last TankFeeling hour on char
*   Input: HourSymbNumb - 0,1 day symbol number
*/
char TankFeeling_LastHourReturn(uint8_t HourSymbNumb)
{   
   if(g_aLastTankFeelingTime[0] >= 10)
   {
      char aHour[2];
      sprintf(aHour, "%d", g_aLastTankFeelingTime[1]);
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
            return g_aLastTankFeelingTime[0] + 48;
      }
   }
	 return 0;   //Input value not correct
}

/*  Return Last TankFeeling minute on char
*   Input: MinSymbNumb - 0,1 day symbol number
*/
char TankFeeling_LastMinReturn(uint8_t MinSymbNumb)
{   
   if(g_aLastTankFeelingTime[1] >= 10)
   {
      char aMin[2];
      sprintf(aMin, "%d", g_aLastTankFeelingTime[1]);
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
            return g_aLastTankFeelingTime[1] + 48;
      }
   }
	 return 0;   //Input value not correct
}

/*  Set date on g_aLastWaterAirTime[2]
*
*
*/
void WaterAir_SetDate()
{
   g_aLastWaterAirTime[0] = Timer_hour();
   g_aLastWaterAirTime[1] = Timer_minute();
}

/*  Return Last WaterAir hour on char
*   Input: HourSymbNumb - 0,1 day symbol number
*/
char WaterAir_LastHourReturn(uint8_t HourSymbNumb)
{   
   if(g_aLastWaterAirTime[0] >= 10)
   {
      char aHour[2];
      sprintf(aHour, "%d", g_aLastWaterAirTime[1]);
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
            return g_aLastWaterAirTime[0] + 48;
      }
   }
	 return 0;   //Input value not correct
}

/*  Return Last WaterAir minute on char
*   Input: MinSymbNumb - 0,1 day symbol number
*/
char WaterAir_LastMinReturn(uint8_t MinSymbNumb)
{   
   if(g_aLastWaterAirTime[1] >= 10)
   {
      char aMin[2];
      sprintf(aMin, "%d", g_aLastWaterAirTime[1]);
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
            return g_aLastWaterAirTime[1] + 48;
      }
   }
	 return 0;   //Input value not correct
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
void Init_RelayWateringMotor()
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
void Init_RelayTankMixing()
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
void Relay_WateringMotor(bool OnOff)
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
void Relay_TankMixing(bool OnOff)
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
