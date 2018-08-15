#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "spi.h"


uint8_t W5500_rxtx(uint8_t data)
{  //Перебрать эти while
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET);
	SPI1->DR = data;

	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)==RESET);
	return SPI1->DR;
}


//Ошибка в настройке порта.
void spiW5500_init(void)
{

	// Initialize SPI and GPIO
	RCC_APBPeriphClockCmd_SPI_W5500(RCC_APBPeriph_SPI_W5500, ENABLE);
	RCC_APBPeriphClockCmd_CS_W5500(RCC_APB2Periph_GPIO_W5500_CS, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;

	// MOSI & CLK
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_SPI_W5500_SCK | GPIO_Pin_SPI_W5500_MOSI;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIO_SPI_W5500, &GPIO_InitStruct);

	// MISO
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_SPI_W5500_MISO;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIO_SPI_W5500, &GPIO_InitStruct);

	// CS
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_CS_W5500;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIO_W5500_CS, &GPIO_InitStruct);


	SPI_InitTypeDef SPI_InitStruct;

	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_Init(SPI_W5500, &SPI_InitStruct);


	SPI_Cmd(SPI_W5500, ENABLE);



	  // Включаем модули USART1 и GPIOA, а также включаем альтернативные функции выходов
	  RCC->APB2ENR|= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
	  // Контакт PA9 будет выходом с альтернативной функцией, а контакт PA10 - входом
	  GPIOA->CRH &= !GPIO_CRH_CNF9;
	  GPIOA->CRH  |= GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9_0 | GPIO_CRH_CNF10_0;
	  // Настраиваем регистр тактирования, скорость составит 9600 бод (при тактовой частоте 24 МГц)
	  USART1->BRR = 0x9C4;
	  // Выключаем TxD и RxD USART
	  USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
	  // Запускаем модуль USART
	  USART1->CR1 |= USART_CR1_UE;
	  // Разрешаем прерывание по приёму информации с RxD
	  USART1->CR1 |= USART_CR1_RXNEIE;
	  // Назначаем обработчик для всех прерываний от USART1
	  NVIC_EnableIRQ(USART1_IRQn);
	  USART1_Send_String("Start\r\n");
}


void USART1_Send(uint8_t chr) {
  while(!(USART1->SR & USART_SR_TC));
  USART1->DR = chr;
}

void USART1_Send_String(char* str) {
  int i=0;
  while(str[i])
    USART1_Send(str[i++]);
}
