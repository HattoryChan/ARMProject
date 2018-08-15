#include "stm32f10x.h"
#include "stm32f10x_i2c.h"
#include "i2c.h"


void InitKeyAll(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN|RCC_APB2ENR_IOPCEN;

  GPIOC->CRH   |= GPIO_CRH_MODE8; // ???? ?????????? LED4 PC8 ?? ?????.
  GPIOC->CRH   &=~GPIO_CRH_CNF8; //  ??????????? ????? ?? PC8.
  GPIOC->CRH   |= GPIO_CRH_MODE9; // ???? ?????????? LED3 PC9 ?? ?????.
  GPIOC->CRH   &=~GPIO_CRH_CNF9; // ??????????? ????? ?? PC9.

  GPIOA->CRL&=~GPIO_CRL_MODE0; // ?????? "USER" PA0 - ?? ????.
}

uint8_t i_tx_uart=0;
uint8_t *TxBuf;


void InitUART()
{
  //CLOCK
  RCC->APB2ENR	|= RCC_APB2ENR_USART1EN;	      //USART1 Clock ON
  RCC->APB2ENR  |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN; 	// GPIOA Clock ON. Alter function clock ON
  //TX PIN
  GPIOA->CRH	&= ~GPIO_CRH_CNF9; 				// Clear CNF bit 9
  GPIOA->CRH	|= GPIO_CRH_CNF9_1;				// Set CNF bit 9 to 10 - AFIO Push-Pull
  GPIOA->CRH	|= GPIO_CRH_MODE9_0;				// Set MODE bit 9 to Mode 01 = 10MHz
  //RX PIN
  GPIOA->CRH	&= ~GPIO_CRH_CNF10;	// Clear CNF bit 9
  GPIOA->CRH	|= GPIO_CRH_CNF10_0;	// Set CNF bit 9 to 01 = HiZ
  GPIOA->CRH	&= ~GPIO_CRH_MODE10;	// Set MODE bit 9 to Mode 01 = 10MHz

  USART1->BRR = 0xD0;//0xD05;		// Bodrate for 2400 on 8Mhz
  //USART1->CR1  |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE; // USART1 ON, TX ON, RX ON
  USART1->CR1 	|= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE |USART_CR1_RXNEIE;// USART1 ON, TX ON, RX ON

  NVIC_EnableIRQ (USART1_IRQn);
}
void USART1_IRQHandler(void)
{
  if (USART1->SR & USART_SR_RXNE)
  {
    USART1->DR = (USART1->DR)+1;
    USART1->CR1 |= USART_CR1_TXEIE;//EN INT DRE
  }

  if (USART1->SR & USART_SR_TC)
  {
    USART1->CR1 &= ~USART_CR1_TCIE;
    USART1->DR = 'n';
  }
  else if (USART1->SR & USART_SR_TXE)
  {
    if(i_tx_uart)
    {
      TxBuf++;
      USART1->DR = *TxBuf;
      i_tx_uart--;
    }
    else
    {
      USART1->CR1 &= ~USART_CR1_TXEIE;
    }
  }
}
void SendUART1Data(uint8_t TxBytes, uint8_t *Buf)
{
  i_tx_uart=TxBytes;
  TxBuf=Buf;
  USART1->SR &= ~USART_SR_TC;
  USART1->DR = *TxBuf;
  i_tx_uart--;
  USART1->CR1 |= USART_CR1_TXEIE;//EN INT DRE
}

void Sleep(unsigned int i)
{
	unsigned int j;
	for(j=0;j<i;j++){}
}
int main(void)
{

	uint8_t RxBuf[30]={0xff,0xff,0xff,0xff,0xff,0xff,0xFF,0xFF,0xFF,0xFF};
	uint8_t size,addr;

	uint16_t co2,tvos;
	uint32_t res;

	I2C1_Init_master();
	InitKeyAll();
	InitUART();

	uint8_t TxBufUART[30]={0,1,2,3,4,5,6,7,8,9};
	uint8_t TxBuf[30]={0xf7,0xf8,2,3};
	uint8_t TxBytes=9;




	//I2C1_Write(I2C_ADDRESS_IAQ, TxBuf, size);
	//I2C1_Read(I2C_ADDRESS_IAQ, RxBuf, size);


	uint8_t regadr=0xF7;
	addr=0x76;
	size=9;



	while(1)
    {

		Sleep(100000);
		//for(j=0;j<100000;j++){}

		//SendUART1Data(TxBytes, RxBuf);
		GPIOC->BSRR = GPIO_BSRR_BS9;
		/*
		I2C1_Write(addr,TxBuf, 1);
		for(j=0;j<1000;j++){}
		I2C1_Read(addr, RxBuf, 6);
		*/


		//Чтение калибровочных регистров
		regadr=0x88;
		I2C1_Write(addr,&regadr, 1);
		Sleep(1000);
		I2C1_Read(addr, RxBuf, 26);
		Sleep(1000);
		SendUART1Data(26, RxBuf);
		Sleep(10000);

		//Настройка
		regadr=0xF4;
		TxBuf[0]=regadr;
		TxBuf[1]=0xff;//0x27;
		I2C1_Write(addr,TxBuf, 2);
		Sleep(1000);





		//Чтение temp  press
		regadr=0xF4;
		I2C1_Write(addr,&regadr, 1);
		Sleep(1000);
		I2C1_Read(addr, RxBuf, 9);
		Sleep(1000);
		SendUART1Data(9, RxBuf);

		GPIOC->BRR = GPIO_BRR_BR9;
		Sleep(100000);
		//for(j=0;j<100000;j++){}



		//co2=(RxBuf[0]<<8)+RxBuf[1];
		//res=(RxBuf[4]<<16)+(RxBuf[5]<<8)+RxBuf[6];
		//tvos=(RxBuf[7]<<8)+RxBuf[8];

    }
}
