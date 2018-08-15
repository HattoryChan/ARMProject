#include "stm32f10x.h"
#include "UsartInitAndRX_TXFUnction.h"
#include "delay.h"
#include <math.h>


#define DHT11_InLegStatus0 (GPIOC->IDR & GPIO_IDR_IDR8) == !GPIO_IDR_IDR8 //try if we have LOW
#define DHT11_InLegStatus1 (GPIOC->IDR & GPIO_IDR_IDR8) == GPIO_IDR_IDR8 //try if we have High

int Time_us = 0;
int Time_ms = 0;
uint8_t datadht[5];


void USART1_IRQHandler(void)
{
			
}
void ClockPPLHSITry(void);

void ClockPPLHSETry(void);
void ClockHSETry(void);
void InitialAll(void);
//Work with timer
uint8_t TIM2_GiveTime_us(void);
uint8_t TIM2_GiveTime_ms(void);
void TIM2_StartCounting(void);
void TIM2_StopCounting(void);
//Work with DHT11 Leg
void DHT11_OUT(void);
void DHT11_IN(void);
void DHT11_OutLow(void);
void DHT11_OutHigh(void);
void DHT11_InZ(void);
//uint8_t DHT11_InLegStatus(void);
//Work with DHT11
int DHT11_Read(void);

//Tim2 for 5us count, max value - 1ms
void TIM2_IRQHandler(void)
{												
	Time_us+=10;
   if(Time_us >=1000) 
   { 
      Time_us = 0;     
      Time_ms++;
      if(Time_ms >=100)
         Time_ms =0;
   }  
   			//Reset interrupt bits(flag)
	TIM2->SR &= ~TIM_SR_UIF;        
}


int main()
{  
  // ClockPPLHSITry();
   ClockHSETry();
   ClockPPLHSETry();
   InitUART();
	InitialAll();  
   DHT11_OUT();     
   USART1_Send_String("Start");   
	while(1)
   { 
      DHT11_Read();
    //  if(DHT11_Read() == 1)    
    //  {
       //Проверка датчика на ошибки
              if (datadht[0]==0 && datadht[1]==0 && datadht[2]==0 && datadht[3]==0)
              { 
                     USART1_Send('0');
              }
               // Проверка контрольной суммы
              else if (datadht[0] + datadht[1] + datadht[2] + datadht[3] == datadht[4])
             { // Если сумма совпадает, то ок
                     USART1_Send('1');
       }  
                 
       for(int i=0; i<5; i++)   //send data 
               USART1_Send(datadht[i]);              
      USART1_Send('\r\n');
     //  }
         delay(5);  
      

  }   
}



void InitialAll()
{
   //=================----------------------========================--------------------
															//Clocking TIM2
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;	
															//Initial TIM2
													//Preference prescaler timer
		TIM2->PSC = 2;
													//Sets the number of milliseconds
	  TIM2->ARR = 79;                      //10uS = 100kHz
	//Allowed interrupts if counter full
	TIM2->DIER |= TIM_DIER_UIE;
													//Starts account
	TIM2->CR1 |= TIM_CR1_CEN;
														//Allowed Interrupts TIM2
	NVIC_EnableIRQ(TIM2_IRQn);
														//Interrupts priority
	NVIC_SetPriority(TIM2_IRQn,1);
   
   
   
   //====================-------------------------=======================----------------------====================
   RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;          //Clocking port C
   
   														//Initial Port C [8] - output, fmax=50Mhz,push-pull
//	GPIOC->CRH |= GPIO_CRH_MODE8_0;
//	GPIOC->CRH &= ~GPIO_CRH_CNF8;
}

//==============------------------------ Work with TIM2 Part =========-------------------
void TIM2_StartCounting()
{
   Time_us = 0;
   Time_ms = 0;
   TIM2->CR1 |= TIM_CR1_CEN;
}

void TIM2_StopCounting()
{
   TIM2->CR1 &= TIM_CR1_CEN;
}

uint8_t TIM2_GiveTime_us()
{
   return Time_us;
}

uint8_t TIM2_GiveTime_ms()
{
   return Time_ms;
}
//=================================-------------   END      ===================---------------

//-----------------------------------========== Work with DHT11 Leg ---------=================
void DHT11_OUT()
{
   GPIOC->CRH |= GPIO_CRH_MODE8_0;
	GPIOC->CRH &= ~GPIO_CRH_CNF8;
}

void DHT11_IN()
{
   GPIOC->CRH &= ~GPIO_CRH_CNF8;
	GPIOC->CRH |= GPIO_CRH_CNF8_1;
}

void DHT11_OutLow()
{   
   GPIOC->BSRR = GPIO_BSRR_BR8; //reset bit
}

void DHT11_OutHigh()
{
   GPIOC->BSRR = GPIO_BSRR_BS8;  //set bit
}

void DHT11_InZ()
{  
   GPIOC->CRH &= ~GPIO_CRH_CNF8;
}
/*
uint8_t DHT11_InLegStatus()
{
   return(GPIOC->IDR && GPIO_IDR_IDR8);
}*/
//=================================-------------   END      ===================---------------

//=================================------------- DHT11 work function   =================================-------------
int DHT11_Read()
{
   unsigned int j =0, i = 0;   
   datadht[0] = datadht[1] = datadht[2] = datadht[3] = datadht[4] = 0 ;     
                                                         //Step 2
   //DHT11 leg - output, low level
 //  DHT11_OUT();
  //DHT11_OutLow();
   
   //Delay 20ms
   Time_us = Time_ms = 0;
   while(TIM2_GiveTime_ms() < 30);
  
   
   //DHT11 - Hi-z input
  // DHT11_InZ();   
   DHT11_IN();
 //  GPIOC->ODR |= GPIO_ODR_ODR8;    //Pull up to high    //это видит

   //Delay 40us
   Time_us = 0;
   while(TIM2_GiveTime_us() < 40);
   
   
   //Must return Low signal
//   if(DHT11_InLegStatus1)
  //    return 0;    //ERROR, exit    
 
   //All ok, delay 80us
   Time_us = 0;   
   while(TIM2_GiveTime_us() < 80);
   
   //Must be HIGH signal   
 //  if(DHT11_InLegStatus0)
//      return 0;    //Exit        //это видит
    
   //Wait start sending data
//   while(DHT11_InLegStatus1);   
 
   
   //передача начинается с нуля
  // __disable_irq(); // Disallow ALL Interrupt  
   for (j=0; j<5; j++)
   {
      datadht[j]=0;  //если ставим 1 и проверяем на 0 - то выдает результат!
   }
      for (i=0; i<8; i++)
      {             
//        while(DHT11_InLegStatus0);   /* ждем когда датчик отпустит шину */  //Все вайлы и шаы проходит, но не записывает(
         /*задержка высокого уровня при 0 30 мкс*/
         Time_us = 0;     
         while(TIM2_GiveTime_us() < 80);       //либо задержка большевата, либо не видит состояние ноги в этот момент
            if (DHT11_InLegStatus0) /*если по истечению времени сигнал на линии высокий, значит передается 1*/     //не работает
            {
              datadht[j] = (1<<(7-i)); /*тогда i-й бит устанавливаем 1*/                 
            }
            
   //       while (DHT11_InLegStatus1);  // ждем окончание 1   
        
          }
//   __enable_irq();   // Allow interrupt
          USART1_Send_String("\n\rStart sending\n\r");
          return 1;
}
   



//=================================-------------   END      ===================---------------




void ClockHSETry()
{
   RCC->CR|=RCC_CR_HSEON; //Запускаем генератор HSE.
   while (!(RCC->CR & RCC_CR_HSERDY)) {}; // Ждем готовности
   RCC->CFGR &=~RCC_CFGR_SW; //Сбрасываем биты
   RCC->CFGR |= RCC_CFGR_SW_HSE; // Переходим на HSE      
}

void ClockPPLHSITry()
{
   RCC->CFGR |= RCC_CFGR_PLLMULL6;               //HSI / 2 * 6 = 24 MHz
  RCC->CR |= RCC_CR_PLLON;                      //enable PLL
  while((RCC->CR & RCC_CR_PLLRDY) == 0) {}      //wait till PLL is ready
  RCC->CFGR &= ~RCC_CFGR_SW;                    //clear SW bits
  RCC->CFGR |= RCC_CFGR_SW_PLL;                 //select PLL as system clock
  while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_1) {} //wait till PLL is used
}

void ClockPPLHSETry()
{
   RCC->CR |= RCC_CR_HSEON;                      //enable HSE
  while((RCC->CR & RCC_CR_HSERDY) == 0) {}      //wait till HSE is ready
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1  |            //HCLK = SYSCLK
               RCC_CFGR_PPRE2_DIV1 |            //PCLK2 = HCLK
               RCC_CFGR_PPRE1_DIV1;             //PCLK1 = HCLK
  RCC->CFGR &= ~RCC_CFGR_PLLMULL;               //clear PLLMULL bits
  RCC->CFGR |= RCC_CFGR_PLLSRC_PREDIV1 |        //PLL source = PREDIV1
               RCC_CFGR_PLLXTPRE_PREDIV1_Div2 | //HSE / 2 = 4 MHz
               RCC_CFGR_PLLMULL6;               //4 * 6 = 24 MHz
  RCC->CR |= RCC_CR_PLLON;                      //enable PLL
  while((RCC->CR & RCC_CR_PLLRDY) == 0);        //wait till PLL is ready
  RCC->CFGR &= ~RCC_CFGR_SW;                    //clear SW bits
  RCC->CFGR |= RCC_CFGR_SW_PLL;                 //select PLL as system clock
  while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_1); //wait till PLL is used
}
