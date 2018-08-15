/*==============================================================
*     Controll step motor 
*
*
*  Using LEG:
*   Port C:                         Port A:
*    First motor 28BYJ-48           Second Motor 42BYGHW609
*     [0] - IN1                      [0] - IN1
*     [1] - IN2                      [1] - IN3
*     [2] - IN3                      [2] - IN2
*     [3] - IN4                      [3] - IN4 
 ===============================================================*/
#include "StepMotor_InitAndWork.h"
#include "delay.h"

//IN1
#define IN3_HIGH GPIOB->BSRR = GPIO_BSRR_BR0;  //HIGH
#define IN3_LOW GPIOB->BSRR = GPIO_BSRR_BS0;  //LOW
//IN2
#define IN4_HIGH GPIOB->BSRR = GPIO_BSRR_BR1;  //HIGH
#define IN4_LOW GPIOB->BSRR = GPIO_BSRR_BS1;  //LOW
//IN3
#define IN1_HIGH GPIOB->BSRR = GPIO_BSRR_BR2;  //HIGH
#define IN1_LOW GPIOB->BSRR = GPIO_BSRR_BS2;  //LOW
//IN4
#define IN2_HIGH GPIOB->BSRR = GPIO_BSRR_BR3;  //HIGH
#define IN2_LOW GPIOB->BSRR = GPIO_BSRR_BS3;  //LOW

#define StepFirstMotorPort GPIOC->ODR
#define StepSecondMotorPort GPIOA->ODR

uint8_t g_aStepMotorFirstArray[8] = {0x3,0x1,0x9,0x8,0xC,0x4,0x6,0x1};
uint8_t g_aStepMotorSecondArray[8] = {0x3,0x1,0x9,0x8,0xC,0x4,0x6,0x1};
uint8_t g_StepCountFirst= 0;
uint8_t g_StepCountSecond= 0;

uint8_t StepCountFirstForward();  //Counting half-step number forward for first motor
uint8_t StepCountFirstBack();     //Counting half-step number back for first motor

uint8_t StepCountSecondForward();  //Counting half-step number forward for second motor
uint8_t StepCountSecondBack();     //Counting half-step number back for second motor

/*    Step Motor Leg Init
*
*/
void StepMotor_Init()
{
   //First Motor
   RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; //clocking for Port C
   GPIOC->CRL &= ~GPIO_CRL_CNF0;     //output, push-pull
   GPIOC->CRL |= GPIO_CRL_MODE0_1;   // Port C[0]   

   GPIOC->CRL &= ~GPIO_CRL_CNF1;     //output, push-pull
   GPIOC->CRL |= GPIO_CRL_MODE1_1;   // Port C[1]   

   GPIOC->CRL &= ~GPIO_CRL_CNF2;     //output, push-pull
   GPIOC->CRL |= GPIO_CRL_MODE2_1;   // Port C[2]   

   GPIOC->CRL &= ~GPIO_CRL_CNF3;     //output, push-pull
   GPIOC->CRL |= GPIO_CRL_MODE3_1;   // Port C[3]   
   
   
    //Second Motor
   RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; //clocking for Port A
   GPIOA->CRL &= ~GPIO_CRL_CNF0;     //output, push-pull
   GPIOA->CRL |= GPIO_CRL_MODE0_1;   // Port A[0]   

   GPIOA->CRL &= ~GPIO_CRL_CNF1;     //output, push-pull
   GPIOA->CRL |= GPIO_CRL_MODE1_1;   // Port A[1]   

   GPIOA->CRL &= ~GPIO_CRL_CNF2;     //output, push-pull
   GPIOA->CRL |= GPIO_CRL_MODE2_1;   // Port A[2]   

   GPIOA->CRL &= ~GPIO_CRL_CNF3;     //output, push-pull
   GPIOA->CRL |= GPIO_CRL_MODE3_1;   // Port A[3]   

  //StepSecondMotorPort = 0xF;
  //StepFirstMotorPort = 0xF;
}

/*===========================================================================================================
*                                First step-motor controll function
*
  ===========================================================================================================*/

/* Rotate step motor 1 on 1 degree Forward
*  Input: DegreeNumb - degree number
*/
void StepMotor_StepFirstForward(uint16_t DegreeNumb)
{
   for (int i=0; i<=DegreeNumb; i++)
   {
      for(int i=0; i<=11; i++)      //Setting for 28BYJ-48 stepMotor
      {
      StepFirstMotorPort = g_aStepMotorFirstArray[StepCountFirstForward()];      
      for(int g = 0x2450; g != 0; g--);
      } 
      StepFirstMotorPort = 0x0;
   }
}


/* Rotate step motor 1 on 1 degree Back
*  Input: DegreeNumb - degree number
*/
void StepMotor_StepFirstBack(uint16_t DegreeNumb)
{
    for (int i=0; i<=DegreeNumb; i++)
   {
      for(int i=0; i<=9; i++)      //Setting for 28BYJ-48 stepMotor
      {
      StepFirstMotorPort = g_aStepMotorFirstArray[StepCountFirstBack()];      
      for(int g = 0x2450; g != 0; g--);
      } 
      StepFirstMotorPort = 0x0;
   }
   
}

/*  Counting half-step number forward
*   Output: Half-step number
*/
uint8_t StepCountFirstForward()
{    
     g_StepCountFirst++;     
   if(g_StepCountFirst > 7)
    {
      g_StepCountFirst = 0;
    }           
     return g_StepCountFirst;
}


/*  Counting half-step number back
*   Output: Half-step number
*/
uint8_t StepCountFirstBack()
{    
     g_StepCountFirst--;     
   if(g_StepCountFirst <= 0)
    {
      g_StepCountFirst = 7;
    }           
     return g_StepCountFirst;
}


/*===========================================================================================================
*                                Second step-motor controll function
*
  ===========================================================================================================*/

/* Rotate step motor 2 on 1 degree Forward
*  Input: DegreeNumb - degree number
*/
void StepMotor_StepSecondForward(uint16_t DegreeNumb)
{
   for (int i=0; i<=DegreeNumb; i++)
   {
      for(int i=0; i<=9; i++)      //Setting for 28BYJ-48 stepMotor
      {
      StepSecondMotorPort = g_aStepMotorSecondArray[StepCountSecondForward()];      
      for(int g = 0x4850; g != 0; g--);
      } 
      StepSecondMotorPort = 0x0;
   }
}


/* Rotate step motor 2 on 1 degree Back
*  Input: DegreeNumb - degree number
*/
void StepMotor_StepSecondBack(uint16_t DegreeNumb)
{
    for (int i=0; i<=DegreeNumb; i++)
   {
      for(int i=0; i<=8; i++)      //Setting for 28BYJ-48 stepMotor
      {
      StepSecondMotorPort = g_aStepMotorSecondArray[StepCountSecondBack()];      
      for(int g = 0x5250; g != 0; g--);
      } 
      StepSecondMotorPort = 0x0;
   }
   
}

/*  Counting half-step number forward
*   Output: Half-step number
*/
uint8_t StepCountSecondForward()
{    
     g_StepCountSecond++;     
   if(g_StepCountSecond > 7)
    {
      g_StepCountSecond = 0;
    }           
     return g_StepCountSecond;
}


/*  Counting half-step number back
*   Output: Half-step number
*/
uint8_t StepCountSecondBack()
{    
     g_StepCountSecond--;     
   if(g_StepCountSecond <= 0)
    {
      g_StepCountSecond = 7;
    }           
     return g_StepCountSecond;
}

