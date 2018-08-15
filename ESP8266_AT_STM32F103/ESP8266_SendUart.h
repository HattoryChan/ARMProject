/*
This library will be Send and Get byte message from TX/RX, you need create Usart1_IRQHandler
   PA[9]- TX
   PA[10]- RX
*/


void Init_STM32ToESP8266Uart(); //Initiall GpioA, Usart
void ESP8266_Send(char chr); //Send TX One Char Function
void ESP8266_SendStr(char* str); //Send TX string Function
