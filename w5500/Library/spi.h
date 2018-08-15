#include "stm32f10x.h"

void spiW5500_init(void);

uint8_t W5500_rxtx(uint8_t data);

#define W5500_select() GPIOA->BSRR = GPIO_BSRR_BR4;
#define W5500_release() GPIOA->BSRR = GPIO_BSRR_BS4;

 #define W5500_rx() W5500_rxtx(0xff)
 #define W5500_tx(data) W5500_rxtx(data)
