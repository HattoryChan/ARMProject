/*  74HC595_InitAndWork(Water level check and inner/outer indication)
*
*
*
*/

// SH_CP PB[5] - Leg for clocking impulse
// DS    PB[6] - Output data leg 
// ST_CP  PB[7] - Fixing data register
// WaterLevelInput PB[12] - ltg for check water lvl

#define SH_CP_Low   GPIOB->BSRR = GPIO_BSRR_BR5
#define DS_Low      GPIOB->BSRR = GPIO_BSRR_BR6
#define ST_CP_Low   GPIOB->BSRR = GPIO_BSRR_BR7
#define SH_CP_High   GPIOB->BSRR = GPIO_BSRR_BS5
#define DS_High      GPIOB->BSRR = GPIO_BSRR_BS6
#define ST_CP_High   GPIOB->BSRR = GPIO_BSRR_BS7
#define WaterLevelInput GPIOB->IDR & GPIO_IDR_IDR12

#include "stm32f10x.h"
#include "STM32_Timer_Library.h"

void ShiftReg_NullWaterLevelInd();

bool g_WaterLevelFlag = false; 
int g_WaterLevelValue = 0;

uint8_t g_aShiftArray[16] = {0};   //[0-5] - Led indication, [6-16] - water level



/*=======================================================================
                    Initial and work with 74HC595 Part
========================================================================*/ 
/*      74HC595 Init
*
*/
void ShiftReg_74HC595_Init()
{
//		-------------------------------------- GPIO[B]
																										
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;												//Clocking for Port[B]
																														//PB[5,6,7] - output, push-pull, 10Mhz
	GPIOB->CRL |= GPIO_CRL_MODE5_1 | GPIO_CRL_MODE6_1 | GPIO_CRL_MODE7_1;
	GPIOB->CRL &= ~(GPIO_CRL_CNF5 | GPIO_CRL_CNF6 | GPIO_CRL_CNF7);		
   
   GPIOB->CRH &= ~GPIO_CRH_MODE12;   //PB[12] - input, push-pull.
   GPIOB->CRH &= ~GPIO_CRH_CNF12;
   GPIOB->CRH |= GPIO_CRH_CNF12_1;
}


/*   Work with 74HC595 function
*
*/
void ShiftReg_SetData(int prior)
{																									
																		//Work mode
   SH_CP_High;													//SH_CP --> Low	
	
		if(prior == 1)
		{
			DS_High;								//DS --> Set log.'1'
		}
    if(prior == 0)
			{
				DS_Low;							//DS --> Set log '0'	
			}		
         
   Timer_DelayMillis(1);
	SH_CP_Low;													//SH_CP --> High		
}

/*    Send g_aShiftArray value to Shift register
*
*/
void ShiftReg_ArrayToReg()
{   
   ST_CP_Low;                           //ST_CP --> Low
   for(int i = 0; i <= 16; i++)
   {
      ShiftReg_SetData(g_aShiftArray[i]);
      Timer_DelayMillis(1);
   }
   ST_CP_High;
}

/*=======================================================================
                    Data return Part
========================================================================*/ 
/* Return WaterLevel Value
*
*/
int ShiftReg_WaterSensorReading()
{
   for(int i=0; i<=11; i++)
   {
    g_aShiftArray[i+5] = 1;     //Set log '1' to leg and....
    ShiftReg_ArrayToReg();     
     Timer_DelayMillis(1); 
    if(WaterLevelInput)   //read WaterCheck leg value  ! - for Holl sensor
     {                             //If we have signal -> stop and return value
      g_aShiftArray[i+5] = 0; 
       return i; //Water Level value      
     } 
     else      
      g_aShiftArray[i+5] = 0;
   }  
   return 0; //We dont have signal
}

/*   Return WaterLevel value and set its on Led Indication
*
*/
int ShiftReg_WaterLevelIndication()
{
  g_WaterLevelValue = ShiftReg_WaterSensorReading();    //Write WaterLevel value to Global variable
      
   ShiftReg_NullWaterLevelInd();
   
   for(int i=0; i<=g_WaterLevelValue/2.2; i++)   
   {
      g_aShiftArray[i] = 1;
   }
   ShiftReg_ArrayToReg();
   return g_WaterLevelValue;
}

/*   Null g_aShoftArray value
*
*/
void ShiftReg_NullWaterLevelInd()
{
   for(int i=0; i<=6; i++)
   {
      g_aShiftArray[i] =0;
   }
}

/* Return Water Level percent on char
*
*/
char ShiftReg_WaterLevelReturn(uint8_t WLSymbNumb)
{
  switch(WLSymbNumb)
  {
     case 0:
        return (g_WaterLevelValue + 48);
     
     case 1:
        return 48;      //Return Zero
  }     
  return 0;  //Input value don't correct
}

/*=======================================================================
                    CallTimer function Part
========================================================================*/ 
/*     Water Level call and controll function
*
*/
bool CallTimer_WaterLevel()
{
   if((Timer_minute() == 0 || Timer_minute() == 7 || Timer_minute() == 22 || Timer_minute() == 37 || Timer_minute() == 52) && g_WaterLevelFlag == false)
   {
      g_WaterLevelFlag = true;  //Get up flag
     ShiftReg_WaterLevelIndication();  //Call Water Level check function  
      return 1; //It's OK
   }
   else if(Timer_minute() == 1 || Timer_minute() == 8 || Timer_minute() == 23 || Timer_minute() == 38 || Timer_minute() == 53)
   {
      g_WaterLevelFlag = false;  //Get gown flag      
      return 0; //It's Not Time yet
   }
   else 
   {
      return 0; //It's Not Time yet
   }
      
}


