#include "stm32f10x.h"
#include "delay.h"
#include "UsartInitAndRX_TXFUnction.h"
#include <stdio.h>

//������������� ���� ������ (������� ��� ���)
#define FLASH_KEY1 ((uint32_t)0x45670123)
#define FLASH_KEY2 ((uint32_t)0xCDEF89AB)
#define Page127  0x0801FC00

/*===============================================================================
��� ��������� ����� ���������� ������ �� ���� ������ �� � ��������� �� ������, 
��������� ��������� �� UART
Pins:
PA[9]- TX
PA[10]- RX
===============================================================================*/

//������� ������ �� ����, ������������: data - ��������� �� ������������ ������, address - ����� �� flash
//count - ���������� ������������ ����, ������ ���� ������ 2
void Internal_Flash_Write(unsigned char* data, unsigned int address, unsigned int count); 
void Internal_Flash_Erase(unsigned int pageAddress);//������� ������� ���� ��������.
void flash_erase_all_pages(void);									//������� ������� ��� ��������.
void flash_unlock(void);													//������������� ���� ������
void flash_lock();																//���������� ����
uint8_t flash_ready(void);												//������� ��������� true ����� ����� ������� ��� ������ ������.
uint32_t flash_read(uint32_t address);      			//��������� flesh ������ ����������


//���������� UART
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
// ������������� ������ 
	flash_unlock();
 
   
if(flash_ready())
	{
    //�������� �������� (127)
		Internal_Flash_Erase(0x0801FC00);
// ������ � 127-� �������� 16 ����! 
	Internal_Flash_Write((unsigned char*)"1234566543212342",Page127,16);
    }
	else 
	{
		USART1_Send_String("ERROR FLASH READY\r\n\r\n");
		USART1_Send_String("\r\n\r\n");
	}
   //���������� ������   
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
  
//   = (unsigned char*)in_u8Value; //data - ��������� �� ������������ ������
   unsigned int address = Page127;     //address - ����� �� flash
   unsigned int count = 2;                           //count - ���������� ������������ ����, ������ ���� ������ 2
   
   // ������������� ������
   FLASH->KEYR = FLASH_KEY1;   // ((uint32_t)0x45670123)
   FLASH->KEYR = FLASH_KEY2;   // ((uint32_t)0xCDEF89AB)
      
    if(!(FLASH->SR & FLASH_SR_BSY))     //���������� true ����� ����� ������� ��� ������ ������.
	{
   //������� �������� ����� ����� � ��� ������ 
   while (FLASH->SR & FLASH_SR_BSY);      //���� ���� ������ ����� ������
	if (FLASH->SR & FLASH_SR_EOP) {        //���� �������� ��������� �������
		FLASH->SR = FLASH_SR_EOP;   //���������� ����. �������� ���������, ������� ������� "1" 
	}

	FLASH->CR |= FLASH_CR_PER;      //������������� ��� �������� ����� ��������
	FLASH->AR = address;        // ������ � �����
	FLASH->CR |= FLASH_CR_STRT;     // ��������� ��������
	while (!(FLASH->SR & FLASH_SR_EOP));  //���� ���� �������� ��������.
	FLASH->SR = FLASH_SR_EOP;            //��� ��������� �������� � ������(��� ������ ����� "1")
	FLASH->CR &= ~FLASH_CR_PER;      //���������� ��� �������
   
//  Internal_Flash_Write((unsigned char*)"1234566543212342",Page127,16);
   
   
   
   //����� � ��������   
      while (FLASH->SR & FLASH_SR_BSY);    //���� ���� ������ ����� ������
	if (FLASH->SR & FLASH_SR_EOP) {        //���� �������� ��������� �������
		FLASH->SR = FLASH_SR_EOP;           //���������� ����. �������� ���������, ������� ������� "1" 
	}

	FLASH->CR |= FLASH_CR_PG;              //���� ���� ��� ���������� ��� ��������� ������ �� ����

	for (unsigned int i = 0; i < count; i += 2) 
   {
	*(volatile unsigned short*)(address + i) = (((unsigned short)data[i + 1]) << 8) + data[i];   //����� � ������
  
   
		while (!(FLASH->SR & FLASH_SR_EOP));         //���� ��������� ��������
		FLASH->SR = FLASH_SR_EOP;            //���������� ����. �������� ���������, ������� ������� "1" 
//	}

	FLASH->CR &= ~(FLASH_CR_PG);            //���� ���� ��� ���������� ��� ��������� ������ �� ����, ���������� ���
   }
   }
   
   //���������� ������   
   FLASH->CR |= FLASH_CR_LOCK;             //���������� "1" - ��������� ������ �� ������ �� ���� ������  
	
   
	while(1)
	{		
	
	}
}

//������������� ���� ������ ����� �������
void flash_unlock(void) 
{
  FLASH->KEYR = FLASH_KEY1;   // ((uint32_t)0x45670123)
  FLASH->KEYR = FLASH_KEY2;   // ((uint32_t)0xCDEF89AB)
}

//���������� ������ ��  ����
void flash_lock() 
{
  FLASH->CR |= FLASH_CR_LOCK;    //���������� "1" - ��������� ������ �� ������ �� ���� ������
}

//������� ���������� true ����� ����� ������� ��� ������ ������.
uint8_t flash_ready(void) 
{
  return !(FLASH->SR & FLASH_SR_BSY);  //���� ���� ������ ����� ������
}
 
//������� ������� ��� ��������. ��� � ������ �������� ���������������� 
void flash_erase_all_pages(void) 
{
  FLASH->CR |= FLASH_CR_MER; //������������� ��� �������� ���� �������
  FLASH->CR |= FLASH_CR_STRT; //������ ��������
  while(!flash_ready()) // �������� ����������.. ���� ��� ��� �������� �� � ���� �����...
    ;
  FLASH->CR &= FLASH_CR_MER;     //��� �������� ���� �������
}
 
//������� ������� ���� ��������. � �������� ������ ����� ������������ �����
//������������� ��������� ������� ��� �������� ������� ����� ��������.
//pageAddress - ����� �����, ������������� ��������� ��������
void Internal_Flash_Erase(unsigned int pageAddress) 
{
	while (FLASH->SR & FLASH_SR_BSY);      //���� ���� ������ ����� ������
	if (FLASH->SR & FLASH_SR_EOP) {        //���� �������� ��������� �������
		FLASH->SR = FLASH_SR_EOP;   //���������� ����. �������� ���������, ������� ������� "1" 
	}

	FLASH->CR |= FLASH_CR_PER;      //������������� ��� �������� ����� ��������
	FLASH->AR = pageAddress;        // ������ � �����
	FLASH->CR |= FLASH_CR_STRT;     // ��������� ��������
	while (!(FLASH->SR & FLASH_SR_EOP));  //���� ���� �������� ��������.
	FLASH->SR = FLASH_SR_EOP;            //��� ��������� �������� � ������(��� ������ ����� "1")
	FLASH->CR &= ~FLASH_CR_PER;      //���������� ��� �������
}

//������� ������ �� ����
//data - ��������� �� ������������ ������
//address - ����� �� flash
//count - ���������� ������������ ����, ������ ���� ������ 2
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

//��������� flesh ������ ����������
uint32_t flash_read(uint32_t address) 
{
  return (*(__IO uint32_t*) address);   
}


