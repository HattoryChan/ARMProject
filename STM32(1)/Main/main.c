#include "stm32f10x.h"

void delay(uint16_t sec)
{
	uint32_t i;
 for (; sec != 0; sec--)
	{
		for(i = 0x5B8D80; i !=0; i--);										// 1 second delay if Fmax = 72 Mhz
	}
}

void InitAll(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; 
	
	GPIOA->CRL &= ~GPIO_CRL_CNF1;
	
	GPIOA->CRL |= GPIO_CRL_MODE1_0;
	
	return;	
}	
int main(void)
{
	InitAll();
	
	while(1)
	{
		GPIOA->BSRR = GPIO_BSRR_BS1;
		delay(1);
		GPIOA->BSRR = GPIO_BSRR_BR1;
		delay(1);
	}
}
