 #define SPI_W5500                        SPI1
 #define GPIO_W5500_CS                    GPIOA
 #define RCC_APB2Periph_GPIO_W5500_CS     RCC_APB2Periph_GPIOA
 #define GPIO_Pin_CS_W5500                GPIO_Pin_4
 #define RCC_APBPeriphClockCmd_CS_W5500   RCC_APB2PeriphClockCmd
 #define GPIO_SPI_W5500                   GPIOA
 #define GPIO_Pin_SPI_W5500_SCK           GPIO_Pin_5
 #define GPIO_Pin_SPI_W5500_MISO          GPIO_Pin_6
 #define GPIO_Pin_SPI_W5500_MOSI          GPIO_Pin_7
 #define RCC_APBPeriphClockCmd_SPI_W5500  RCC_APB2PeriphClockCmd
 #define RCC_APBPeriph_SPI_W5500          RCC_APB2Periph_SPI1

 #define SPI_BaudRatePrescaler_SPI_SD  SPI_BaudRatePrescaler_2

 #define W5500_select() GPIO_ResetBits(GPIO_W5500_CS, GPIO_Pin_CS_W5500)
 #define W5500_release() GPIO_SetBits(GPIO_W5500_CS, GPIO_Pin_CS_W5500)

 #define W5500_rx() W5500_rxtx(0xff)
 #define W5500_tx(data) W5500_rxtx(data)

void spiW5500_init(void);

uint8_t W5500_rxtx(uint8_t data);
#include "stm32f10x.h"

void USART1_Send(uint8_t chr);
void USART1_Send_String(char* str);
