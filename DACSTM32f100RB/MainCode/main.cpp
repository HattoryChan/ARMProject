#include "stm32f10x.h"

/*
      DAC for stm32 PA[4]
*/

/* Массив, элементы которого нужно быстро запихивать в DAC чтоб получить синус */
/*
const uint16_t sin[32] = {
 2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
 3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909,
 599, 344, 155, 38,155, 344, 599, 909, 1000, 1263, 1647, 1867};
*/
const uint16_t sin[32] = {
 2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
 3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909,
 599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647};
 
unsigned char i=0;
 
void delayms(uint16_t ms);

extern "C"
{ 
void TIM2_IRQHandler(void)
{
	TIM2->SR &= ~TIM_SR_UIF;   //Reset Flag
	
	//DAC->DHR12R1=sin[i]; //Запихиваем в ЦАП очередной элемент массива
  //    if (i==32) i=0; //Если вывели в ЦАП все 32 значения то начинаем заново		  
	//i++;
}


 void TIM6_DAC_IRQHandler(void)
{
	TIM6->SR &= ~TIM_SR_UIF;   //Reset Flag
	
	DAC->DHR12R1=sin[i++]; //Запихиваем в ЦАП очередной элемент массива
   if (i==32) i=0; //Если вывели в ЦАП все 32 значения то начинаем заново		  

}
}    // END Extern "C"
void InitAll()
{
   //---------------------------------   GPIO and DAC
   
   RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	 RCC->APB1ENR |= RCC_APB1ENR_DACEN;
   // PA[4] - output, Fmax = 2Mhz, Alternative Hi-Z mode
   GPIOA->CRL |= GPIO_CRL_CNF4;
   GPIOA->CRL |= GPIO_CRL_MODE4;
   
	//---------------------------------    TIM2
//	 RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	 
//	 TIM2->PSC = 0;
//	 TIM2->ARR = 500;
//	 TIM2->DIER |= TIM_DIER_UIE;		 
//	 TIM2->CR1 |= TIM_CR1_CEN;
	
//	NVIC_EnableIRQ(TIM2_IRQn);
	//NVIC_SetPriority(TIM2_IRQn, 1);
	
	//---------------------------------    TIM6
	 RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	 	
    TIM6-> PSC = 2;
    TIM6->ARR = 10;
   // TIM6->ARR = 12300; //Anton frequency aetting
    
    // TIM6->PSC = 0;      //10.2 KHz
	// TIM6->ARR = 72;      //10.2 KHz    
   
	 TIM6->DIER |= TIM_DIER_UIE;		 
	 TIM6->CR1 |= TIM_CR1_CEN;
	
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
	//NVIC_SetPriority(TIM2_IRQn, 1);
	
   //------------------------------------   DAC
   //On DAC1
   DAC->CR |= DAC_CR_EN1;
   /*
   DAC->DHR12R1 = value; 
                  value - значение выводимое на ноге.
   */
}


int main()
{
   InitAll();
 //  DAC->DHR12R1 = 4095;   
   while(1)
   {      
 //     DAC->DHR12R1=sin[i++]; //Запихиваем в ЦАП очередной элемент массива
 //      if (i==32) i=0; //Если вывели в ЦАП все 32 значения то начинаем заново
//      delayms(1);
   }
}



//-----------------------------------------------    Delay Function (ms)
void delayms(uint16_t ms)
{
	uint32_t i;
 for(; ms !=0; ms--)
	{
		for(i = 0x2710; i != 0; i--);											// 1ms delay if Fmax = 72mHz
	}
}

 
