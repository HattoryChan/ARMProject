#include "spi.h"

uint8_t W5500_rxtx(uint8_t data)
{
   while(!(SPI1->SR & SPI_SR_RXNE));
   SPI1->DR = data;
   while(!(SPI1->SR & SPI_SR_TXE));
   return SPI1->DR;
}

void spiW5500_init(void)
{
   RCC->APB2ENR |= (RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN); //Тактирование портов А, В, C и альтернативных функций
   GPIOA->CRL &= ~(GPIO_CRL_CNF7 | GPIO_CRL_CNF5 | GPIO_CRL_CNF4); //Очистка бит выбора режима 
   GPIOA->CRL |= GPIO_CRL_CNF7_1 | GPIO_CRL_CNF5_1 | GPIO_CRL_CNF4_1; //PA7(MOSI), PA5(SCK), PA4(NSS) - AF, Output, PP
   GPIOA->CRL |= GPIO_CRL_MODE7 | GPIO_CRL_MODE5 | GPIO_CRL_MODE4; //Выходы MOSI, SCK, NSS - 50MHz
   GPIOA->CRL &= ~GPIO_CRL_MODE6; //PA6(MISO) - Input
   GPIOA->CRL |= GPIO_CRL_CNF6_0; //PA6(MISO) - Input floating
   
   /*Настройка SPI1 (Master)*/
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; //Тактирование модуля SPI1
  SPI1->CR1 &= ~SPI_CR1_BR; //Baud rate = Fpclk/2
  SPI1->CR1 &= ~SPI_CR1_CPOL; //Полярность тактового сигнала = 0
  SPI1->CR1 &= ~SPI_CR1_CPHA; //Фаза тактового сигнала
  SPI1->CR1 &= ~SPI_CR1_DFF; //8 бит данных
  SPI1->CR1 &= ~SPI_CR1_LSBFIRST; //MSB передается первым
  SPI1->CR1 |= SPI_CR1_SSM; //Программный режим NSS
  SPI1->CR1 |= SPI_CR1_SSI; //Аналогично состоянию, когда на входе NSS высокий уровень
  SPI1->CR2 |= SPI_CR2_SSOE; //Вывод NSS - выход управления slave select
  SPI1->CR1 |= SPI_CR1_MSTR; //Режим Master
  SPI1->CRCPR = 0x17;
 // SPI1->CR1 |= SPI_CR1_CRCEN;  //Включаем подсчет CRC
   SPI1->CRCPR = 7;
  SPI1->CR1 |= SPI_CR1_SPE; //Включаем SPI1
   
   
    //Led
  GPIOC->CRH &= ~(GPIO_CRH_CNF8 | GPIO_CRH_CNF9); //PC8, PC9 - Output, PP 
  GPIOC->CRH |= GPIO_CRH_MODE8 | GPIO_CRH_MODE9; //PC8, PC9 - 50MHz
}
