#include "ff/ff.h"
#include "ff/diskio.h"
#include "stm32f10x.h"

 /*  if SD power check is used
 #define CARD_SUPPLY_SWITCHABLE   1
 #define GPIO_PWR                 GPIOC
 #define RCC_APB2Periph_GPIO_PWR  RCC_APB2Periph_GPIOC
 #define GPIO_Pin_PWR             GPIO_Pin_8
 #define GPIO_Mode_PWR            GPIO_Mode_Out_OD // pull-up resistor at power FET
 */


 #define GPIO_LED                 GPIOC

 //SD Activity led
#define RCC_APB2Periph_GPIO_ACT  RCC_APB2Periph_GPIOC
 #define GPIO_Pin_ACT             GPIO_Pin_8


//Sd OK Led
 #define RCC_APB2Periph_GPIO_LED  RCC_APB2Periph_GPIOC
 #define GPIO_Pin_LED             GPIO_Pin_9

 #define SPI_SD                   SPI2
 #define GPIO_CS                  GPIOB
 #define RCC_APB2Periph_GPIO_CS   RCC_APB2Periph_GPIOB
 #define GPIO_Pin_CS              GPIO_Pin_12
 #define GPIO_SPI_SD              GPIOB
 #define GPIO_Pin_SPI_SD_SCK      GPIO_Pin_13
 #define GPIO_Pin_SPI_SD_MISO     GPIO_Pin_14
 #define GPIO_Pin_SPI_SD_MOSI     GPIO_Pin_15
 #define RCC_APBPeriphClockCmd_SPI_SD  RCC_APB1PeriphClockCmd
 #define RCC_APBPeriph_SPI_SD     RCC_APB1Periph_SPI2

#define SPI_BaudRatePrescaler_SPI_SD  SPI_BaudRatePrescaler_2

#define CS_SELECT()        GPIO_ResetBits(GPIO_CS, GPIO_Pin_CS)    /* MMC CS = L */
#define CS_DESELECT()      GPIO_SetBits(GPIO_CS, GPIO_Pin_CS)      /* MMC CS = H */

#define SD_LED_OFF()        GPIO_ResetBits(GPIO_LED, GPIO_Pin_LED)
#define SD_LED_ON()      GPIO_SetBits(GPIO_LED, GPIO_Pin_LED)

#define SD_ACT_OFF()        GPIO_ResetBits(GPIO_LED, GPIO_Pin_ACT)
#define SD_ACT_ON()         GPIO_SetBits(GPIO_LED, GPIO_Pin_ACT)

#define GO_IDLE_STATE			0
#define SEND_OP_COND			1
#define SEND_IF_COND			8
#define SET_BLOCKLEN			16
#define READ_BLOCK				17
#define WRITE_BLOCK				24
#define APP_CMD					55
#define READ_OCR				58
#define SD_SEND_OP_COND			(0x40|41)


typedef enum card_type_id {
	NO_CARD,
	CARD_MMC,
	CARD_SD,
	CARD_SD2,
	CARD_SDHC
} card_type_id_t;

uint8_t mmc_mount();

void Delay_ms(uint32_t ms);
