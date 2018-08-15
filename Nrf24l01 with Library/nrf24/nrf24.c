//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Это программное обеспечение распространяется свободно. Вы можете размещать
// его на вашем сайте, но не забудьте указать ссылку на мой YouTube-канал 
// "Электроника в объектике" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Автор: Надыршин Руслан / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#include <stm32f10x_i2c.h>
#include <string.h>
#include <gpio.h>
#include <spim.h>
#include <delay.h>
#include "nrf24.h"


#define NRF24_CSN_HIGH()        GPIO_WriteBit(NRF24_CSN_Port, NRF24_CSN_Pin, Bit_SET)
#define NRF24_CSN_LOW()         GPIO_WriteBit(NRF24_CSN_Port, NRF24_CSN_Pin, Bit_RESET)
#define NRF24_CE_HIGH()         GPIO_WriteBit(NRF24_CE_Port, NRF24_CE_Pin, Bit_SET)
#define NRF24_CE_LOW()          GPIO_WriteBit(NRF24_CE_Port, NRF24_CE_Pin, Bit_RESET)


SPI_TypeDef* nrf24_SPIx = SPI1; // SPI для работы с nRF24
uint16_t NRF24_TO = 0;          // Счётчик времени отправки пакета
tPipeSettings RxPipes[6];       // Структуры для хранения настроек соединений



//==============================================================================
// Функция выполняет операцию чтения группы байт из nRF24L01 в буфер pBuff командой Cmd
// Возвращает статусный байт
//==============================================================================
uint8_t nrf24_read(SPI_TypeDef* SPIx, uint8_t Cmd, uint8_t *pBuff, uint8_t Len)
{
  NRF24_CSN_LOW();

  // Передаём байт команды, параллельно принимаем байт состояния
  uint8_t State = SPI_SendRecvByte(SPIx, Cmd);
  // Принимаем указанное кол-во байт
  SPI_recv8b(SPIx, pBuff, Len);
  
  NRF24_CSN_HIGH();

  return State;
}
//==============================================================================


//==============================================================================
// Функция выполняет операцию записи группы байт из буфера pBuff в nRF24L01 командой Cmd
// Возвращает статусный байт
//==============================================================================
uint8_t nrf24_write(SPI_TypeDef* SPIx, uint8_t Cmd, uint8_t *pBuff, uint8_t Len)
{
  NRF24_CSN_LOW();

  // Передаём байт команды, параллельно принимаем байт состояния
  uint8_t State = SPI_SendRecvByte(SPIx, Cmd);
  // Передаём указанное кол-во байт
  SPI_send8b(SPIx, pBuff, Len);

  NRF24_CSN_HIGH();

  return State;
}
//==============================================================================


//==============================================================================
// Функция читает регистр из nRF24L01
//==============================================================================
uint8_t nrf24_read_reg(SPI_TypeDef* SPIx, uint8_t RegAddr)
{
  RegAddr &= NRF24_REGADDR_MASK;
  uint8_t Reg = 0;
  nrf24_read(SPIx, NRF24_CMD_R_REGISTER | RegAddr, &Reg, 1);
  return Reg;
}
//==============================================================================


//==============================================================================
// Функция читает группу регистров из nRF24L01 в буфер nRF24L01
//==============================================================================
uint8_t nrf24_read_regs(SPI_TypeDef* SPIx, uint8_t RegAddr, uint8_t *pBuff, uint8_t Len)
{
  RegAddr &= NRF24_REGADDR_MASK;
  return nrf24_read(SPIx, NRF24_CMD_R_REGISTER | RegAddr, pBuff, Len);
}
//==============================================================================


//==============================================================================
// Процедура записывает 1 регистр nRF24L01
//==============================================================================
void nrf24_write_reg(SPI_TypeDef* SPIx, uint8_t RegAddr, uint8_t Value)
{
  RegAddr &= NRF24_REGADDR_MASK;
  nrf24_write(SPIx, NRF24_CMD_W_REGISTER | RegAddr, &Value, 1);
}
//==============================================================================


//==============================================================================
// Функция записывает группу регистров nRF24L01 из буфера pBuff, возвращает статусный байт
//==============================================================================
uint8_t nrf24_write_regs(SPI_TypeDef* SPIx, uint8_t RegAddr, uint8_t *pBuff, uint8_t Len)
{
  RegAddr &= NRF24_REGADDR_MASK;
  return nrf24_write(SPIx, NRF24_CMD_W_REGISTER | RegAddr, pBuff, Len);
}
//==============================================================================


//==============================================================================
// Являясь передатчиком циклически повторять последний переданный пакет пока CE = 1
//==============================================================================
uint8_t nrf24_reuse_tx_lastpayload(SPI_TypeDef* SPIx)
{
  return nrf24_write(SPIx, NRF24_CMD_REUSE_TX_PL, 0, 0);
}
//==============================================================================


//==============================================================================
// Функция применяет изменения в настройках R_RX_PL_WID, W_ACK_PAYLOAD, W_TX_PAYLOAD_NOACK
//==============================================================================
#if (NRF24_MODEL == NRF24_MODEL_L01)
uint8_t nrf24_activate(SPI_TypeDef* SPIx)
{
  uint8_t Buff = 0x73;
  return nrf24_write(SPIx, NRF24_CMD_ACTIVATE, &Buff, 1);
}
#endif
//==============================================================================


//==============================================================================
// В режиме работы приёмником, функция пишет тело пакета, для отправки передатчику в очередном ACK-пакете
//==============================================================================
uint8_t nrf24_write_ackpayload(SPI_TypeDef* SPIx, uint8_t *pBuff, uint8_t Len)
{
  if (Len > 32)
    Len = 32;
  return nrf24_read(SPIx, NRF24_CMD_W_ACK_PAYLOAD, pBuff, Len);
}
//==============================================================================


//==============================================================================
// Функция устанавливает № канала (0..127)
//==============================================================================
uint8_t nrf24_SetChannel(uint8_t Channel)
{
  Channel &= 0x7F;
  return nrf24_write_regs(nrf24_SPIx, NRF24_REG_RF_CH, &Channel, 1);
}
//==============================================================================


//==============================================================================
// Функция очищает входной FIFO-буфер пакетов
//==============================================================================
uint8_t nrf24_FlushRx(void)
{
  return nrf24_write(nrf24_SPIx, NRF24_CMD_FLUSH_RX, 0, 0);
}
//==============================================================================


//==============================================================================
// Функция очищает выходной FIFO-буфер пакетов
//==============================================================================
uint8_t nrf24_FlushTx(void)
{
  return nrf24_write(nrf24_SPIx, NRF24_CMD_FLUSH_TX, 0, 0);
}
//==============================================================================


//==============================================================================
// Функция читает пакет из входного FIFO-буфера
//==============================================================================
uint8_t nrf24_Read_RxPayload(uint8_t *pBuff, uint8_t Len)
{
  if (Len > 32)
    Len = 32;
  return nrf24_read(nrf24_SPIx, NRF24_CMD_R_RX_PAYLOAD, pBuff, Len);
}
//==============================================================================


//==============================================================================
// Функция записывает пакет в выходной FIFO-буфер 
//==============================================================================
uint8_t nrf24_Write_TxPayload(uint8_t *pBuff, uint8_t Len)
{
  if (Len > 32)
    Len = 32;
  return nrf24_write(nrf24_SPIx, NRF24_CMD_W_TX_PAYLOAD, pBuff, Len);
}
//==============================================================================


//==============================================================================
// Процедура устанавливает мощность передатчика. PowerLevel - мощность передатчика (0 = -18dBm; 1 = -12dBm; 2 = -6dBm; 3 = 0dBm)
//==============================================================================
void nrf24_SetOutputPower(uint8_t PowerLevel)
{
  tNrf24Reg06h RegValue; 
  nrf24_read_regs(nrf24_SPIx, NRF24_REG_RF_SETUP, (uint8_t *) &RegValue, 1);
  RegValue.bRF_PWR = (PowerLevel > 3) ? 3 : PowerLevel; 
  nrf24_write_regs(nrf24_SPIx, NRF24_REG_RF_SETUP, (uint8_t *) &RegValue, 1);
}
//==============================================================================


//==============================================================================
// Функция возвращает установленную в nRF24 мощность передатчика (0 = -18dBm; 1 = -12dBm; 2 = -6dBm; 3 = 0dBm)
//==============================================================================
uint8_t nrf24_GetOutputPower(void)
{
  tNrf24Reg06h RegValue; 
  nrf24_read_regs(nrf24_SPIx, NRF24_REG_RF_SETUP, (uint8_t *) &RegValue, 1);
  return RegValue.bRF_PWR;
}
//==============================================================================


//==============================================================================
// Процедура устанавливает частоту приёмо-передачи (см. константы NRF24_DataRate_*)
//==============================================================================
void nrf24_SetDataRate(uint8_t DataRate)
{
  if (DataRate > 2)
    DataRate = NRF24_DataRate_2MBps;
  
  tNrf24Reg06h RegValue; 
  nrf24_read_regs(nrf24_SPIx, NRF24_REG_RF_SETUP, (uint8_t *) &RegValue, 1);
  
  // Значения битов bRF_DR_HIGH и bRF_DR_LOW. 00 = 1Mbps; 10 = 2Mbps; 01 = 250Kbps
  switch (DataRate)
  {
  case NRF24_DataRate_250KBps:
    RegValue.bRF_DR_HIGH = 0;
    RegValue.bRF_DR_LOW = 1;
    break;
  case NRF24_DataRate_1MBps:
    RegValue.bRF_DR_HIGH = 0;
    RegValue.bRF_DR_LOW = 0;
    break;
  case NRF24_DataRate_2MBps:
    RegValue.bRF_DR_HIGH = 1;
    RegValue.bRF_DR_LOW = 0;
    break;
  }
  
  nrf24_write_regs(nrf24_SPIx, NRF24_REG_RF_SETUP, (uint8_t *) &RegValue, 1);
}
//==============================================================================


//==============================================================================
// Функция читает установленную частоту (см. константы NRF24_DataRate_*)
//==============================================================================
uint8_t nrf24_getDataRate(void)
{
  tNrf24Reg06h RegValue; 
  nrf24_read_regs(nrf24_SPIx, NRF24_REG_RF_SETUP, (uint8_t *) &RegValue, 1);
  
  // Значения битов bRF_DR_HIGH и bRF_DR_LOW. 00 = 1Mbps; 10 = 2Mbps; 01 = 250Kbps
  if ((!RegValue.bRF_DR_HIGH) && RegValue.bRF_DR_LOW)
    return NRF24_DataRate_250KBps;
  else if ((!RegValue.bRF_DR_HIGH) && (!RegValue.bRF_DR_LOW))
    return NRF24_DataRate_1MBps;
  else if (RegValue.bRF_DR_HIGH && (!RegValue.bRF_DR_LOW))
    return NRF24_DataRate_2MBps;

  return 3;     // Не корректное значение
}
//==============================================================================


//==============================================================================
// Процедура устанавливает длину поля CRC (1 либо 2 байта). Если CRCLen = 0, то CRC отключается
//==============================================================================
void nrf24_SetCRCLen(uint8_t CRCLen)
{
  tNrf24Reg00h RegValue; 
  nrf24_read_regs(nrf24_SPIx, NRF24_REG_CONFIG, (uint8_t *) &RegValue, 1);

  if (CRCLen)   // CRC должен быть включен
  {
    RegValue.bEN_CRC = 1;
    RegValue.bCRC_O = (CRCLen == 1) ? 0 : 1;
  }
  else          // CRC нужно отключить
    RegValue.bEN_CRC = 0;
  
  nrf24_write_regs(nrf24_SPIx, NRF24_REG_CONFIG, (uint8_t *) &RegValue, 1);
}
//==============================================================================


//==============================================================================
// Функция читает установленную  длину поля CRC. Возвращает 0, если CRC отключено
//==============================================================================
uint8_t nrf24_getCRCLen(void)
{
  tNrf24Reg00h RegValue; 
  nrf24_read_regs(nrf24_SPIx, NRF24_REG_CONFIG, (uint8_t *) &RegValue, 1);
  
  if (!RegValue.bEN_CRC)
    return 0;
  if (RegValue.bCRC_O)
    return 2;
  return 1;
}
//==============================================================================


//==============================================================================
// Включаем/отключаем переменную длину пакета для соединения (pipe) с номером PipeNum
//==============================================================================
void nrf24_SetDynLen(uint8_t Pipe, uint8_t Enable)
{
  uint8_t RegValue;
  if (Pipe >= NRF24_PipesNum)
    return;
  
  nrf24_read_regs(nrf24_SPIx, NRF24_REG_DYNPD, &RegValue, 1);

  if (Enable)
    RegValue |= (1 << Pipe);
  else
    RegValue &= ~(1 << Pipe);
  
  nrf24_write_regs(nrf24_SPIx, NRF24_REG_DYNPD, &RegValue, 1);
}
//==============================================================================


//==============================================================================
// Функция читает длину очередного пакета переменной длины в буфере RX FIFO
//==============================================================================
uint8_t nrf24_getDynLen(void)
{
  uint8_t Size = 0;
  nrf24_read(nrf24_SPIx, NRF24_CMD_R_RX_PL_WID, &Size, 1);
  return Size;
}
//==============================================================================


//==============================================================================
// Процедура разрешает/запрещает переменную длину глобально
//==============================================================================
void nrf24_DynPayload_OnOff(uint8_t Enable)
{
#if (NRF24_MODEL == NRF24_MODEL_L01)    // Микросхема без "+"
  nrf24_activate(nrf24_SPIx);
#endif

  // Разрешаем переменную длину пакета
  tNrf24Reg1Dh RegValue;
  nrf24_read_regs(nrf24_SPIx, NRF24_REG_FEATURE, (uint8_t *) &RegValue, 1);
  RegValue.bEN_DPL = (Enable) ? 1 : 0;
  nrf24_write_regs(nrf24_SPIx, NRF24_REG_FEATURE, (uint8_t *) &RegValue, 1);
}
//==============================================================================


//==============================================================================
// Процедура Разрешает/запрещает отправку пакетов внутри ACK-пакетов
// Попутно разрешает переменную длину пакета (необходимое условие)
//==============================================================================
void nrf24_AckPayload_OnOff(uint8_t Enable)
{
#if (NRF24_MODEL == NRF24_MODEL_L01)
  nrf24_activate(nrf24_SPIx);
#endif
  
  // Разрешаем/запрещаем отправку пакетов внутри ACK-пакетов
  tNrf24Reg1Dh RegValue;
  nrf24_read_regs(nrf24_SPIx, NRF24_REG_FEATURE, (uint8_t *) &RegValue, 1);
  RegValue.bEN_DPL = 1;
  RegValue.bEN_ACK_PAY = (Enable) ? 1 : 0;
  nrf24_write_regs(nrf24_SPIx, NRF24_REG_FEATURE, (uint8_t *) &RegValue, 1);

  // Enable dynamic payload on pipes 0 & 1
  nrf24_write_reg(nrf24_SPIx, NRF24_REG_DYNPD, (1 << 0) | (1 << 1));
}
//==============================================================================


//==============================================================================
// Процедура настраивает параметры автоматического повтора передачи при отсутствии ACK от приёмника.
// TryCount - кол-во повторов (0..15)
// TryPeriod - период между повторами. Считается как (TryPeriod + 1) * 250мкс
//==============================================================================
void nrf24_AutoRetrasmission_Setup(uint8_t TryCount, uint8_t TryPeriod)
{
  tNrf24Reg04h RegValue;
  RegValue.bARC = (TryCount > 15) ? 15 : TryCount;
  RegValue.bARD = (TryPeriod > 15) ? 15 : TryPeriod;
  
  nrf24_write_regs(nrf24_SPIx, NRF24_REG_SETUP_RETR, (uint8_t *) &RegValue, 1);
}
//==============================================================================


//==============================================================================
// Функция возвращает, есть ли несущая на текущем канале. Доступно только в режиме PRX
//==============================================================================
uint8_t nrf24_TestCarrier(void)
{
  return nrf24_read_reg(nrf24_SPIx, NRF24_REG_CD) & (1<<0);
}
//==============================================================================


//==============================================================================
// Процедура включает/отключает автовыдачу ACK для соединения (pipe) с номером Pipe
//==============================================================================
void nrf24_setAutoAck(uint8_t Pipe, uint8_t Enable)
{
  if (Pipe >= NRF24_PipesNum)
    return;
    
  uint8_t RegValue = nrf24_read_reg(nrf24_SPIx, NRF24_REG_EN_AA);
  if (Enable)
    RegValue |= (1 << Pipe);
  else
    RegValue &= ~(1 << Pipe);

  nrf24_write_reg(nrf24_SPIx, NRF24_REG_EN_AA, RegValue);
}
//==============================================================================


//==============================================================================
// Процедура загружает в Tx FIFO пакет для передачи в теле ACK-пакета
//==============================================================================
void nrf24_SendAckPayload(uint8_t Pipe, uint8_t *pBuff, uint8_t Len)
{
  if (Len > 32)
    Len = 32;
  if (Pipe > 5)
    return;
  
  nrf24_write(nrf24_SPIx, NRF24_CMD_W_ACK_PAYLOAD | Pipe, pBuff, Len);
}
//==============================================================================


//==============================================================================
// Процедура переводит модуль в режим приёмника (PRX)
//==============================================================================
void nrf24_Set_PRX_Mode(void)
{
  // Переключаемся в режим приёмника и будим nRF24 (PowerUp)
  tNrf24Reg00h RegValue;
  nrf24_read_regs(nrf24_SPIx, NRF24_REG_CONFIG, (uint8_t *) &RegValue, 1);
  RegValue.bPRIM_RX = 1;        // Переключаемся в режим приёмника (PRX)
  RegValue.bPWR_UP = 1;         // Будим nRF24, если он спал
  nrf24_write_regs(nrf24_SPIx, NRF24_REG_CONFIG, (uint8_t *) &RegValue, 1);

  // Сбрасываем флаги прерываний nRF24
  nrf24_ResetStateFlags(NRF24_bMAX_RT_Mask | NRF24_bTX_DS_Mask | NRF24_bRX_DR_Mask);

  // Восстанавливаем адрес Rx для соединения №0, т.к. он мог быть затёрт при передаче
  nrf24_write_regs(nrf24_SPIx, NRF24_REG_RX_ADDR_P0, RxPipes[0].Address, 5);

  // Очищаем буферы FIFO
  nrf24_FlushRx();
  nrf24_FlushTx();

  NRF24_CE_HIGH();

  // Задержка как минимум 130 мкс
  delay_us(135);}
//==============================================================================


//==============================================================================
// Процедура переводит модуль в режим передатчика (PTX)
//==============================================================================
void nrf24_Set_PTX_Mode(void)
{
  NRF24_CE_LOW();
  // Очищаем буферы FIFO
  nrf24_FlushRx();
  nrf24_FlushTx();
}
//==============================================================================


//==============================================================================
// Процедура настраивает параметры приёма для соединения (pipe) с номером Pipe
//==============================================================================
void nrf24_RxPipe_Setup(uint8_t Pipe, uint8_t *pAddress, uint8_t PayloadSize)
{
  // Некорректный номер соединения (pipe)
  if (Pipe >= NRF24_PipesNum)
    return;
  // Ограничиваем длину пакета
  if (PayloadSize > 32)
    PayloadSize = 32;
  
  // Сохраняем в структурах настройки указанного соединения (pipe)
  memcpy(RxPipes[Pipe].Address, pAddress, 5);
  RxPipes[Pipe].PayLoadLen = PayloadSize;
  RxPipes[Pipe].bDynPayLoad = (PayloadSize) ? 0 : 1;
  nrf24_SetDynLen(Pipe, RxPipes[Pipe].bDynPayLoad);
  if (RxPipes[Pipe].bDynPayLoad)
    nrf24_DynPayload_OnOff(1);
  
  // Для соединений 2..5 пишем только один последний байт адреса, остальные должны совпадать с байтами адреса соединения №1
  // Для соединений 0..1 пишем все 5 байт адреса
  if (Pipe < 2)
    nrf24_write_regs(nrf24_SPIx, NRF24_REG_RX_ADDR_P0 + Pipe, pAddress, 5);
  else
    nrf24_write_regs(nrf24_SPIx, NRF24_REG_RX_ADDR_P0 + Pipe, pAddress, 1);

  if (!RxPipes[Pipe].bDynPayLoad)       // Размер пакета статический
  {
    // Устанавливаем размер пакета для соединения
    nrf24_write_reg(nrf24_SPIx, NRF24_REG_RX_PW_P0 + Pipe, PayloadSize);
  }
  
  // Включаем приём из указанного соединения (pipe)
  uint8_t RegValue = nrf24_read_reg(nrf24_SPIx, NRF24_REG_EN_RXADDR);
  RegValue |= (1 << Pipe);
  nrf24_write_reg(nrf24_SPIx, NRF24_REG_EN_RXADDR, RegValue);
}
//==============================================================================


//==============================================================================
// Функция проверяет, есть ли принятые пакеты в буфере RX FIFO. Если есть - копирует в буфер *pPipe
// Возвращает длину принятого пакета. Если новых пакетов нет, то возвращает 0.
// По указателю pPipe записывается номер соединения, из которого получен пакет (0-5) 
//==============================================================================
uint8_t nrf24_Recv(uint8_t *pPipe, uint8_t *pBuff)
{
  uint8_t status = nrf24_ReadState();
  uint8_t Len = 0;
  
  // Сброс флага окончания отправки ACK
  if (status & NRF24_bTX_DS_Mask)
    nrf24_ResetStateFlags(NRF24_bTX_DS_Mask);

  if (status & NRF24_bRX_DR_Mask)       // Был принят пакет и флаг RX_DR ещё не сброшен
  {
    // Читаем номер соединения (pipe)
    *pPipe = (status >> 1) & 0x07;
    
    // Читаем тело пакета
    if (RxPipes[*pPipe].bDynPayLoad)
      Len = nrf24_getDynLen();
    else
      Len = RxPipes[*pPipe].PayLoadLen;
    
    nrf24_Read_RxPayload(pBuff, Len);
    
    // Сбрасываем флаг RX_DR
    nrf24_ResetStateFlags(NRF24_bRX_DR_Mask);
  }
  
  return Len;
}
//==============================================================================


//==============================================================================
// Процедура отправляет пакет (передача пойдёт через соединение №0)
// Возвращает -1 в случае ошибки (не было ACK, кол-во повторов передачи истрачено)
// В случае успеха, возвращает кол-во повторов передачи
//==============================================================================
int8_t nrf24_Send(uint8_t *pAddress, uint8_t *pBuff, uint8_t Len)
{
  uint8_t State;
  int8_t ReturnVal = -1;
  tNrf24Reg00h RegValue;

  // Пишем адреса на передачу и приём с соединения №0
  //nrf24_write_regs(nrf24_SPIx, NRF24_REG_RX_ADDR_P0, pAddress, 5);
  nrf24_write_regs(nrf24_SPIx, NRF24_REG_TX_ADDR, pAddress, 5);

  nrf24_Set_PTX_Mode();
  
  // Переключаемся в режим передатчика и будим nRF24 (PowerUp)
  nrf24_read_regs(nrf24_SPIx, NRF24_REG_CONFIG, (uint8_t *) &RegValue, 1);
  RegValue.bPRIM_RX = 0;        // Переключаемся в режим передатчика (PTX)
  RegValue.bPWR_UP = 1;         // Будим nRF24, если он спал
  nrf24_write_regs(nrf24_SPIx, NRF24_REG_CONFIG, (uint8_t *) &RegValue, 1);
  
  // Задержка минимум 130 мкс
  delay_us(135);

  // Пишем в nRF24 тело пакета на передачу
  nrf24_Write_TxPayload(pBuff, Len);

  // Выдаём положительный импульс на ножку CE
  NRF24_CE_HIGH();
  delay_us(10);         // Задержка минимум 10 мкс
  NRF24_CE_LOW();

  // Ждём событий от nRF24
  NRF24_TO = NRF24_Tx_TimeOut;
  do
  {
    State = nrf24_ReadState();
    delay_us(100);
    NRF24_TO--;
  }
  while ( (!(State & (NRF24_bMAX_RT_Mask | NRF24_bTX_DS_Mask))) && NRF24_TO);
  
  if (NRF24_TO)
    nrf24_ResetStateFlags(NRF24_bMAX_RT_Mask | NRF24_bTX_DS_Mask);
    
  // Переключаемся в режим приёмника и переводим nRF24 в режим PowerDown
  nrf24_read_regs(nrf24_SPIx, NRF24_REG_CONFIG, (uint8_t *) &RegValue, 1);
  RegValue.bPRIM_RX = 1;        // Переключаемся в режим приёмника (PRX)
  RegValue.bPWR_UP = 0;         // Усыпляем nRF24
  nrf24_write_regs(nrf24_SPIx, NRF24_REG_CONFIG, (uint8_t *) &RegValue, 1);

  if (State & NRF24_bTX_DS_Mask)// Пакет был успешно отправлен (ACK получен), возвращаем кол-во перезапросов
  {
    // Читаем счётчики отправки
    tNrf24Reg08h Observe;
    nrf24_read_regs(nrf24_SPIx, NRF24_REG_OBSERVE_TX, (uint8_t *) &Observe, 1);
    ReturnVal = Observe.bARC_CNT;
  }
  
  nrf24_Set_PRX_Mode();
  
  return ReturnVal;
}
//==============================================================================


//==============================================================================
// Функция читает статусный регистр nRF24
//==============================================================================
uint8_t nrf24_ReadState(void)
{
  return nrf24_read(nrf24_SPIx, NRF24_CMD_NOP, 0, 0);
}
//==============================================================================


//==============================================================================
// Процедура сбрасывает состояние флагов прерываний
//==============================================================================
void nrf24_ResetStateFlags(uint8_t Flags)
{
  nrf24_write_reg(nrf24_SPIx, NRF24_REG_STATUS, Flags);
}
//==============================================================================


//==============================================================================
// Инициализация nRF24
//==============================================================================
void nrf24_init(SPI_TypeDef* SPIx, uint8_t Channel)
{
  nrf24_SPIx = SPIx;
  
  // Инициализируем ножки CE, CSN
  gpio_PortClockStart(NRF24_CSN_Port);
  gpio_PortClockStart(NRF24_CE_Port);
  gpio_SetGPIOmode_Out(NRF24_CSN_Port, NRF24_CSN_Pin);
  gpio_SetGPIOmode_Out(NRF24_CE_Port, NRF24_CE_Pin);
  NRF24_CSN_HIGH();
  NRF24_CE_LOW();
  // Инициализируем SPI
  spim_init(SPIx, 8);
  
  // Задержка
  delay_ms(100);
  
  // Настраиваем автоповтор передачи
  nrf24_AutoRetrasmission_Setup(5, 2);          // 5 попыток с периодом 0.75 мс
  // Устанавливаем мощность передатчика
  nrf24_SetOutputPower(NRF24_OutPower_0dBm);    // 0dBm
  // Устанавливаем частоту работы
  nrf24_SetDataRate(NRF24_DataRate_250KBps);//NRF24_DataRate_2MBps);
  // Устанавливаем длину поля CRC (1 либо 2 байта)
  nrf24_SetCRCLen(2);
  // Сбрасываем флаги прерываний nRF24
  nrf24_ResetStateFlags(NRF24_bMAX_RT_Mask | NRF24_bTX_DS_Mask | NRF24_bRX_DR_Mask);
  // Устанавливаем канал
  nrf24_SetChannel(Channel);
  // Очищаем буферы Rx/Tx
  nrf24_FlushRx();
  nrf24_FlushTx();
  
  nrf24_Set_PRX_Mode();
}
//==============================================================================
