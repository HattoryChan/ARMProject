#include "stm32f10x.h"
#include "stm32f10x_i2c.h"

#define I2C_MODE_READ	1
#define I2C_MODE_WRITE	0
#define I2C_ADDRESS(addr, mode)	((addr<<1) | mode)
#define I2C_ADDRESS_IAQ 	0b1000000	//0b1011010

void I2C1_Init_master(void)
{
	// Включение тактирования
	// Включить тактирование альтернативных функций
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	// Включить тактирование I2C
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	// Включить тактирование порта B
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

	// Настройка выводов согласно выполняемым функциям:
	//PB6-> SCL: альтернативная функция, 50 МГц
	GPIOB->CRL |= GPIO_CRL_MODE6;//Max. output speed 50 MHz
	GPIOB->CRL |= GPIO_CRL_CNF6;//Open-drain

	//PB7-> SDA: альтернативная функция, 50 МГц
	GPIOB->CRL |= GPIO_CRL_MODE7;//Max. output speed 50 MHz
	GPIOB->CRL |= GPIO_CRL_CNF7;//Open-drain

/*
	I2C_InitTypeDef i2c;
	i2c.I2C_ClockSpeed = 100000;
	i2c.I2C_Mode = I2C_Mode_I2C;
	i2c.I2C_DutyCycle = I2C_DutyCycle_2;
	i2c.I2C_OwnAddress1 = 0x15;
	i2c.I2C_Ack = I2C_Ack_Disable;
	i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2C1, &i2c);

/*/
	//Настройка регистров I2C

	I2C1->CR1= 0x0000; // Очистить первый управляющий регистр
	I2C1->CR1|=I2C_CR1_ACK;// Acknowledge enable


	I2C1->CR2|=24;//PCLK=24MHz
	I2C1->CR2|=I2C_CR2_ITEVTEN;// Event interrupt enable
	I2C1->CR2|=I2C_CR2_ITERREN;

	I2C1->CCR=120;//240;//PCLK_24MHz/2*I2C_SPEED_100KHz
	I2C1->TRISE=25;//1000нс*PCLK_24MHz 		1000нс-время наростания фронта из спецификации шины


	I2C1->CR2|=I2C_CR2_ITEVTEN;
	I2C1->CR1|=I2C_CR1_PE;// Peripheral enable



	NVIC_EnableIRQ(I2C1_ER_IRQn);
	NVIC_EnableIRQ(I2C1_EV_IRQn);
}



uint8_t i_tx=0;
uint8_t i_rx=0;
uint8_t	*Rx,*Tx;
uint8_t I2C_addr;
uint8_t REG;

void I2C1_ER_IRQHandler(void)
{
	if (I2C1->SR1&I2C_SR1_AF)
	{
		/* Generate stop */
		I2C1->CR1|=I2C_CR1_STOP;
		I2C1->SR1&=~I2C_SR1_AF;

	}

}

void I2C1_EV_IRQHandler(void)
{
	 uint32_t status1 = I2C1->SR1;
	 uint32_t status2 = I2C1->SR2;
	 if( status1 & I2C_SR1_SB )
	 {
		 //start бит отправлен
		 //Отправляем адрес+W/R
		 I2C1->DR = I2C_addr;
	 }
	 if(status1&I2C_SR1_ADDR)
	 {
		 //Адрес отправлен
		 if(status1&I2C_SR1_TXE)
		 {
			//W - Режим передачи данных
			I2C1->DR = *Tx;
			Tx++;
			i_tx--;
		 }
		 else
		 {
			 //R - Режим приема данных
			 //Если ожидается прием только одного байта
			 if(i_rx<2) I2C1->CR1&=~I2C_CR1_ACK;// Acknowledge disable
			 else 		I2C1->CR1|=I2C_CR1_ACK;// Acknowledge enable
		 }
	}
	else //После адреса либо получаем данные либо отпровляем
	{
		if(status1&I2C_SR1_TXE)
		{
			//Буфер отправки пуст
			if (!i_tx)
			{
				/* Generate stop */
				I2C1->CR1|=I2C_CR1_STOP;
				return;
			}
			I2C1->DR = *Tx;
			Tx++;
			i_tx--;

		}
		if(status1&I2C_SR1_RXNE)
		{//Пришел байт данных
			if(i_rx==2)I2C1->CR1&=~I2C_CR1_ACK;// Acknowledge disable
			if (i_rx==1)
			{
				I2C1->CR1|=I2C_CR1_ACK;// Acknowledge enable
				/* Generate stop */
				I2C1->CR1|=I2C_CR1_STOP;
				REG== I2C1->DR;
				return;
			}
			//*Rx = I2C1->DR;
			REG== I2C1->DR;
			i_rx--;
		}
	}

}
void I2C1_Write(uint8_t adress,uint8_t *buf, uint8_t size)
{
	i_tx=size-1;
	Tx=buf;
	I2C_addr=I2C_ADDRESS(adress,I2C_MODE_WRITE);
	/* Generate I2C start pulse */
	I2C1->CR1|=I2C_CR1_START;
}
void I2C1_Read(uint8_t adress,uint8_t *buf, uint8_t size)
{
	i_rx=size;
	Rx=buf;
	I2C_addr=I2C_ADDRESS(adress,I2C_MODE_READ);
	/* Generate I2C start pulse */
	I2C1->CR1|=I2C_CR1_START;
}
void InitKeyAll(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN|RCC_APB2ENR_IOPCEN;

  GPIOC->CRH   |= GPIO_CRH_MODE8; // ???? ?????????? LED4 PC8 ?? ?????.
  GPIOC->CRH   &=~GPIO_CRH_CNF8; //  ??????????? ????? ?? PC8.
  GPIOC->CRH   |= GPIO_CRH_MODE9; // ???? ?????????? LED3 PC9 ?? ?????.
  GPIOC->CRH   &=~GPIO_CRH_CNF9; // ??????????? ????? ?? PC9.

  GPIOA->CRL&=~GPIO_CRL_MODE0; // ?????? "USER" PA0 - ?? ????.
}
int main(void)
{
	uint8_t TxBuf[10]={0,1,2,3};
	uint8_t RxBuf[10];
	uint8_t size;

	I2C1_Init_master();


	unsigned int i,j;
	InitKeyAll();

	//I2C1_Write(I2C_ADDRESS_IAQ, TxBuf, size);
	size=9;
	//I2C1_Read(I2C_ADDRESS_IAQ, RxBuf, size);
	for(j=0;j<1000000;j++){}
	uint8_t addr=0x55;
	I2C1_Read(addr, RxBuf, size);
	addr=0x5A;

	while(1)
    {
		GPIOC->BSRR = GPIO_BSRR_BS9;
		for(j=0;j<1000000;j++){}
		I2C1_Read(addr, RxBuf, size);
		GPIOC->BRR = GPIO_BRR_BR9;
		for(j=0;j<1000000;j++){}

		/*if ((GPIOA->IDR & GPIO_IDR_IDR0))
		{
			GPIOC->BSRR = GPIO_BSRR_BS9;
			I2C1_Read(addr, RxBuf, size);
			addr++;
			for(j=0;j<1000000;j++){}

		}
		else
		{
			GPIOC->BRR = GPIO_BRR_BR9;
		}
		*/
    }
}
