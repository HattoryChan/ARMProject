#include <stm32f4xx.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_spi.h>
//#include <stdint.h>
//#include <stdbool.h>
//#include <string.h>
#include <nrf24l01p.h>


typedef void (*RCC_APBPeriphClockCmd)(uint32_t RCC_APBPeriph, FunctionalState NewState);

typedef struct
{
    RCC_APBPeriphClockCmd clock_cmd;
    uint32_t rcc;
} RCC_Periphery;

#define RCC_PERIPHERY(bus,periph) { RCC_##bus##PeriphClockCmd, RCC_##bus##Periph_##periph }

typedef struct
{
    RCC_Periphery *rcc_periphery;
    GPIO_TypeDef *gpio;
    uint16_t pinmask;
    uint8_t pinsource;
} GPIO_Pin;

#define GPIO_PIN(gpio,pin_num) { &gpio##_RCC, gpio, GPIO_Pin_##pin_num, GPIO_PinSource##pin_num }

typedef struct
{
    RCC_Periphery *rcc_periphery;
    USART_TypeDef *periphery;
    uint8_t gpio_af;
} USART;

typedef struct
{
    RCC_Periphery *rcc_periphery;
    SPI_TypeDef *periphery;
    uint8_t gpio_af;
} SPI;


RCC_Periphery
    GPIOA_RCC =  RCC_PERIPHERY(AHB1, GPIOA),
    GPIOB_RCC =  RCC_PERIPHERY(AHB1, GPIOB),
    GPIOC_RCC =  RCC_PERIPHERY(AHB1, GPIOC),
    GPIOD_RCC =  RCC_PERIPHERY(AHB1, GPIOD),

    USART1_RCC = RCC_PERIPHERY(APB2, USART1),
    SPI1_RCC =   RCC_PERIPHERY(APB2, SPI1);

GPIO_Pin
    BLUE_LED =  GPIO_PIN(GPIOD, 15),
    GREEN_LED = GPIO_PIN(GPIOD, 12);

GPIO_Pin
    NRF_CE =    GPIO_PIN(GPIOA, 3),
    NRF_IRQ =   GPIO_PIN(GPIOA, 2),
    NRF_POWER = GPIO_PIN(GPIOA, 1);

SPI NRF_SPI = { &SPI1_RCC, SPI1, GPIO_AF_SPI1 };
GPIO_Pin
    NRF_SPI_NSS =  GPIO_PIN(GPIOA, 4),
    NRF_SPI_SCK =  GPIO_PIN(GPIOA, 5),
    NRF_SPI_MISO = GPIO_PIN(GPIOA, 6),
    NRF_SPI_MOSI = GPIO_PIN(GPIOA, 7);

GPIO_Pin BUTTON = GPIO_PIN(GPIOA, 0);

volatile bool send_flag = 0;
volatile int delay_systicks = 0;

void leds_init(void);

void nrf_gpio_init(void);
void nrf_spi_init(void);
void nrf_init(uint8_t *address);
void nrf_power(bool on);

void init_pin(GPIO_Pin *pin, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed);
void delay(int milliseconds);

enum
{
    NRF_CHANNEL = 120,
    NRF_POWER_UP_DELAY = 2,
    NRF_PAYLOAD_LENGTH = 10
};



void init_system()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);


	// Инициализируем порты диодов
    leds_init();

    // Инициализируем порты радиоприемоперидачика
    nrf_gpio_init();
    nrf_spi_init();
    nrf24l01p_spi_ss(NRF24L01P_SPI_SS_HIGH);

    // Указываем устройство
    uint8_t address[5] = { 0xE2, 0xE4, 0x23, 0xE4, 0x78 };
    nrf_init(address);

    // сбрасываем ноги
    GPIO_ResetBits(NRF_CE.gpio, NRF_CE.pinmask);

    // инициализируем порт кнопки
    init_pin(&BUTTON, GPIO_Mode_IN, GPIO_Speed_2MHz);

}


int main(void)
{
    SysTick_Config(SystemCoreClock / 1000);

    init_system();

    uint8_t btn_state = RESET;

    while (1)
    {

            if (send_flag)
            {
                send_flag = 0;

                // Зажигаем голубой диод
                GPIO_SetBits(BLUE_LED.gpio, BLUE_LED.pinmask);

                // готовим груз
                static uint8_t payload[NRF_PAYLOAD_LENGTH];
                memset(payload, 0, sizeof(payload));
                // грузом будет импульс приняты от кнопки
                //payload[0] = GPIO_ReadInputDataBit(BUTTON.gpio, BUTTON.pinmask);

                //btn_state = GPIO_ReadInputDataBit(BUTTON.gpio, BUTTON.pinmask);
                //if (btn_state != RESET)
                //{
                	// Зажигаем зеленый диод
                	//GPIO_WriteBit(GREEN_LED.gpio, GREEN_LED.pinmask, btn_state);
                //}
                //payload[0] = btn_state;

                payload[0] = 1;
                payload[1] = 2;
                payload[2] = 3;
                payload[3] = 4;
                payload[4] = 5;
                payload[5] = 4;
                payload[6] = 3;
                payload[7] = 2;
                payload[8] = 1;
                payload[9] = 0;



                // отправляем
                nrf24l01p_write_tx_payload(payload, sizeof(payload));

                GPIO_SetBits(NRF_CE.gpio, NRF_CE.pinmask);
                delay(1);
                GPIO_ResetBits(NRF_CE.gpio, NRF_CE.pinmask);

                // ждем ответ от мастера
                do {} while (!(nrf24l01p_get_irq_flags() & (1 << NRF24L01P_IRQ_TX_DS)));
                nrf24l01p_clear_irq_flag(NRF24L01P_IRQ_TX_DS);

                // Тушим голубой диод
                GPIO_ResetBits(BLUE_LED.gpio, BLUE_LED.pinmask);
            }

    }

    do {} while (1);
}








void SysTick_Handler(void)
{
    ++delay_systicks;

    static unsigned send_systicks = 0;

    if ((++send_systicks % 100) == 0)
        send_flag = true;
}

void leds_init(void)
{
    init_pin(&BLUE_LED, GPIO_Mode_OUT, GPIO_Speed_2MHz);
    init_pin(&GREEN_LED, GPIO_Mode_OUT, GPIO_Speed_2MHz);

    GPIO_ResetBits(BLUE_LED.gpio, BLUE_LED.pinmask);
    GPIO_ResetBits(GREEN_LED.gpio, GREEN_LED.pinmask);
}

void nrf_gpio_init(void)
{
    init_pin(&NRF_CE, GPIO_Mode_OUT, GPIO_Speed_2MHz);
    init_pin(&NRF_IRQ, GPIO_Mode_IN, GPIO_Speed_2MHz);
    init_pin(&NRF_POWER, GPIO_Mode_OUT, GPIO_Speed_2MHz);
}

void nrf_spi_init(void)
{
    init_pin(&NRF_SPI_NSS, GPIO_Mode_OUT, GPIO_Speed_50MHz);
    init_pin(&NRF_SPI_SCK, GPIO_Mode_AF, GPIO_Speed_50MHz);
    init_pin(&NRF_SPI_MISO, GPIO_Mode_AF, GPIO_Speed_50MHz);
    init_pin(&NRF_SPI_MOSI, GPIO_Mode_AF, GPIO_Speed_50MHz);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

    NRF_SPI.rcc_periphery->clock_cmd(NRF_SPI.rcc_periphery->rcc, ENABLE);
    SPI_InitTypeDef spi_cfg;
    SPI_StructInit(&spi_cfg);
    spi_cfg.SPI_Mode = SPI_Mode_Master;
    spi_cfg.SPI_NSS = SPI_NSS_Soft;
    spi_cfg.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
    SPI_Init(NRF_SPI.periphery, &spi_cfg);
    SPI_Cmd(NRF_SPI.periphery, ENABLE);
}

void nrf_init(uint8_t *address)
{
    nrf_power(false);
    delay(1);
    nrf_power(true);
    delay(100);

    nrf24l01p_get_clear_irq_flags();
    nrf24l01p_close_pipe(NRF24L01P_ALL);
    nrf24l01p_open_pipe(NRF24L01P_TX, false);
    nrf24l01p_set_crc_mode(NRF24L01P_CRC_16BIT);
    nrf24l01p_set_address_width(NRF24L01P_AW_5BYTES);
    nrf24l01p_set_address(NRF24L01P_TX, address);
    nrf24l01p_set_operation_mode(NRF24L01P_PTX);
    nrf24l01p_set_rf_channel(NRF_CHANNEL);

    nrf24l01p_set_power_mode(NRF24L01P_PWR_UP);
    delay(NRF_POWER_UP_DELAY);
}

void nrf_power(bool on)
{
    GPIO_WriteBit(NRF_POWER.gpio, NRF_POWER.pinmask, on ? Bit_SET : Bit_RESET);
}

void init_pin(GPIO_Pin *pin, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed)
{
    pin->rcc_periphery->clock_cmd(pin->rcc_periphery->rcc, ENABLE);

    GPIO_InitTypeDef gpio_config;
    GPIO_StructInit(&gpio_config);
    gpio_config.GPIO_Pin = pin->pinmask;
    gpio_config.GPIO_Speed = speed;
    gpio_config.GPIO_Mode = mode;
    GPIO_Init(pin->gpio, &gpio_config);
}

void delay(int milliseconds)
{
    SysTick->VAL = SysTick->LOAD;
    delay_systicks = 0;
    do {} while (delay_systicks < milliseconds);
}

void nrf24l01p_spi_ss(nrf24l01p_spi_ss_level_t level)
{
    GPIO_WriteBit(NRF_SPI_NSS.gpio, NRF_SPI_NSS.pinmask,
        (level == NRF24L01P_SPI_SS_LOW ? Bit_RESET : Bit_SET));
}

uint8_t nrf24l01p_spi_rw(uint8_t value)
{
    SPI_I2S_SendData(SPI1, value);
    do {} while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

    return SPI_I2S_ReceiveData(SPI1);
}


