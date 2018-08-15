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

//===========-------------======   Preference for DHT11 leg PA[8]
void DHT11_OutLeg_Setting();  // Set leg in Output, 50Mhz mode
void DHT11_InLeg_Setting();  // Set leg in Input, push-pull mode

//===========------------=========== DHT11 leg control block
int DHT11_LegStatus();  //Status for DHT11 Leg

//===========-------------=========    TIM2 control block
void TIM2_Start();
void TIM2_Stop();
