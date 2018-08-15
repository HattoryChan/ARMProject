#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include "socket.h"	// Just include one header for WIZCHIP
#include "spi.h"
#include "spi.h"




/////////////////////////////////////////
// SOCKET NUMBER DEFINION for Examples //
/////////////////////////////////////////
#define SOCK_TCPS        0
#define SOCK_UDPS        1
////////////////////////////////////////////////
// Shared Buffer Definition for LOOPBACK TEST //
////////////////////////////////////////////////
#define DATA_BUF_SIZE   2048
uint8_t gDATABUF[DATA_BUF_SIZE];

///////////////////////////////////
// Default Network Configuration //
///////////////////////////////////
wiz_NetInfo gWIZNETINFO = { .mac = {0x00, 0x08, 0xdc,0x00, 0xab, 0xcd},
                            .ip = {192, 168, 0, 123},
                            .sn = {255,255,255,0},
                            .gw = {192, 168, 0, 10},
                            .dns = {192, 168, 0, 10},
                            .dhcp = NETINFO_STATIC };


// initialize the dependent host peripheral
void platform_init(void);
void reverse(char s[]);
void itoa(int n, char s[]);


void delay(uint16_t sec);
char gDataOutArray[] = {'r','e','t','\n','\r'};
char gDataInArray[1000] = {0};
//////////
// TODO //
//////////////////////////////////////////////////////////////////////////////////////////////
// Call back function for W5500 SPI - Theses used as parameter of reg_wizchip_xxx_cbfunc()  //
// Should be implemented by WIZCHIP users because host is dependent                         //
//////////////////////////////////////////////////////////////////////////////////////////////
void  wizchip_select(void);
void  wizchip_deselect(void);
void  wizchip_write(uint8_t wb);
uint8_t wizchip_read();
//////////////////////////////////////////////////////////////////////////////////////////////
int32_t loopback_tcps(uint8_t, uint8_t*, uint16_t);		// Loopback TCP server
int32_t loopback_udps(uint8_t, uint8_t*, uint16_t);		// Loopback UDP server


//////////////////////////////////
// For example of ioLibrary_BSD //
//////////////////////////////////
void network_init(void);								// Initialize Network information and display it


//////////////////////////////////


//states for multythread http
#define HTTP_IDLE 0
#define HTTP_SENDING 1

//variables for multythread http
uint32_t sentsize[_WIZCHIP_SOCK_NUM_];
uint8_t http_state[_WIZCHIP_SOCK_NUM_];



int main(void)
{
   uint8_t i;
   int32_t ret = 0;
   uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};

   ///////////////////////////////////////////
   // Host dependent peripheral initialized //
   ///////////////////////////////////////////
   spiW5500_init();

   ////////////////////////////////////////////////////////////////////////////////////////////////////
   // First of all, Should register SPI callback functions implemented by user for accessing WIZCHIP //
   ////////////////////////////////////////////////////////////////////////////////////////////////////

   /* Chip selection call back */

#if   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
    reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
#elif _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_
    reg_wizchip_cs_cbfunc(wizchip_select, wizchip_select);  // CS must be tried with LOW.
#else
   #if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SIP_) != _WIZCHIP_IO_MODE_SIP_
      #error "Unknown _WIZCHIP_IO_MODE_"
   #else
      reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
   #endif
#endif


     /* SPI Read & Write callback function */
    reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);

    ////////////////////////////////////////////////////////////////////////

    /* WIZCHIP SOCKET Buffer initialize */
    if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1)
    {
       //init fail
       while(1);
    }


    /* Network initialization */
    network_init();



    //all connections inactive
    for(i=0;i<_WIZCHIP_SOCK_NUM_;i++)


	/*******************************/
	/* WIZnet W5500 Code Examples  */

    /* Main loop */
    while(1)
	{
    	/* Loopback Test */
    	// TCP server
    	// TCP server loopback test
    	    	if( (ret = loopback_tcps(SOCK_TCPS, gDATABUF, 5000)) < 0) {
    				printf("SOCKET ERROR : %ld\r\n", ret);
    			}

	}

    // end of Main loop
} // end of main()


//////////
// TODO //
/////////////////////////////////////////////////////////////////
// SPI Callback function for accessing WIZCHIP                 //
// WIZCHIP user should implement with your host spi peripheral //
/////////////////////////////////////////////////////////////////
void  wizchip_select(void)
{
	W5500_select();
}

void  wizchip_deselect(void)
{
	W5500_release();
}

void  wizchip_write(uint8_t wb)
{
	W5500_tx(wb);
}

uint8_t wizchip_read()
{
   return W5500_rx();
}
//////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
// Intialize the network information to be used in WIZCHIP //
/////////////////////////////////////////////////////////////
void network_init(void)
{
   uint8_t tmpstr[6];

	ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);

	ctlwizchip(CW_GET_ID,(void*)tmpstr);
}
/////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////
// Platform (STM32F103X) initialization for peripherals as GPIO, SPI, UARTs //
//////////////////////////////////////////////////////////////////////////////
void platform_init(void)
{
	spiW5500_init();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//****************************************************
// reverse:
void reverse(char s[])
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

//*******************************************************
// itoa
 void itoa(int n, char s[])
 {
     int i, sign;

     if ((sign = n) < 0)  /* sign */
         n = -n;          /* make positive n */
     i = 0;
     do {       /* generate reverse */
         s[i++] = n % 10 + '0';   /* next digit */
     } while ((n /= 10) > 0);     /* delete */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

 //*********************************************

 ///////////////////////////////////////////////////////////////
 // Loopback Test Example Code using ioLibrary_BSD			 //
 ///////////////////////////////////////////////////////////////
 int32_t loopback_tcps(uint8_t sn, uint8_t* buf, uint16_t port)
 {
    int32_t ret;
    uint16_t size = 0, sentsize=0;
    switch(getSn_SR(sn))
    {
       case SOCK_ESTABLISHED :
          if(getSn_IR(sn) & Sn_IR_CON)
          {
            // printf("%d:Connected\r\n",sn);   //кто-то подключен(номер подключаемого передаем функции)
             setSn_IR(sn,Sn_IR_CON);
          }
          if((size = getSn_RX_RSR(sn)) > 0)
          {
             if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;

             ret = recv(sn,buf,size);   //Количество байтов на входе(*3)

             if(ret <= 0) return ret;
             sentsize = 0;

             while(size != sentsize)
             {
           //  ret = send(sn,buf+sentsize,size-sentsize);
               ret = send(sn,&gDataInArray[0],size-sentsize);  // передаем 3 байта из массива
                if(ret < 0)
                {
                   close(sn);    //закрывает соединение
                   return ret;
                }
                sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
             }
          }
          break;
       case SOCK_CLOSE_WAIT :
          printf("%d:CloseWait\r\n",sn);
          if((ret=disconnect(sn)) != SOCK_OK) return ret;
          printf("%d:Closed\r\n",sn);
          break;
       case SOCK_INIT :
     	  printf("%d:Listen, port [%d]\r\n",sn, port);
          if( (ret = listen(sn)) != SOCK_OK) return ret;
          break;
       case SOCK_CLOSED:
          printf("%d:LBTStart\r\n",sn);
          if((ret=socket(sn,Sn_MR_TCP,port,0x00)) != sn)
             return ret;
          printf("%d:Opened\r\n",sn);
          break;
       default:
          break;
    }
    return 1;
 }
 //////////////////////////////////////////////////////////////////////////////

 ///////////////////////////////////////////////////////////////
 //    Get data from TCP									///
 /////////////////////////////////////////////////////////////
 void TCP_In_data(uint8_t* buf, uint16_t i)
 {
   gDataInArray[i] = buf;
 }



 //---------------------------------------------         Delay function (sec)
 void delay(uint16_t sec)
 {
 	uint32_t i;
  for (; sec != 0; sec--)
 	{
 		for(i = 0x5B8D80; i !=0; i--);										// 1 second delay if Fmax = 72 Mhz
 	}
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

