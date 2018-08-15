/*

Header for Periphery_Init.

   Port A:
   [9,10] - Usart1(TX,RX)

*/

#define  F_CPU 24000000L

//============------------====== Initiall GpioA, Usart
void InitPeriphery(uint32_t in_u32BaudRate);

void USART1_Send(char chr);											//Send one chr sunction
void USART1_Send_String(char* str);									//Send string function
