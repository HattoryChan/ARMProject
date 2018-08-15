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
   RCC->APB2ENR |= (RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN); //������������ ������ �, �, C � �������������� �������
   GPIOA->CRL &= ~(GPIO_CRL_CNF7 | GPIO_CRL_CNF5 | GPIO_CRL_CNF4); //������� ��� ������ ������ 
   GPIOA->CRL |= GPIO_CRL_CNF7_1 | GPIO_CRL_CNF5_1 | GPIO_CRL_CNF4_1; //PA7(MOSI), PA5(SCK), PA4(NSS) - AF, Output, PP
   GPIOA->CRL |= GPIO_CRL_MODE7 | GPIO_CRL_MODE5 | GPIO_CRL_MODE4; //������ MOSI, SCK, NSS - 50MHz
   GPIOA->CRL &= ~GPIO_CRL_MODE6; //PA6(MISO) - Input
   GPIOA->CRL |= GPIO_CRL_CNF6_0; //PA6(MISO) - Input floating
   
   /*��������� SPI1 (Master)*/
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; //������������ ������ SPI1
  SPI1->CR1 &= ~SPI_CR1_BR; //Baud rate = Fpclk/2
  SPI1->CR1 &= ~SPI_CR1_CPOL; //���������� ��������� ������� = 0
  SPI1->CR1 &= ~SPI_CR1_CPHA; //���� ��������� �������
  SPI1->CR1 &= ~SPI_CR1_DFF; //8 ��� ������
  SPI1->CR1 &= ~SPI_CR1_LSBFIRST; //MSB ���������� ������
  SPI1->CR1 |= SPI_CR1_SSM; //����������� ����� NSS
  SPI1->CR1 |= SPI_CR1_SSI; //���������� ���������, ����� �� ����� NSS ������� �������
  SPI1->CR2 |= SPI_CR2_SSOE; //����� NSS - ����� ���������� slave select
  SPI1->CR1 |= SPI_CR1_MSTR; //����� Master
  SPI1->CRCPR = 0x17;
 // SPI1->CR1 |= SPI_CR1_CRCEN;  //�������� ������� CRC
   SPI1->CRCPR = 7;
  SPI1->CR1 |= SPI_CR1_SPE; //�������� SPI1
   
   
    //Led
  GPIOC->CRH &= ~(GPIO_CRH_CNF8 | GPIO_CRH_CNF9); //PC8, PC9 - Output, PP 
  GPIOC->CRH |= GPIO_CRH_MODE8 | GPIO_CRH_MODE9; //PC8, PC9 - 50MHz
}
