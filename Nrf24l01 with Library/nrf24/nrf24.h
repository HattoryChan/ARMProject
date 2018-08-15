//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// ��� ����������� ����������� ���������������� ��������. �� ������ ���������
// ��� �� ����� �����, �� �� �������� ������� ������ �� ��� YouTube-����� 
// "����������� � ���������" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// �����: �������� ������ / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#ifndef _NRF24_H
#define _NRF24_H

#include <types.h>

// ���������� ��������� ��� �������� �������� ����������
typedef struct
{
  uint8_t bDynPayLoad   :1;
  uint8_t PayLoadLen    :6;
  uint8_t Address[5];
} tPipeSettings;

// ������ NRF24, � �������� �������� ����������
#define NRF24_MODEL_L01         0       // nRF24L01
#define NRF24_MODEL_L01P        1       // nRF24L01+

// ������ NRF24
#define NRF24_MODEL             NRF24_MODEL_L01P

// ����, � ������� ���������� ��������� ������� CSN � CE
#define NRF24_CSN_Port          GPIOB
#define NRF24_CSN_Pin           GPIO_Pin_0
#define NRF24_CE_Port           GPIOB
#define NRF24_CE_Pin            GPIO_Pin_1


#define NRF24_Tx_TimeOut        500      // ����-��� �������� ������ (� ���������), *0.1 ��


// ��������������� ���������
#define NRF24_REGADDR_MASK      0x1F
#define NRF24_PipesNum          6       // ���-�� ���������� (����, pipes)

// ��������� �������� �����-��������
#define NRF24_DataRate_250KBps  0
#define NRF24_DataRate_1MBps    1
#define NRF24_DataRate_2MBps    2
// ��������� �������� �������� �����������
#define NRF24_OutPower_m18dBm   0
#define NRF24_OutPower_m12dBm   1
#define NRF24_OutPower_m6dBm    2
#define NRF24_OutPower_0dBm     3

// ����� � �������� STATUS
#define NRF24_bMAX_RT_Mask      (1 << 4)        // ���� ���������� ��������� ������� ��������.
#define NRF24_bTX_DS_Mask       (1 << 5)        // ���� ���������� ��������� �������� �� TX FIFO. ��������������� � 1 ����� ������� ACK
#define NRF24_bRX_DR_Mask       (1 << 6)        // ���� ���������� ��������� ������. ��������������� � 1 ����� � RX FIFO ��� ������� �����
// ����� � �������� FIFO_STATUS
#define NRF24_bRX_EMPTY_Mask    (1 << 0)        // ���� ����������� RX FIFO
#define NRF24_bRX_FULL_Mask     (1 << 1)        // ���� ���������� RX FIFO
#define NRF24_bTX_EMPTY_Mask    (1 << 4)        // ���� ����������� TX FIFO
#define NRF24_bTX_FULL_Mask     (1 << 5)        // ���� ���������� TX FIFO
#define NRF24_bTX_REUSE_Mask    (1 << 6)        // Reuse last transmitted data packet if set high. 
                                                // The packet is repeatedly retransmitted as long as CE is high.
                                                // TX_REUSE is set by the SPI command REUSE_TX_PL, and is reset by
                                                // the SPI commands W_TX_PAYLOAD or FLUSH TX

// ����� ������ nRF24
#define NRF24_CMD_R_REGISTER    0x00    // ��������� ���������� �������� nRF
#define NRF24_CMD_W_REGISTER    0x20
#define NRF24_CMD_ACTIVATE      0x50
#define NRF24_CMD_R_RX_PL_WID   0x60
#define NRF24_CMD_R_RX_PAYLOAD  0x61
#define NRF24_CMD_W_TX_PAYLOAD  0xA0
#define NRF24_CMD_W_ACK_PAYLOAD 0xA8
#define NRF24_CMD_FLUSH_TX      0xE1
#define NRF24_CMD_FLUSH_RX      0xE2
#define NRF24_CMD_REUSE_TX_PL   0xE3
#define NRF24_CMD_NOP           0xFF    // ������������ ������� (������������ ����� ��������� ������ nRF24)

// ������ ���������� ���������
#define NRF24_REG_CONFIG        0x00
#define NRF24_REG_EN_AA         0x01
#define NRF24_REG_EN_RXADDR     0x02
#define NRF24_REG_SETUP_AW      0x03
#define NRF24_REG_SETUP_RETR    0x04
#define NRF24_REG_RF_CH         0x05
#define NRF24_REG_RF_SETUP      0x06
#define NRF24_REG_STATUS        0x07
#define NRF24_REG_OBSERVE_TX    0x08
#define NRF24_REG_CD            0x09    // � ������ nRF24L01+ ���� ������� RPD (Received Power Detector)
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
  uint8_t bPRIM_RX      :1;     // 00 ����� ������ (0 = PTX; 1 = PRX)   
  uint8_t bPWR_UP       :1;     // 01 ����� �������������� (1 = Power Up, 0 = Power Down)
  uint8_t bCRC_O        :1;     // 02 ������ ���� CRC (0 = 1 ����, 1 = 2 �����)  
  uint8_t bEN_CRC       :1;     // 03 Enable CRC    
  uint8_t bMASK_MAX_RT  :1;     // 04 ���������� �� ������� MAX_RT �� ����� IRQ (0 = ��������; 1 = ���������)
  uint8_t bMASK_TX_DS   :1;     // 05 ���������� �� ������� TX_DS �� ����� IRQ (0 = ��������; 1 = ���������)   
  uint8_t bMASK_RX_DR   :1;     // 06 ���������� �� ������� RX_DR �� ����� IRQ (0 = ��������; 1 = ���������)   
  uint8_t bRes          :1;     // 07 Reserved
} tNrf24Reg00h; // CONFIG       Configuration Register

typedef struct 
{
  uint8_t bARC          :4;     // 00-03 ���-�� ������� �������� ������ ��� ���������� ACK �� ��������
  uint8_t bARD          :4;     // 04-07 ����� ����� ��������� ((N + 1) * 250���)
} tNrf24Reg04h; // SETUP_RETR   Setup of Automatic Retransmission

typedef struct 
{
  uint8_t bLNA_HCURR    :1;     // 00 ����������� �������� ��������
  uint8_t bRF_PWR       :2;     // 01-02 �������� ����������� (0 = -18dBm; 1 = -12dBm; 2 = -6dBm; 3 = 0dBm)
  uint8_t bRF_DR_HIGH   :1;     // 03 ������� �������� (������� ���). 00 = 1Mbps; 10 = 2Mbps; 01 = 250Kbps
  uint8_t bPLL_LOCK     :1;     // 04 Force PLL lock signal. Only used in test
  uint8_t bRF_DR_LOW    :1;     // 05 ������� �������� (������� ���). 00 = 1Mbps; 10 = 2Mbps; 01 = 250Kbps
  uint8_t bRes          :1;     // 06 Reserved
  uint8_t bCONT_WAVE    :1;     // 07 Enable continuous carrier transmit
} tNrf24Reg06h; // RF_SETUP     RF Setup Register

typedef struct 
{
  uint8_t bARC_CNT      :4;     // 00-03 ������� �������� �������� ������. ������� ������������ ����� ���������� �������� ������ ������.
  uint8_t bPLOS_CNT     :4;     // 04-07 ������� ���������� ������� (�� ��������� 15). ������� ������������ ������� � RF_CH.
} tNrf24Reg08h; // OBSERVE_TX   Transmit observe register

typedef struct 
{
  uint8_t bEN_DYN_ACK   :1;     // 00 Enables the W_TX_PAYLOAD_NOACK command
  uint8_t bEN_ACK_PAY   :1;     // 01 Enables Payload with ACK
  uint8_t bEN_DPL       :1;     // 02 Enables Dynamic Payload Length
  uint8_t bRes          :5;     // 03-07 Reserved
} tNrf24Reg1Dh; // FEATURE      R/W Feature Register




// ������������� nRF24
void nrf24_init(SPI_TypeDef* SPIx, uint8_t Channel);
// ������� ������������� � ������ (0..127)
uint8_t nrf24_SetChannel(uint8_t Channel);

// ��������� ������������� �������� �����������. PowerLevel - �������� ����������� (0 = -18dBm; 1 = -12dBm; 2 = -6dBm; 3 = 0dBm)
void nrf24_SetOutputPower(uint8_t PowerLevel);
// ������� ���������� ������������� � nRF24 �������� ����������� (0 = -18dBm; 1 = -12dBm; 2 = -6dBm; 3 = 0dBm)
uint8_t nrf24_GetOutputPower(void);

// ��������� ������������� ������� �����-�������� (��. ��������� NRF24_DataRate_*)
void nrf24_SetDataRate(uint8_t DataRate);
// ������� ������ ������������� ������� (��. ��������� NRF24_DataRate_*)
uint8_t nrf24_getDataRate(void);

// ��������� ������������� ����� ���� CRC (1 ���� 2 �����). ���� CRCLen = 0, �� CRC �����������
void nrf24_SetCRCLen(uint8_t CRCLen);
// ������� ������ �������������  ����� ���� CRC. ���������� 0, ���� CRC ���������
uint8_t nrf24_getCRCLen(void);

// ������� ������ ����� ���������� ������ ���������� ����� � ������ RX FIFO
uint8_t nrf24_getDynLen(void);

// ��������� ���������/��������� ���������� ����� ���������
void nrf24_DynPayload_OnOff(uint8_t Enable);
// ��������� ���������/��������� �������� ������� ������ ACK-�������.
// ������� ��������� ���������� ����� ������ (����������� �������)
void nrf24_AckPayload_OnOff(uint8_t Enable);

// ��������� ����������� ��������� ��������������� ������� �������� ��� ���������� ACK �� ��������.
// TryCount - ���-�� �������� (0..15)
// TryPeriod - ������ ����� ���������. ��������� ��� (TryPeriod + 1) * 250���
void nrf24_AutoRetrasmission_Setup(uint8_t TryCount, uint8_t TryPeriod);
// ������� ����������, ���� �� ������� �� ������� ������. �������� ������ � ������ PRX
uint8_t nrf24_TestCarrier(void);

// ��������� ��������/��������� ���������� ACK ��� ���������� (pipe) � ������� Pipe
void nrf24_setAutoAck(uint8_t Pipe, uint8_t Enable);

// ��������� ��������� � Tx FIFO ����� ��� �������� � ���� ACK-������
void nrf24_SendAckPayload(uint8_t Pipe, uint8_t *pBuff, uint8_t Len);

// ��������� ����������� ��������� ����� ��� ���������� (pipe) � ������� Pipe
void nrf24_RxPipe_Setup(uint8_t Pipe, uint8_t *pAddress, uint8_t PayloadSize);

// ������� ���������, ���� �� �������� ������ � ������ RX FIFO. ���� ���� - �������� � ����� *pPipe
// ���������� ����� ��������� ������. ���� ����� ������� ���, �� ���������� 0.
// �� ��������� pPipe ������������ ����� ����������, �� �������� ������� ����� (0-5) 
uint8_t nrf24_Recv(uint8_t *pPipe, uint8_t *pBuff);
// ��������� ���������� ����� (�������� ����� ����� ���������� �0)
// ���������� -1 � ������ ������ (�� ���� ACK, ���-�� �������� �������� ���������)
// � ������ ������, ���������� ���-�� �������� ��������
int8_t nrf24_Send(uint8_t *pAddress, uint8_t *pBuff, uint8_t Len);

// ������� ������ ��������� ������� nRF24
uint8_t nrf24_ReadState(void);
// ��������� ���������� ��������� ������ ����������
void nrf24_ResetStateFlags(uint8_t Flags);

#endif