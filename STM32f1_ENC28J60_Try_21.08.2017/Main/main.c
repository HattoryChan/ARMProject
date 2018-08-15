
#include "main.h"

//Инициализация GPIO
void gpio_init(void);

//Инициализация SPI1, SPI2
void spi_init(void);

 uint8_t enc28j60_current_bank = 0; // Bank select value
 uint16_t enc28j60_rxrdpt = 0;
 



//Инициализация SPI1, SPI2
void spi_init()
{ 
  /*Настройка SPI1 (Master)*/
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; //Тактирование модуля SPI1
  SPI1->CR1 |= SPI_CR1_BR; //Baud rate = Fpclk/256
  SPI1->CR1 |= SPI_CR1_CPOL; //Полярность тактового сигнала
  SPI1->CR1 &= ~SPI_CR1_CPHA; //Фаза тактового сигнала
  SPI1->CR1 &= ~SPI_CR1_DFF; //8 бит данных
  SPI1->CR1 &= ~SPI_CR1_LSBFIRST; //MSB передается первым
  SPI1->CR1 |= SPI_CR1_SSM; //Программный режим NSS
  SPI1->CR1 |= SPI_CR1_SSI; //Аналогично состоянию, когда на входе NSS высокий уровень
  SPI1->CR2 |= SPI_CR2_SSOE; //Вывод NSS - выход управления slave select
  SPI1->CR1 |= SPI_CR1_MSTR; //Режим Master
  SPI1->CR1 |= SPI_CR1_SPE; //Включаем SPI1
}


//Инициализация GPIO
void gpio_init()
{ 
  //Линии SPI1 (Master)
  RCC->APB2ENR |= (RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN); //Тактирование портов А, В, C и альтернативных функций
  GPIOA->CRL &= ~(GPIO_CRL_CNF7 | GPIO_CRL_CNF5 | GPIO_CRL_CNF4); //Очистка бит выбора режима 
  GPIOA->CRL |= GPIO_CRL_CNF7_1 | GPIO_CRL_CNF5_1 | GPIO_CRL_CNF4_1; //PA7(MOSI), PA5(SCK), PA4(NSS) - AF, Output, PP
  GPIOA->CRL |= GPIO_CRL_MODE7 | GPIO_CRL_MODE5 | GPIO_CRL_MODE4; //Выходы MOSI, SCK, NSS - 50MHz
  GPIOA->CRL &= ~GPIO_CRL_MODE6; //PA6(MISO) - Input
  GPIOA->CRL |= GPIO_CRL_CNF6_0; //PA6(MISO) - Input floating
}

// Передача данных через SPI
uint8_t enc28j60_rxtx(uint8_t data)
{
    SPI1->DR = data;
    while(!(SPI1->SR & SPI_SR_RXNE));
        
    return SPI1->DR;
}

// Операция чтения
uint8_t enc28j60_read_op(uint8_t cmd, uint8_t adr)
{
    uint8_t data;

    // Низкий уровень на CS
//    enc28j60_select();
    
    // Отправляем команду
    enc28j60_tx(cmd | (adr & 0x1f));
    
    // При необходимости, пропускаем "ложный" байт
    if(adr & 0x80)
        enc28j60_rx();
        
    // Читаем данные
    data = enc28j60_rx();
    
    // Высокий уровень на ножке CS
//    enc28j60_release();
    return data;
}

// Операция записи
void enc28j60_write_op(uint8_t cmd, uint8_t adr, uint8_t data)
{
  //  enc28j60_select();  //CS - LOW
    
    // Отправляем команду
    enc28j60_tx(cmd | (adr & 0x1f));
    
    // Отправляем значение
    enc28j60_tx(data);
    
   // enc28j60_release();  // CS - HIGH
}


// Выбор банка регистров
void enc28j60_set_bank(uint8_t adr)
{
    uint8_t bank;

    // Регистр относится к определённому банку?
    if( (adr & ENC28J60_ADDR_MASK) < ENC28J60_COMMON_CR )
    {
        // Получаем номер банка
        bank = (adr >> 5) & 0x03; //BSEL1|BSEL0=0x03
        
        // Если выбран "не тот" банк
        if(bank != enc28j60_current_bank)
        {
            // Выбираем банк
            enc28j60_write_op(ENC28J60_SPI_BFC, ECON1, 0x03);
            enc28j60_write_op(ENC28J60_SPI_BFS, ECON1, bank);
            enc28j60_current_bank = bank;
        }
    }
}

// Чтение регистра
uint8_t enc28j60_rcr(uint8_t adr)
{
    enc28j60_set_bank(adr);
    return enc28j60_read_op(ENC28J60_SPI_RCR, adr);
}

// Чтение пары регистров (L и H)
uint16_t enc28j60_rcr16(uint8_t adr)
{
    enc28j60_set_bank(adr);
    return enc28j60_read_op(ENC28J60_SPI_RCR, adr) |
        (enc28j60_read_op(ENC28J60_SPI_RCR, adr+1) << 8);
}

// Запись регистра
void enc28j60_wcr(uint8_t adr, uint8_t arg)
{
    enc28j60_set_bank(adr);
    enc28j60_write_op(ENC28J60_SPI_WCR, adr, arg);
}

// Запись пары регистров (L и H)
void enc28j60_wcr16(uint8_t adr, uint16_t arg)
{
    enc28j60_set_bank(adr);
    enc28j60_write_op(ENC28J60_SPI_WCR, adr, arg);
    enc28j60_write_op(ENC28J60_SPI_WCR, adr+1, arg>>8);
}

// Очистка битов в регистре (reg[adr] &= ~mask)
void enc28j60_bfc(uint8_t adr, uint8_t mask)
{
    enc28j60_set_bank(adr);
    enc28j60_write_op(ENC28J60_SPI_BFC, adr, mask);
}

// Установка битов в регистре (reg[adr] |= mask)
void enc28j60_bfs(uint8_t adr, uint8_t mask)
{
    enc28j60_set_bank(adr);
    enc28j60_write_op(ENC28J60_SPI_BFS, adr, mask);
}

// Чтение данных из буфера (по адресу в регистрах ERDPT)
void enc28j60_read_buffer(uint8_t *buf, uint16_t len)
{
 //   enc28j60_select();
    enc28j60_tx(ENC28J60_SPI_RBM);
    while(len--)
        *(buf++) = enc28j60_rx();
 //   enc28j60_release();
}

// Запись данных в буфер (по адресу в регистрах EWRPT)
void enc28j60_write_buffer(uint8_t *buf, uint16_t len)
{
//    enc28j60_select();
    enc28j60_tx(ENC28J60_SPI_WBM);
    while(len--)
        enc28j60_tx(*(buf++));
  //  enc28j60_release();
}

void enc28j60_soft_reset()
{
    // Отправляем команду
 //   enc28j60_select();
    enc28j60_tx(ENC28J60_SPI_SC);
//    enc28j60_release();

    // Ждём, пока ENC28J60 инициализируется
    delayms(1);

    // Не забываем про банк
    enc28j60_current_bank = 0;
}

//-----------------------------------------------    Delay Function (ms)
void delayms(uint16_t ms)
{
	uint32_t i;
 for(; ms !=0; ms--)
	{
		for(i = 0x2710; i != 0; i--);											// 1ms delay if Fmax = 72mHz
	}
}

// Write PHY register
void enc28j60_write_phy(uint8_t adr, uint16_t data)
{
	enc28j60_wcr(MIREGADR, adr);
	enc28j60_wcr16(MIWR, data);
	while(enc28j60_rcr(MISTAT) & MISTAT_BUSY);
}


void enc28j60_init(uint8_t *macadr)
{

    // Выполняем сброс
    enc28j60_soft_reset();

    // Настраиваем размер буфера для приёма пакетов
    enc28j60_wcr16(ERXST, ENC28J60_RXSTART);
    enc28j60_wcr16(ERXND, ENC28J60_RXEND);

    // Указатель для чтения принятых пакетов
    enc28j60_wcr16(ERXRDPT, ENC28J60_RXSTART);
    enc28j60_rxrdpt = ENC28J60_RXSTART;

    // Настраиваем MAC
    enc28j60_wcr(MACON2, 0); // очищаем сброс
    enc28j60_wcr(MACON1, MACON1_TXPAUS|MACON1_RXPAUS| // включаем управление потоком
        MACON1_MARXEN); // разрешаем приём данных
    enc28j60_wcr(MACON3, MACON3_PADCFG0| // разрешаем паддинг
        MACON3_TXCRCEN| // разрешаем рассчёт контрольной суммы
        MACON3_FRMLNEN| //разрешаем контроль длины фреймов
        MACON3_FULDPX);// включаем полный дуплекс
    enc28j60_wcr16(MAMXFL, ENC28J60_MAXFRAME); // устанавливаем максимальный размер фрейма
    enc28j60_wcr(MABBIPG, 0x15); // устанавливаем промежуток между фреймами
    enc28j60_wcr(MAIPGL, 0x12);
    enc28j60_wcr(MAIPGH, 0x0c);
    enc28j60_wcr(MAADR5, macadr[0]); // устанавливаем MAC-адрес
    enc28j60_wcr(MAADR4, macadr[1]);
    enc28j60_wcr(MAADR3, macadr[2]);
    enc28j60_wcr(MAADR2, macadr[3]);
    enc28j60_wcr(MAADR1, macadr[4]);
    enc28j60_wcr(MAADR0, macadr[5]);

    // Настраиваем PHY
    enc28j60_write_phy(PHCON1, PHCON1_PDPXMD); // включаем полный дуплекс
    enc28j60_write_phy(PHCON2, PHCON2_HDLDIS); // отключаем loopback
    enc28j60_write_phy(PHLCON, PHLCON_LACFG2| // настраиваем светодиодики
        PHLCON_LBCFG2|PHLCON_LBCFG1|PHLCON_LBCFG0|
        PHLCON_LFRQ0|PHLCON_STRCH);

    // разрешаем приём пакетов
    enc28j60_bfs(ECON1, ECON1_RXEN);
}


int main()
{
   uint8_t macadr[6] = {0,26,57,0,1,2};   
   uint8_t *pmacadr = &macadr[0];
   
   gpio_init(); //Вызов функции инициализации портов
   spi_init(); //Вызов функции инициализации модулей SPI 
   
   //Led
  GPIOC->CRH &= ~(GPIO_CRH_CNF8 | GPIO_CRH_CNF9); //PC8, PC9 - Output, PP 
   
   enc28j60_init(pmacadr);
   
   while(1)
   {
      while(!(SPI1->SR & SPI_SR_RXNE));
     if(SPI1->DR != 0)
     {
        GPIOC ->ODR ^= GPIO_ODR_ODR8;
        delayms(500);
     }
   }
}
