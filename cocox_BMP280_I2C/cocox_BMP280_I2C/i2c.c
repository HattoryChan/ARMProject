#include "i2c.h"

uint8_t i_tx=0;
uint8_t i_rx=0;
uint8_t	*Rx_i2c,*Tx_i2c;
uint8_t I2C_addr;


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


void I2C1_ER_IRQHandler(void)
{
	/*
	if (I2C1->SR1&I2C_SR1_AF)
	{
		I2C1->CR1|=I2C_CR1_STOP;//Generate stop
		I2C1->SR1&=~I2C_SR1_AF;
	}
*/
}

void I2C1_EV_IRQHandler(void)
{
	 uint32_t status1 = I2C1->SR1;
	 uint32_t status2 = I2C1->SR2;

	 if( status1 & I2C_SR1_SB ) //start бит сгенерирован
	 {
		 //Отправляем адрес+W/R
		 I2C1->DR = I2C_addr;
	 }
	 else if(status1&I2C_SR1_ADDR) //Адрес отправлен
	 {
		if (I2C_addr&I2C_MODE_READ) //R - Режим приема данных
		{
			*Rx_i2c = I2C1->DR;//Cчитали адресс
			I2C1->CR2|=I2C_CR2_ITBUFEN;//Enable TX RX EVENT;
			//Если ожидается прием только одного байта
			if(i_rx==1)
			{
				I2C1->CR1&=~I2C_CR1_ACK;// Acknowledge disable
				I2C1->CR1|=I2C_CR1_STOP;//Generate stop
			}
		}
		else //W - Режим передачи данных
		{

			I2C1->DR = *Tx_i2c;
			Tx_i2c++;
			i_tx--;
			I2C1->CR2|=I2C_CR2_ITBUFEN;//Enable TX RX EVENT;
			//if(i_tx==0)
			//{
			//	I2C1->CR1|=I2C_CR1_STOP;//Generate stop
			//}
		}
	 }
	 else if(status1&I2C_SR1_RXNE)//После адреса получаем данные
	 {
		//Пришел байт данных
		*Rx_i2c = I2C1->DR;
		i_rx--;
		Rx_i2c++;
		if (i_rx==1)//Если осталось принять 1 байт
		{
			I2C1->CR1&=~I2C_CR1_ACK;// Acknowledge disable
			I2C1->CR1|=I2C_CR1_STOP;//Generate stop
		}
		else if (i_rx==0)//Если данных больше нет
		{
			I2C1->CR1|=I2C_CR1_ACK;// Acknowledge enable
			I2C1->CR2&=~I2C_CR2_ITBUFEN;//Disable TX RX EVENT
		}
	 }

	else if(status1&I2C_SR1_TXE)//После адреса отпровляем данные
	{

			if (!i_tx)//Буфер отправки пуст
			{
				I2C1->CR1|=I2C_CR1_STOP;//Generate stop
				I2C1->CR2&=~I2C_CR2_ITBUFEN;//Disable TX RX EVENT
			}
			else
			{
				I2C1->DR = *Tx_i2c;
				Tx_i2c++;
				i_tx--;
			}


	}
	else
	{

	}
}
void I2C1_Write(uint8_t adress,uint8_t *buf, uint8_t size)
{
	i_tx=size;
	Tx_i2c=buf;
	I2C_addr=I2C_ADDRESS(adress,I2C_MODE_WRITE);
	I2C1->CR1|=I2C_CR1_ACK;// Acknowledge enable
	/* Generate I2C start pulse */
	I2C1->CR1|=I2C_CR1_START;
}
void I2C1_Read(uint8_t adress,uint8_t *buf, uint8_t size)
{
	i_rx=size;
	Rx_i2c=buf;
	I2C_addr=I2C_ADDRESS(adress,I2C_MODE_READ);
	I2C1->CR1|=I2C_CR1_ACK;// Acknowledge enable
	/* Generate I2C start pulse */
	I2C1->CR1|=I2C_CR1_START;
}
