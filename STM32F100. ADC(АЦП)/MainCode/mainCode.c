#include "stm32f10x.h"
/*=========================================================
This program will be use potentiometer on PA1 and make anteiliger delay.
PA1 - potentiometer leg
PC[8,9] - Led
==========================================================*/

void delay(uint32_t i); 						//delay function
//====================================================				Initiall Function.
																			  
void InitAll()
{
	//-----------------------------------    GPIOA
																												//Clocking Port [A]
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
																									//PA[1]- input, Hi-z
	GPIOA->CRL &= GPIO_CRL_MODE1;
	GPIOA->CRL |= GPIO_CRL_CNF1_0;
	//------------------------------------   GPIOC
																												//Clocking Port [C]
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
																									//PC[8,9] - output, fmax=2MHz,Push-Pull
	GPIOC->CRH |= GPIO_CRH_MODE8_1 | GPIO_CRH_MODE9_1;
	GPIOC->CRH &= ~(GPIO_CRH_CNF8 | GPIO_CRH_CNF9);
	//------------------------------------    ADC
																										//Clocking ADC
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
																									//Calibration ADC
																							//Start
	ADC1->CR2 |= ADC_CR2_CAL;
																				//Wait calibration ending
	while(!(ADC1->CR2 & ADC_CR2_CAL)){}		//<---- This litle MAGIC
	
																//Set the sampling duration
  ADC1->SMPR2 |= (ADC_SMPR2_SMP1_2 | ADC_SMPR2_SMP1_1 | ADC_SMPR2_SMP1_0);
																						//Conversion of the injected group
																	//Strart with set JSWSTART bit
	ADC1->CR2 |= ADC_CR2_JEXTSEL;
																	//Allow start external injected group
	ADC1->CR2 |= ADC_CR2_JEXTTRIG;	
																	//external start in series
	ADC1->CR2 |= ADC_CR2_CONT;
																	//Allow start injected group after regular group
	ADC1->CR2 |= ADC_CR1_JAUTO;
																	//Sets chanel Number(ADC1)
	ADC1->JSQR |= (1<<15);	
																
}
//=================================================   MAIN function
int main(void)
{	
	//Debug variable
	uint32_t adc_res;
	InitAll();
	
																										//Check LED and DELAY
	GPIOC->BSRR = GPIO_BSRR_BS9;
	GPIOC->BSRR = GPIO_BSRR_BS8;	
	delay(10);
	GPIOC->BSRR = GPIO_BSRR_BR9;
	GPIOC->BSRR = GPIO_BSRR_BR8;
	
	
	
		//ADC ON
	ADC1->CR2 |= ADC_CR2_ADON;
																	//Start external
	ADC1->CR2 |= ADC_CR2_JSWSTART;
																	//Wait until the first external complited
	while(!(ADC1->SR & ADC_SR_JEOC)){} //<------ THIS IS MAGIC
																	//Read resuilt 											
  while(1)
	{
															// Read value
	 adc_res = ADC1->JDR1;
														// make anteiliger delay
	 delay(adc_res);	
															//LED PC[8] OFF
	 GPIOC->BSRR = GPIO_BSRR_BR8;
															//LED PC[9] ON
	 GPIOC->BSRR = GPIO_BSRR_BS9;	
																//Repeat this
	 adc_res = ADC1->JDR1;	
	 delay(adc_res);
															//LED PC[9] OFF
	 GPIOC->BSRR = GPIO_BSRR_BR9;
															// LED PC[8] ON
	 GPIOC->BSRR = GPIO_BSRR_BS8;	
	
	}		 
}

//================================================     DELAY FUNCTION
void delay(uint32_t i) 
{
	volatile uint32_t j;
	for(j=0; j!= i * 1000; j++);
}
