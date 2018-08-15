//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Это программное обеспечение распространяется свободно. Вы можете размещать
// его на вашем сайте, но не забудьте указать ссылку на мой YouTube-канал 
// "Электроника в объектике" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Автор: Надыршин Руслан / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#ifndef _NRF24_H
#define _NRF24_H

#include <types.h>

// Определяем структуру для хранения настроек соединений
typedef struct
{
  uint8_t bDynPayLoad   :1;
  uint8_t PayLoadLen    :6;
  uint8_t Address[5];
} tPipeSettings;

// Модели NRF24, с которыми работает библиотека
#define NRF24_MODEL_L01         0       // nRF24L01
#define NRF24_MODEL_L01P        1       // nRF24L01+

// Модель NRF24
#define NRF24_MODEL             NRF24_MODEL_L01P

// Пины, к которым подключены служебные сигналы CSN и CE
#define NRF24_CSN_Port          GPIOB
#define NRF24_CSN_Pin           GPIO_Pin_0
#define NRF24_CE_Port           GPIOB
#define NRF24_CE_Pin            GPIO_Pin_1


#define NRF24_Tx_TimeOut        500      // Тайм-аут передачи пакета (с повторами), *0.1 мс


// Вспомогательные константы
#define NRF24_REGADDR_MASK      0x1F
#define NRF24_PipesNum          6       // Кол-во соединений (труб, pipes)

// Возможные скорости приёмо-передачи
#define NRF24_DataRate_250KBps  0
#define NRF24_DataRate_1MBps    1
#define NRF24_DataRate_2MBps    2
// Возможные выходные мощности передатчика
#define NRF24_OutPower_m18dBm   0
#define NRF24_OutPower_m12dBm   1
#define NRF24_OutPower_m6dBm    2
#define NRF24_OutPower_0dBm     3

// Маски в регистре STATUS
#define NRF24_bMAX_RT_Mask      (1 << 4)        // Флаг прерывания истечения попыток отправки.
#define NRF24_bTX_DS_Mask       (1 << 5)        // Флаг прерывания окончания отправки из TX FIFO. Устанавливается в 1 когда получен ACK
#define NRF24_bRX_DR_Mask       (1 << 6)        // Флаг прерывания получения пакета. Устанавливается в 1 когда в RX FIFO был получен пакет
// Маски в регистре FIFO_STATUS
#define NRF24_bRX_EMPTY_Mask    (1 << 0)        // Флаг опустошения RX FIFO
#define NRF24_bRX_FULL_Mask     (1 << 1)        // Флаг заполнения RX FIFO
#define NRF24_bTX_EMPTY_Mask    (1 << 4)        // Флаг опустошения TX FIFO
#define NRF24_bTX_FULL_Mask     (1 << 5)        // Флаг заполнения TX FIFO
#define NRF24_bTX_REUSE_Mask    (1 << 6)        // Reuse last transmitted data packet if set high. 
                                                // The packet is repeatedly retransmitted as long as CE is high.
                                                // TX_REUSE is set by the SPI command REUSE_TX_PL, and is reset by
                                                // the SPI commands W_TX_PAYLOAD or FLUSH TX

// Набор команд nRF24
#define NRF24_CMD_R_REGISTER    0x00    // Прочитать внутренние регистры nRF
#define NRF24_CMD_W_REGISTER    0x20
#define NRF24_CMD_ACTIVATE      0x50
#define NRF24_CMD_R_RX_PL_WID   0x60
#define NRF24_CMD_R_RX_PAYLOAD  0x61
#define NRF24_CMD_W_TX_PAYLOAD  0xA0
#define NRF24_CMD_W_ACK_PAYLOAD 0xA8
#define NRF24_CMD_FLUSH_TX      0xE1
#define NRF24_CMD_FLUSH_RX      0xE2
#define NRF24_CMD_REUSE_TX_PL   0xE3
#define NRF24_CMD_NOP           0xFF    // Игнорируемая команда (используется чтобы прочитать статус nRF24)

// Адреса внутренних регистров
#define NRF24_REG_CONFIG        0x00
#define NRF24_REG_EN_AA         0x01
#define NRF24_REG_EN_RXADDR     0x02
#define NRF24_REG_SETUP_AW      0x03
#define NRF24_REG_SETUP_RETR    0x04
#define NRF24_REG_RF_CH         0x05
#define NRF24_REG_RF_SETUP      0x06
#define NRF24_REG_STATUS        0x07
#define NRF24_REG_OBSERVE_TX    0x08
#define NRF24_REG_CD            0x09    // В модели nRF24L01+ этот регистр RPD (Received Power Detector)
#define NRF24_REG_RX_ADDR_P0    0x0A
#define NRF24_REG_RX_ADDR_P1    0x0B
#define NRF24_REG_RX_ADDR_P2    0x0C
#define NRF24_REG_RX_ADDR_P3    0x0D
#define NRF24_REG_RX_ADDR_P4    0x0E
#define NRF24_REG_RX_ADDR_P5    0x0F
#define NRF24_REG_TX_ADDR       0x10
#define NRF24_REG_RX_PW_P0      0x11
#define NRF24_REG_RX_PW_P1      0x12
#define NRF24_REG_RX_PW_P2      0x13
#define NRF24_REG_RX_PW_P3      0x14
#define NRF24_REG_RX_PW_P4      0x15
#define NRF24_REG_RX_PW_P5      0x16
#define NRF24_REG_FIFO_STATUS   0x17
#define NRF24_REG_DYNPD	        0x1C
#define NRF24_REG_FEATURE	0x1D


typedef struct 
{
  uint8_t bPRIM_RX      :1;     // 00 Режим работы (0 = PTX; 1 = PRX)   
  uint8_t bPWR_UP       :1;     // 01 Режим электропитания (1 = Power Up, 0 = Power Down)
  uint8_t bCRC_O        :1;     // 02 Размер поля CRC (0 = 1 байт, 1 = 2 байта)  
  uint8_t bEN_CRC       :1;     // 03 Enable CRC    
  uint8_t bMASK_MAX_RT  :1;     // 04 Прерывание по событию MAX_RT на ножке IRQ (0 = Включено; 1 = Отключено)
  uint8_t bMASK_TX_DS   :1;     // 05 Прерывание по событию TX_DS на ножке IRQ (0 = Включено; 1 = Отключено)   
  uint8_t bMASK_RX_DR   :1;     // 06 Прерывание по событию RX_DR на ножке IRQ (0 = Включено; 1 = Отключено)   
  uint8_t bRes          :1;     // 07 Reserved
} tNrf24Reg00h; // CONFIG       Configuration Register

typedef struct 
{
  uint8_t bARC          :4;     // 00-03 Кол-во попыток передачи пакета при отсутствии ACK от приёмника
  uint8_t bARD          :4;     // 04-07 Пауза между попытками ((N + 1) * 250мкс)
} tNrf24Reg04h; // SETUP_RETR   Setup of Automatic Retransmission

typedef struct 
{
  uint8_t bLNA_HCURR    :1;     // 00 Коэффициент усиления приёмника
  uint8_t bRF_PWR       :2;     // 01-02 Мощность передатчика (0 = -18dBm; 1 = -12dBm; 2 = -6dBm; 3 = 0dBm)
  uint8_t bRF_DR_HIGH   :1;     // 03 Частота передачи (старший бит). 00 = 1Mbps; 10 = 2Mbps; 01 = 250Kbps
  uint8_t bPLL_LOCK     :1;     // 04 Force PLL lock signal. Only used in test
  uint8_t bRF_DR_LOW    :1;     // 05 Частота передачи (младший бит). 00 = 1Mbps; 10 = 2Mbps; 01 = 250Kbps
  uint8_t bRes          :1;     // 06 Reserved
  uint8_t bCONT_WAVE    :1;     // 07 Enable continuous carrier transmit
} tNrf24Reg06h; // RF_SETUP     RF Setup Register

typedef struct 
{
  uint8_t bARC_CNT      :4;     // 00-03 Счётчик повторов отправки пакета. Счётчик сбрасывается когда начинается отправка нового пакета.
  uint8_t bPLOS_CNT     :4;     // 04-07 Счётчик потерянных пакетов (не превышает 15). Счётчик сбрасывается записью в RF_CH.
} tNrf24Reg08h; // OBSERVE_TX   Transmit observe register

typedef struct 
{
  uint8_t bEN_DYN_ACK   :1;     // 00 Enables the W_TX_PAYLOAD_NOACK command
  uint8_t bEN_ACK_PAY   :1;     // 01 Enables Payload with ACK
  uint8_t bEN_DPL       :1;     // 02 Enables Dynamic Payload Length
  uint8_t bRes          :5;     // 03-07 Reserved
} tNrf24Reg1Dh; // FEATURE      R/W Feature Register




// Инициализация nRF24
void nrf24_init(SPI_TypeDef* SPIx, uint8_t Channel);
// Функция устанавливает № канала (0..127)
uint8_t nrf24_SetChannel(uint8_t Channel);

// Процедура устанавливает мощность передатчика. PowerLevel - мощность передатчика (0 = -18dBm; 1 = -12dBm; 2 = -6dBm; 3 = 0dBm)
void nrf24_SetOutputPower(uint8_t PowerLevel);
// Функция возвращает установленную в nRF24 мощность передатчика (0 = -18dBm; 1 = -12dBm; 2 = -6dBm; 3 = 0dBm)
uint8_t nrf24_GetOutputPower(void);

// Процедура устанавливает частоту приёмо-передачи (см. константы NRF24_DataRate_*)
void nrf24_SetDataRate(uint8_t DataRate);
// Функция читает установленную частоту (см. константы NRF24_DataRate_*)
uint8_t nrf24_getDataRate(void);

// Процедура устанавливает длину поля CRC (1 либо 2 байта). Если CRCLen = 0, то CRC отключается
void nrf24_SetCRCLen(uint8_t CRCLen);
// Функция читает установленную  длину поля CRC. Возвращает 0, если CRC отключено
uint8_t nrf24_getCRCLen(void);

// Функция читает длину очередного пакета переменной длины в буфере RX FIFO
uint8_t nrf24_getDynLen(void);

// Процедура разрешает/запрещает переменную длину глобально
void nrf24_DynPayload_OnOff(uint8_t Enable);
// Процедура Разрешает/запрещает отправку пакетов внутри ACK-пакетов.
// Попутно разрешает переменную длину пакета (необходимое условие)
void nrf24_AckPayload_OnOff(uint8_t Enable);

// Процедура настраивает параметры автоматического повтора передачи при отсутствии ACK от приёмника.
// TryCount - кол-во повторов (0..15)
// TryPeriod - период между повторами. Считается как (TryPeriod + 1) * 250мкс
void nrf24_AutoRetrasmission_Setup(uint8_t TryCount, uint8_t TryPeriod);
// Функция возвращает, есть ли несущая на текущем канале. Доступно только в режиме PRX
uint8_t nrf24_TestCarrier(void);

// Процедура включает/отключает автовыдачу ACK для соединения (pipe) с номером Pipe
void nrf24_setAutoAck(uint8_t Pipe, uint8_t Enable);

// Процедура загружает в Tx FIFO пакет для передачи в теле ACK-пакета
void nrf24_SendAckPayload(uint8_t Pipe, uint8_t *pBuff, uint8_t Len);

// Процедура настраивает параметры приёма для соединения (pipe) с номером Pipe
void nrf24_RxPipe_Setup(uint8_t Pipe, uint8_t *pAddress, uint8_t PayloadSize);

// Функция проверяет, есть ли принятые пакеты в буфере RX FIFO. Если есть - копирует в буфер *pPipe
// Возвращает длину принятого пакета. Если новых пакетов нет, то возвращает 0.
// По указателю pPipe записывается номер соединения, из которого получен пакет (0-5) 
uint8_t nrf24_Recv(uint8_t *pPipe, uint8_t *pBuff);
// Процедура отправляет пакет (передача пойдёт через соединение №0)
// Возвращает -1 в случае ошибки (не было ACK, кол-во повторов передачи истрачено)
// В случае успеха, возвращает кол-во повторов передачи
int8_t nrf24_Send(uint8_t *pAddress, uint8_t *pBuff, uint8_t Len);

// Функция читает статусный регистр nRF24
uint8_t nrf24_ReadState(void);
// Процедура сбрасывает состояние флагов прерываний
void nrf24_ResetStateFlags(uint8_t Flags);

#endif