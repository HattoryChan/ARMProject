/*
      Header to Initial_Periphery.h
*/

void USART1_Send(char chr);											//Send one chr function
void USART1_Send_String(char* str);									//Send string function
void delay(uint16_t sec);												//Delay in Second
void Initial_Periphery(void);                               //Initial ADC,Usart,RCC to port A
