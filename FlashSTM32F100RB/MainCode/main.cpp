#include "stm32f10x.h"
#include "delay.h"
#include "UsartInitAndRX_TXFUnction.h"
#include <stdio.h>

//Разблокировка флеш памяти (дефайны для нее)
#define FLASH_KEY1 ((uint32_t)0x45670123)
#define FLASH_KEY2 ((uint32_t)0xCDEF89AB)
#define Page127  0x0801FC00

/*===============================================================================
Эта программа будет записывать данные во флеш память мк и считывать их оттуда, 
передавая результат по UART
Pins:
PA[9]- TX
PA[10]- RX
===============================================================================*/

//Функция записи во флеш, записываются: data - указатель на записываемые данные, address - адрес во flash
//count - количество записываемых байт, должно быть кратно 2
void Internal_Flash_Write(unsigned char* data, unsigned int address, unsigned int count); 
void Internal_Flash_Erase(unsigned int pageAddress);//Функция стирает одну страницу.
void flash_erase_all_pages(void);									//Функция стирает ВСЕ страницы.
void flash_unlock(void);													//Разблокировка флеш памяти
void flash_lock();																//Блокировка флеш
uint8_t flash_ready(void);												//Функция возврщает true когда можно стирать или писать память.
uint32_t flash_read(uint32_t address);      			//Прочитать flesh память программно


//Прерывания UART
void USART1_IRQHandler(void)
{
	if (USART1->SR & USART_SR_RXNE)				//what causind interruption, if byte went to RxD - treats
	{	
		  USART1->SR &= ~USART_SR_RXNE;					//Reset Interription 
	}		
}

int main(void)	
{	
	InitUART();
	/*===================================================================================
// Разблокировка памяти 
	flash_unlock();
 
   
if(flash_ready())
	{
    //Стирание страницы (127)
		Internal_Flash_Erase(0x0801FC00);
// Запись в 127-ю страницу 16 байт! 
	Internal_Flash_Write((unsigned char*)"1234566543212342",Page127,16);
    }
	else 
	{
		USART1_Send_String("ERROR FLASH READY\r\n\r\n");
		USART1_Send_String("\r\n\r\n");
	}
   //Блокировка памяти   
   flash_lock();   
   uint16_t Temp = flash_read(0x0801FC00);
   USART1_Send_String((char*)Temp);
   

  for(int i = 0; i<=15; i++)
    {
       	USART1_Send((u16)flash_read(0x0801FC00 + i));      
    		delayms(5);
    }
    USART1_Send_String("\r\n\r\n");
 ==========================================================================================  */
 uint16_t in_u16Value = 9;
  char Temp[1];
  sprintf(Temp,"%d",in_u16Value);
   
   uint16_t in_pPtr = 6;
   unsigned char* data = (unsigned char*)Temp;
  
//   = (unsigned char*)in_u8Value; //data - указатель на записываемые данные
   unsigned int address = Page127;     //address - адрес во flash
   unsigned int count = 2;                           //count - количество записываемых байт, должно быть кратно 2
   
   // Разблокировка памяти
   FLASH->KEYR = FLASH_KEY1;   // ((uint32_t)0x45670123)
   FLASH->KEYR = FLASH_KEY2;   // ((uint32_t)0xCDEF89AB)
      
    if(!(FLASH->SR & FLASH_SR_BSY))     //возвращает true когда можно стирать или писать память.
	{
   //Стираем страницу чтобы потом в нее писать 
   while (FLASH->SR & FLASH_SR_BSY);      //Ждем пока память будет готова
	if (FLASH->SR & FLASH_SR_EOP) {        //Если операция завершена успешно
		FLASH->SR = FLASH_SR_EOP;   //Сбрасываем флаг. Ставится аппаратно, снимаем записав "1" 
	}

	FLASH->CR |= FLASH_CR_PER;      //Устанавливаем бит стирания одной страницы
	FLASH->AR = address;        // Задаем её адрес
	FLASH->CR |= FLASH_CR_STRT;     // Запускаем стирание
	while (!(FLASH->SR & FLASH_SR_EOP));  //Ждем пока страница сотрется.
	FLASH->SR = FLASH_SR_EOP;            //Бит окончания стирания и записи(для сброса пишем "1")
	FLASH->CR &= ~FLASH_CR_PER;      //Сбрасываем бит обратно
   
//  Internal_Flash_Write((unsigned char*)"1234566543212342",Page127,16);
   
   
   
   //Пишем в страницу   
      while (FLASH->SR & FLASH_SR_BSY);    //Ждем пока память будет готова
	if (FLASH->SR & FLASH_SR_EOP) {        //Если операция завершена успешно
		FLASH->SR = FLASH_SR_EOP;           //Сбрасываем флаг. Ставится аппаратно, снимаем записав "1" 
	}

	FLASH->CR |= FLASH_CR_PG;              //пока этот бит установлен нам разрешено писать во флеш

	for (unsigned int i = 0; i < count; i += 2) 
   {
	*(volatile unsigned short*)(address + i) = (((unsigned short)data[i + 1]) << 8) + data[i];   //Пишем в память
  
   
		while (!(FLASH->SR & FLASH_SR_EOP));         //Ждем окончания действия
		FLASH->SR = FLASH_SR_EOP;            //Сбрасываем флаг. Ставится аппаратно, снимаем записав "1" 
//	}

	FLASH->CR &= ~(FLASH_CR_PG);            //пока этот бит установлен нам разрешено писать во флеш, сбрасываем его
   }
   }
   
   //Блокировка памяти   
   FLASH->CR |= FLASH_CR_LOCK;             //Записываем "1" - блокируем доступ на запись во флеш память  
	
   
	while(1)
	{		
	
	}
}

//Разблокировка флеш памяти двумя ключами
void flash_unlock(void) 
{
  FLASH->KEYR = FLASH_KEY1;   // ((uint32_t)0x45670123)
  FLASH->KEYR = FLASH_KEY2;   // ((uint32_t)0xCDEF89AB)
}

//Блокировка записи во  флеш
void flash_lock() 
{
  FLASH->CR |= FLASH_CR_LOCK;    //Записываем "1" - блокируем доступ на запись во флеш память
}

//Функция возвращает true когда можно стирать или писать память.
uint8_t flash_ready(void) 
{
  return !(FLASH->SR & FLASH_SR_BSY);  //Ждем пока память будет готова
}
 
//Функция стирает ВСЕ страницы. При её вызове прошивка самоуничтожается 
void flash_erase_all_pages(void) 
{
  FLASH->CR |= FLASH_CR_MER; //Устанавливаем бит стирания ВСЕХ страниц
  FLASH->CR |= FLASH_CR_STRT; //Начать стирание
  while(!flash_ready()) // Ожидание готовности.. Хотя оно уже наверное ни к чему здесь...
    ;
  FLASH->CR &= FLASH_CR_MER;     //Бит стирания всех страниц
}
 
//Функция стирает одну страницу. В качестве адреса можно использовать любой
//принадлежащий диапазону адресов той странице которую нужно очистить.
//pageAddress - любой адрес, принадлежащий стираемой странице
void Internal_Flash_Erase(unsigned int pageAddress) 
{
	while (FLASH->SR & FLASH_SR_BSY);      //Ждем пока память будет готова
	if (FLASH->SR & FLASH_SR_EOP) {        //Если операция завершена успешно
		FLASH->SR = FLASH_SR_EOP;   //Сбрасываем флаг. Ставится аппаратно, снимаем записав "1" 
	}

	FLASH->CR |= FLASH_CR_PER;      //Устанавливаем бит стирания одной страницы
	FLASH->AR = pageAddress;        // Задаем её адрес
	FLASH->CR |= FLASH_CR_STRT;     // Запускаем стирание
	while (!(FLASH->SR & FLASH_SR_EOP));  //Ждем пока страница сотрется.
	FLASH->SR = FLASH_SR_EOP;            //Бит окончания стирания и записи(для сброса пишем "1")
	FLASH->CR &= ~FLASH_CR_PER;      //Сбрасываем бит обратно
}

//Функция записи во флеш
//data - указатель на записываемые данные
//address - адрес во flash
//count - количество записываемых байт, должно быть кратно 2
void Internal_Flash_Write(unsigned char* data, unsigned int address, unsigned int count) 
{
	unsigned int i;

	while (FLASH->SR & FLASH_SR_BSY);
	if (FLASH->SR & FLASH_SR_EOP) 
   {
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR |= FLASH_CR_PG;

	for (i = 0; i < count; i += 2)
   {
		*(volatile unsigned short*)(address + i) = (((unsigned short)data[i + 1]) << 8) + data[i];
		while (!(FLASH->SR & FLASH_SR_EOP));
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR &= ~(FLASH_CR_PG);
}

//Прочитать flesh память программно
uint32_t flash_read(uint32_t address) 
{
  return (*(__IO uint32_t*) address);   
}


