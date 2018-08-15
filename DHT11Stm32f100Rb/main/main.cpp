#include "stm32f10x.h"
#include "Periphery_Init.h"
#include "delay.h"


uint8_t dataDHT11[5];   //Array for DHT11 data

uint16_t Time_us = 0; // count
uint16_t Time_ms = 0;

//Reset Time_us and Time_ms
void Time_ResetCounter(int SwitchValue); 
int DHT11_GetData();  //Get message from DHT11 function

extern "C"   //if we don't translate interrupts handler to "C"
{            // we down to last position NVIC_Handler table
   
   //TIM2 for measure time
   void TIM2_IRQHandler(void)
   {
      TIM2->SR &= ~TIM_SR_UIF;   //Reset interruption flag
      if(Time_us == 100)  //1000us = 1 ms
      {
         Time_us = 0;
         
        if(Time_ms == 1000) // 1000ms = 1 s
           Time_ms = 0;
        
         Time_ms ++;                 
      }
      Time_us++;  // time count: 10us -> 20us -> 30us			
   }

   void USART1_IRQHandler(void)
   {
	  if(USART1->SR & USART_SR_RXNE) //If interrupts calling input message
     {
        USART1->SR &= ~USART_SR_RXNE;   //Reset interruption Flag
        
        if(USART1->DR == '1')
        {
           USART1_Send_String("DHT11 data: ");
           
           for(int i=0; i<5; i++)   //send data 
               USART1_Send((int)dataDHT11[i]);          
           if(DHT11_GetData() == 1)    //check if it's OK
           {
              USART1_Send_String("DHT11 data: ");
              
              for(int i=0; i<5; i++)   //send data 
               USART1_Send(dataDHT11[i]);
           }        
          else
               USART1_Send_String("ERROR");  
        }
     }        
   }
}
int main(void)
{
  InitPeriphery(9600); // Initiall perephery MC   

      USART1_Send_String("0");
    DHT11_OutLeg_Setting();   
   GPIOC->BSRR = GPIO_BSRR_BR8;
 while(1)
  {   
   
   //  delay(1);
   //  GPIOA->BSRR = GPIO_BSRR_BR8;
   //    delay(1);
  }
}

/*------------------------------------
Get data from DHT11
------------------------------------*/
int DHT11_GetData()
{
  dataDHT11[0] = dataDHT11[1] = dataDHT11[2] = dataDHT11[3] = dataDHT11[4] = 0;   
   
 // Send output signal for wake DHT11
   DHT11_OutLeg_Setting(); //DHT11 leg - output
   
   GPIOC->BSRR = GPIO_BSRR_BS8;   //Leg for 0
   
      Time_ResetCounter(2);   //Reset All time counter value 
      while(Time_us > 2); //delay 20us  
   
   GPIOC->BSRR = GPIO_BSRR_BR8;  //Leg for 1 ( Send wake up signal)
   
      Time_ResetCounter(2); //Reset All time counter value
      while(Time_us > 3); //delay 40us
   
  // GPIOA->BSRR = GPIO_BSRR_BR8;  //Leg for 1 ( Send wake up signal) 
   
   DHT11_InLeg_Setting();
   if(!(DHT11_LegStatus() == '1'))
        USART1_Send('8');
      
      while(!DHT11_LegStatus());  //Wait answer from DHT11
      USART1_Send_String("1\n");                                                         //111111!~
      Time_ResetCounter(2);
      while(Time_us > 7);   //delay 80us
      
      
     
      while(!DHT11_LegStatus()); //Wait Start Data from GHT11
       USART1_Send_String("1.5\n");                                                         //111111!~
      while(DHT11_LegStatus() || Time_ms == 20); // We get message
       USART1_Send_String("2\n");                                                         //111111!~
      for(int j = 0; j <= 5; j++)
      {
         dataDHT11[j] = 0;
         
         for(int i = 0; i <= 8; i++)
         {
            __disable_irq(); // Disallow ALL Interrupt            
                         
               Time_ResetCounter(2);
               while(Time_us > 2);   
            
            if(DHT11_LegStatus())  //If signal HIGH, we get '1'
               dataDHT11[j] |= 1<<(7-i);
            while(!DHT11_LegStatus()); //wait end HIGH signal
            
           __enable_irq();   // Allow interrupt
         }
          USART1_Send_String("3");                                                         //111111!~
      }      
//==============================    Check control summ block
   //   if(dataDHT11[0]==0 && dataDHT11[1]==0 && dataDHT11[2]==0 && dataDHT11[3]==0 && dataDHT11[4]==0)
   //            return 0;
    //  else if(dataDHT11[0] + dataDHT11[1] + dataDHT11[2] + dataDHT11[3] == dataDHT11[4])
         return 1;
   //   else 
   //      return 0;
}

/*-------------------------------------
Reset Time_us and Time_ms. 
"SwitchValue" :
 "0" - Time_us
 "1" - Time_ms
 "2" - Time_us and Time_ms
-------------------------------------*/
void Time_ResetCounter(int SwitchValue)
{
   if(SwitchValue == 0)
      Time_us = 0;
   
   if(SwitchValue == 1)
      Time_ms = 0;
   
   if(SwitchValue == 2)
   {
      Time_us = 0;
      Time_ms = 0;
   }
}
