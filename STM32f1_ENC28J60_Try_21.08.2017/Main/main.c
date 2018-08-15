
#include "main.h"

//������������� GPIO
void gpio_init(void);

//������������� SPI1, SPI2
void spi_init(void);

 uint8_t enc28j60_current_bank = 0; // Bank select value
 uint16_t enc28j60_rxrdpt = 0;
 



//������������� SPI1, SPI2
void spi_init()
{ 
  /*��������� SPI1 (Master)*/
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; //������������ ������ SPI1
  SPI1->CR1 |= SPI_CR1_BR; //Baud rate = Fpclk/256
  SPI1->CR1 |= SPI_CR1_CPOL; //���������� ��������� �������
  SPI1->CR1 &= ~SPI_CR1_CPHA; //���� ��������� �������
  SPI1->CR1 &= ~SPI_CR1_DFF; //8 ��� ������
  SPI1->CR1 &= ~SPI_CR1_LSBFIRST; //MSB ���������� ������
  SPI1->CR1 |= SPI_CR1_SSM; //����������� ����� NSS
  SPI1->CR1 |= SPI_CR1_SSI; //���������� ���������, ����� �� ����� NSS ������� �������
  SPI1->CR2 |= SPI_CR2_SSOE; //����� NSS - ����� ���������� slave select
  SPI1->CR1 |= SPI_CR1_MSTR; //����� Master
  SPI1->CR1 |= SPI_CR1_SPE; //�������� SPI1
}


//������������� GPIO
void gpio_init()
{ 
  //����� SPI1 (Master)
  RCC->APB2ENR |= (RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN); //������������ ������ �, �, C � �������������� �������
  GPIOA->CRL &= ~(GPIO_CRL_CNF7 | GPIO_CRL_CNF5 | GPIO_CRL_CNF4); //������� ��� ������ ������ 
  GPIOA->CRL |= GPIO_CRL_CNF7_1 | GPIO_CRL_CNF5_1 | GPIO_CRL_CNF4_1; //PA7(MOSI), PA5(SCK), PA4(NSS) - AF, Output, PP
  GPIOA->CRL |= GPIO_CRL_MODE7 | GPIO_CRL_MODE5 | GPIO_CRL_MODE4; //������ MOSI, SCK, NSS - 50MHz
  GPIOA->CRL &= ~GPIO_CRL_MODE6; //PA6(MISO) - Input
  GPIOA->CRL |= GPIO_CRL_CNF6_0; //PA6(MISO) - Input floating
}

// �������� ������ ����� SPI
uint8_t enc28j60_rxtx(uint8_t data)
{
    SPI1->DR = data;
    while(!(SPI1->SR & SPI_SR_RXNE));
        
    return SPI1->DR;
}

// �������� ������
uint8_t enc28j60_read_op(uint8_t cmd, uint8_t adr)
{
    uint8_t data;

    // ������ ������� �� CS
//    enc28j60_select();
    
    // ���������� �������
    enc28j60_tx(cmd | (adr & 0x1f));
    
    // ��� �������������, ���������� "������" ����
    if(adr & 0x80)
        enc28j60_rx();
        
    // ������ ������
    data = enc28j60_rx();
    
    // ������� ������� �� ����� CS
//    enc28j60_release();
    return data;
}

// �������� ������
void enc28j60_write_op(uint8_t cmd, uint8_t adr, uint8_t data)
{
  //  enc28j60_select();  //CS - LOW
    
    // ���������� �������
    enc28j60_tx(cmd | (adr & 0x1f));
    
    // ���������� ��������
    enc28j60_tx(data);
    
   // enc28j60_release();  // CS - HIGH
}


// ����� ����� ���������
void enc28j60_set_bank(uint8_t adr)
{
    uint8_t bank;

    // ������� ��������� � ������������ �����?
    if( (adr & ENC28J60_ADDR_MASK) < ENC28J60_COMMON_CR )
    {
        // �������� ����� �����
        bank = (adr >> 5) & 0x03; //BSEL1|BSEL0=0x03
        
        // ���� ������ "�� ���" ����
        if(bank != enc28j60_current_bank)
        {
            // �������� ����
            enc28j60_write_op(ENC28J60_SPI_BFC, ECON1, 0x03);
            enc28j60_write_op(ENC28J60_SPI_BFS, ECON1, bank);
            enc28j60_current_bank = bank;
        }
    }
}

// ������ ��������
uint8_t enc28j60_rcr(uint8_t adr)
{
    enc28j60_set_bank(adr);
    return enc28j60_read_op(ENC28J60_SPI_RCR, adr);
}

// ������ ���� ��������� (L � H)
uint16_t enc28j60_rcr16(uint8_t adr)
{
    enc28j60_set_bank(adr);
    return enc28j60_read_op(ENC28J60_SPI_RCR, adr) |
        (enc28j60_read_op(ENC28J60_SPI_RCR, adr+1) << 8);
}

// ������ ��������
void enc28j60_wcr(uint8_t adr, uint8_t arg)
{
    enc28j60_set_bank(adr);
    enc28j60_write_op(ENC28J60_SPI_WCR, adr, arg);
}

// ������ ���� ��������� (L � H)
void enc28j60_wcr16(uint8_t adr, uint16_t arg)
{
    enc28j60_set_bank(adr);
    enc28j60_write_op(ENC28J60_SPI_WCR, adr, arg);
    enc28j60_write_op(ENC28J60_SPI_WCR, adr+1, arg>>8);
}

// ������� ����� � �������� (reg[adr] &= ~mask)
void enc28j60_bfc(uint8_t adr, uint8_t mask)
{
    enc28j60_set_bank(adr);
    enc28j60_write_op(ENC28J60_SPI_BFC, adr, mask);
}

// ��������� ����� � �������� (reg[adr] |= mask)
void enc28j60_bfs(uint8_t adr, uint8_t mask)
{
    enc28j60_set_bank(adr);
    enc28j60_write_op(ENC28J60_SPI_BFS, adr, mask);
}

// ������ ������ �� ������ (�� ������ � ��������� ERDPT)
void enc28j60_read_buffer(uint8_t *buf, uint16_t len)
{
 //   enc28j60_select();
    enc28j60_tx(ENC28J60_SPI_RBM);
    while(len--)
        *(buf++) = enc28j60_rx();
 //   enc28j60_release();
}

// ������ ������ � ����� (�� ������ � ��������� EWRPT)
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
    // ���������� �������
 //   enc28j60_select();
    enc28j60_tx(ENC28J60_SPI_SC);
//    enc28j60_release();

    // ���, ���� ENC28J60 ����������������
    delayms(1);

    // �� �������� ��� ����
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

    // ��������� �����
    enc28j60_soft_reset();

    // ����������� ������ ������ ��� ����� �������
    enc28j60_wcr16(ERXST, ENC28J60_RXSTART);
    enc28j60_wcr16(ERXND, ENC28J60_RXEND);

    // ��������� ��� ������ �������� �������
    enc28j60_wcr16(ERXRDPT, ENC28J60_RXSTART);
    enc28j60_rxrdpt = ENC28J60_RXSTART;

    // ����������� MAC
    enc28j60_wcr(MACON2, 0); // ������� �����
    enc28j60_wcr(MACON1, MACON1_TXPAUS|MACON1_RXPAUS| // �������� ���������� �������
        MACON1_MARXEN); // ��������� ���� ������
    enc28j60_wcr(MACON3, MACON3_PADCFG0| // ��������� �������
        MACON3_TXCRCEN| // ��������� ������� ����������� �����
        MACON3_FRMLNEN| //��������� �������� ����� �������
        MACON3_FULDPX);// �������� ������ �������
    enc28j60_wcr16(MAMXFL, ENC28J60_MAXFRAME); // ������������� ������������ ������ ������
    enc28j60_wcr(MABBIPG, 0x15); // ������������� ���������� ����� ��������
    enc28j60_wcr(MAIPGL, 0x12);
    enc28j60_wcr(MAIPGH, 0x0c);
    enc28j60_wcr(MAADR5, macadr[0]); // ������������� MAC-�����
    enc28j60_wcr(MAADR4, macadr[1]);
    enc28j60_wcr(MAADR3, macadr[2]);
    enc28j60_wcr(MAADR2, macadr[3]);
    enc28j60_wcr(MAADR1, macadr[4]);
    enc28j60_wcr(MAADR0, macadr[5]);

    // ����������� PHY
    enc28j60_write_phy(PHCON1, PHCON1_PDPXMD); // �������� ������ �������
    enc28j60_write_phy(PHCON2, PHCON2_HDLDIS); // ��������� loopback
    enc28j60_write_phy(PHLCON, PHLCON_LACFG2| // ����������� ������������
        PHLCON_LBCFG2|PHLCON_LBCFG1|PHLCON_LBCFG0|
        PHLCON_LFRQ0|PHLCON_STRCH);

    // ��������� ���� �������
    enc28j60_bfs(ECON1, ECON1_RXEN);
}


int main()
{
   uint8_t macadr[6] = {0,26,57,0,1,2};   
   uint8_t *pmacadr = &macadr[0];
   
   gpio_init(); //����� ������� ������������� ������
   spi_init(); //����� ������� ������������� ������� SPI 
   
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
