/*
This library will be Send and Get byte message from TX/RX, you need create Usart1_IRQHandler
   PA[9]- TX
   PA[10]- RX
*/


void Init_Uart(); //Initiall GpioA, Usart
void Uart_Send(char chr); //Send TX One Char Function
void Uart_SendStr(char* str); //Send TX string Function
