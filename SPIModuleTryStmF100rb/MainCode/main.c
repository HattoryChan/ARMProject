#include "stm32f10x.h"

uint16_t temp = 0x02;
uint16_t send_data = 0x01;

//Инициализация GPIO
void gpio_init()
{ 
  //Линии SPI1 (Master)
  RCC->APB2ENR |= (RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN); //Тактирование портов А, В, C и альтернативных функций
  GPIOA->CRL &= ~(GPIO_CRL_CNF7 | GPIO_CRL_CNF5 | GPIO_CRL_CNF4); //Очистка бит выбора режима 
  GPIOA->CRL |= GPIO_CRL_CNF7_1 | GPIO_CRL_CNF5_1 | GPIO_CRL_CNF4_1; //PA7(MOSI), PA5(SCK), PA4(NSS) - AF, Output, PP
  GPIOA->CRL |= GPIO_CRL_MODE7 | GPIO_CRL_MODE5 | GPIO_CRL_MODE4; //Выходы MOSI, SCK, NSS - 50MHz
  GPIOA->CRL &= ~GPIO_CRL_MODE6; //PA6(MISO) - Input
  GPIOA->CRL |= GPIO_CRL_CNF6_0; //PA6(MISO) - Input floating
  
  //Линии SPI2 (Slave)
  GPIOB->CRH &= ~(GPIO_CRH_CNF14 | GPIO_CRH_CNF15 | GPIO_CRH_CNF13 | GPIO_CRH_CNF12); //Очистка бит выбора режима
  GPIOB->CRH |= GPIO_CRH_CNF14_1; //PB14(MISO) - AF, Output, PP
  GPIOB->CRH |= GPIO_CRH_MODE14; //Выход MISO - 50MHz
  GPIOB->CRH &= ~(GPIO_CRH_MODE15 | GPIO_CRH_MODE13 | GPIO_CRH_MODE12); //PB15(MOSI), PB13(SCK), PB12(NSS) - Input
  GPIOB->CRH |= GPIO_CRH_CNF15_0 | GPIO_CRH_CNF13_0 | GPIO_CRH_CNF12_1; //PB15(MOSI), PB13(SCK) - Input floating, PB12(NSS) - Input pull-up 
  
  //Led
  GPIOC->CRH &= ~(GPIO_CRH_CNF8 | GPIO_CRH_CNF9); //PC8, PC9 - Output, PP 
  GPIOC->CRH |= GPIO_CRH_MODE8 | GPIO_CRH_MODE9; //PC8, PC9 - 50MHz
}

//Инициализация SPI1, SPI2
void spi_init()
{ 
  /*Настройка SPI1 (Master)*/
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; //Тактирование модуля SPI1
  SPI1->CR1 |= SPI_CR1_BR; //Baud rate = Fpclk/256
  SPI1->CR1 |= SPI_CR1_CPOL; //Полярность тактового сигнала
  SPI1->CR1 &= ~SPI_CR1_CPHA; //Фаза тактового сигнала
  SPI1->CR1 &= ~SPI_CR1_DFF; //8 бит данных
  SPI1->CR1 &= ~SPI_CR1_LSBFIRST; //MSB передается первым
  SPI1->CR1 |= SPI_CR1_SSM; //Программный режим NSS
  SPI1->CR1 |= SPI_CR1_SSI; //Аналогично состоянию, когда на входе NSS высокий уровень
  SPI1->CR2 |= SPI_CR2_SSOE; //Вывод NSS - выход управления slave select
  SPI1->CR1 |= SPI_CR1_MSTR; //Режим Master
  SPI1->CR1 |= SPI_CR1_SPE; //Включаем SPI1
  
  /*Настройка SPI2 (Master) */
  RCC->APB1ENR |= RCC_APB1ENR_SPI2EN; //Тактирование модуля SPI2
  SPI2->CR1 |= SPI_CR1_BR; //Baud rate = Fpclk/256
  SPI2->CR1 |= SPI_CR1_CPOL; //Полярность тактового сигнала
  SPI2->CR1 &= ~SPI_CR1_CPHA; //Фаза тактового сигнала
  SPI2->CR1 &= ~SPI_CR1_DFF; //8 бит данных
  SPI2->CR1 &= ~SPI_CR1_LSBFIRST; //MSB передается первым
  SPI2->CR1 |= SPI_CR1_SSM; //Программный режим NSS
  SPI2->CR1 |= SPI_CR1_SSI; //Аналогично состоянию, когда на входе NSS высокий уровень
  SPI2->CR2 |= SPI_CR2_SSOE; //Вывод NSS - выход управления slave select
  SPI2->CR1 |= SPI_CR1_MSTR; //Режим Master
  SPI2->CR1 |= SPI_CR1_SPE; //Включаем SPI2
}


//Основной цикл программы
int main()
{
   int i;
  gpio_init(); //Вызов функции инициализации портов
  spi_init(); //Вызов функции инициализации модулей SPI 
  
   
   
  while(1)
  {
      
      
    SPI1->DR = 0x02;
    while(!(SPI1->SR & SPI_SR_RXNE));
     temp = SPI1->DR;
     
     
    SPI2->DR = 0x05;
    while(!(SPI2->SR & SPI_SR_RXNE));
     send_data = SPI2->DR;
    
    if(send_data != 0x01 )      
     GPIOC ->ODR ^= GPIO_ODR_ODR8;
    
    
    if(temp != 0x02)      
     GPIOC ->ODR ^= GPIO_ODR_ODR9;
    
 
    for(i=0; i<0x001FFFFF; i++); 
  }
}
