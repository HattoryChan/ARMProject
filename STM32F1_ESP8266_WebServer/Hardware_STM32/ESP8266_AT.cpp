/* Library for work with ESP8266(NOT USED)
*
*
*
*
*
*/
//Нужна функция возвращающая заданные элементы массива по номеру. 

#include "stdio.h"
#include "ESP8266_AT.h"

int g_ESP8266InBuffer[512] = {0}; //Array for keep input message
int g_ESP8266ArrayWritePos = 0;  //Show write position in g_ESP8266InBuffer

bool g_UnreadMess = false;  //Show when we have unread message, tree - have unread

/*=======================================================================
                           Work with In Buffer Part
========================================================================*/
/*    Handler for UART
*     Connect ESP8266 and STM32F1(PA[9:10])
*/
extern "C"     //We can see interrupts
{
void USART1_IRQHandler(void)
 { 
	if(USART1->SR & USART_SR_RXNE)	//if we have message on RX
   {
      
      g_UnreadMess = true;      
      g_ESP8266InBuffer[g_ESP8266ArrayWritePos] = USART1->DR;  //Write data on array
      
      g_ESP8266ArrayWritePos++;
      
      if(g_ESP8266ArrayWritePos == 513)  //if buffer is FULL
      {
         
      }
   USART1->SR &= ~USART_SR_RXNE;   // Reset interruption flag
   }
 }
}     //EXTERN "C" END


/* Search Input string in ESP8266 AT input buffer
*  Input: SearchTwoSymb - this text we will search in g_ESP8266InBuffer
*  OutPut: Last symbol string position or Not Found(513)
*/
int ESP8266_SearchStringBuffer(char* SearchTwoSymb)
{
 for(int i = 0; i<=(sizeof(g_ESP8266InBuffer)/sizeof(int)); i++)   //Walk on All Buffer Position
 {
   if(g_ESP8266InBuffer[i] == SearchTwoSymb[0])  //If we have found first consilience
    {              
      if(g_ESP8266InBuffer[i+1] == SearchTwoSymb[1])  //If we have found second consilience          
             return i+1;                                     //Return position          
    }  
 }    
  return 513;           //Error
}


/* Search Input string in ESP8266 AT input buffer from the starting position
*  Input: SearchTwoSymb - this text we will search in g_ESP8266InBuffer
*         StartSearchPos - we start search in this position
*  OutPut: Last symbol string position or Not Found(513)
*/
int ESP8266_SearchStringBuffer(char* SearchTwoSymb, int StartSearchPos)
{
 for( ; StartSearchPos<=(sizeof(g_ESP8266InBuffer)/sizeof(int)); StartSearchPos++)   //Walk on All Buffer Position
 {
    if(g_ESP8266InBuffer[StartSearchPos] == SearchTwoSymb[0])  //If we have found first consilience
    {         
      if(g_ESP8266InBuffer[StartSearchPos+1] == SearchTwoSymb[1])  //If we have found second consilience          
             return StartSearchPos+1;                                     //Return position         
    } 
 }    
  return 513;           //Error
}



/* Search symbol in ESP8266 AT input buffer 
*  Input: SearchSymb - this symbol we will search in g_ESP8266InBuffer
*  Output: Symbol position or ERROR(513)
*/
int ESP8266_SearchSymbolBuffer(char SearchSymb)
{
   for(int i = 0; i<=(sizeof(g_ESP8266InBuffer)/sizeof(int)); i++)
   {
      if(g_ESP8266InBuffer[i] == SearchSymb)
         return i;                  //Return Position
   }
   return 513;
}


/* Search symbol in ESP8266 AT input buffer 
*  Input: SearchSymb - this symbol we will search in g_ESP8266InBuffer
*         StartSearchPos - we start search in this position 
*  Output: Symbol position or ERROR(513)
*/
int ESP8266_SearchSymbolBuffer(char SearchSymb, int StartSearchPos)
{
   for( ; StartSearchPos<=(sizeof(g_ESP8266InBuffer)/sizeof(int)); StartSearchPos++)
   {
      if(g_ESP8266InBuffer[StartSearchPos] == SearchSymb)
         return StartSearchPos;                  //Return Position
   }
   return 513;
}


/*  Repeal g_ESP8266ArrayWritePos and make position "0"
*   Input:
*   Output:
*/
void ESP8266_RepealArrayWritePos()
{
   g_ESP8266ArrayWritePos = 0;
}

/*  Null g_ESP8266InBuffer
*   Input:
*   Output:
*/
void ESP8266_NullInBuffer()
{
   for(int i = 0; i<=(sizeof(g_ESP8266InBuffer)/sizeof(int)); i++)
      g_ESP8266InBuffer[i] = 0;
}

/*  Return g_UnreadMess flag status
*   Input:
*   Output:
*/
bool ESP8266_UnreadFlagStatus()
{
   return g_UnreadMess;
}

/*  Reset g_UnreadMess flag
*   Input:
*   Output:
*/
void ESP8266_UnreadResetFlag()
{
   g_UnreadMess = false;
}

/*  Wait for a responce 10 second, if don't have - return error.
*   Input:
*   Output: '0' - error, '1' - we have answer
*/
bool ESP8266_WaitInMess()
{   
   uint32_t sec = 10;    //wait 10 seconds
 for (; sec != 0; sec--)
	{
		for(int i = 0x5B8D80; i !=0; i--);										// 1 second delay if Fmax = 72 Mhz
      {
       if(g_UnreadMess  == true)
         return 1;
      }
	}
   return 0;            //We dont have message
}

/*  Check input connection to TCP server
*    Searched message structure:  "+IPD,0,395:GET / HTTP/1.1...."
*     Where: '0' - identifier
*   Input:
*   Output: identifier or 513 if we don't search him
*/
int ESP8266_InConnectIdentifier()
{
   if(ESP8266_SearchStringBuffer("IP") != 513)
   {
      //search "+IPD," mess and return identifier
      return (g_ESP8266InBuffer[ESP8266_SearchStringBuffer("D,")+1]);
   }
   else 
      return 513;
}


/* Return symbol from g_ESP8266InBuffer
*  Input: Pos - returned symbol position
*  Output: symbol in g_ESP8266InBuffer[Pos]
*/
int ESP8266_ReturnSymbFromInBuff(uint8_t Pos)
{
   return g_ESP8266InBuffer[Pos];
}

/*=======================================================================
                     Work with AT command part     
========================================================================*/
/*  Send "AT" to ESP8266, answer "OK"
*   Input:
*   Output: '0'- error esp8266, '1' - Successful, '2' - another ERROR
*/
int AT_CheckReady()
{
   ESP8266_RepealArrayWritePos();  //Repeal write pos   
   ESP8266_NullInBuffer();       //Null input buffer
   
   ESP8266_SendStr("AT\r\n"); //Send "AT", answer mess: "OK"
   
   if(ESP8266_WaitInMess() == 1)  //We have input mess
   {
      if(ESP8266_SearchStringBuffer("OK") != 513)  //And in mess we have "OK"
         return 1;      //Operation Successful
      else      
         return 0;      //ESP8266 don't answer "OK"   
   }      
   
   return 2;   //We have ANOTHER ERROR     
}


/*  Send "AT+RST" to ESP8266, this instruction RESET him. Answer "OK", waited
*       and module give us trash in input buffer
*   Input:
*   Output: '0'- error esp8266, '1' - Successful, '2' - another ERROR
*/
int AT_ResetModule()
{
   ESP8266_RepealArrayWritePos(); //Repeal write pos
   ESP8266_NullInBuffer();       //Null input buffer
   
   ESP8266_SendStr("AT+RST\r\n"); //Send "AT+RST", this inctruction reset module
   
   if(ESP8266_WaitInMess() == 1) //We have input mess
   {
      if(ESP8266_SearchStringBuffer("OK") != 513)  //And in mes we have "OK"
      { 
         if(AT_CheckReady() == 1)      //Module ready after reset         
           return 1;    //Operation succesful
         else 
           return 0;   //ESP8266 don't ready, error
      }         
      
   }
   
   return 2;   //Another ERROR
}


/*  Send "AT+CWMODE" to ESP8266, answer "OK"
*     This insctuction change working mod, 1 - Wifi client, 2 - SoftAP
*   Input: Mode: '1' - Wifi client, '2' - Soft AP, '3' - return value 
*          SaveToFlash: '0' - don't save, '1' - save
*   Output: '0'- another error, '1' - Wi-fi client, '2' - Soft AP
*/
int AT_WorkModeChange(uint16_t Mode, bool SaveToFlash)
{
   ESP8266_RepealArrayWritePos(); //Repeal write pos
   ESP8266_NullInBuffer();       //Null input buffer
   
   switch(Mode)
   {
      case 1:                       //Wi-fi client
         if(SaveToFlash == false)
         {
            ESP8266_SendStr("AT+CWMODE_CUR=1\r\n");
            
            if(ESP8266_WaitInMess())  //We have input message
            {
               if(ESP8266_SearchStringBuffer("OK") != 513)  //And in mes we have "OK"                 
                  return 1;     //Operation succesful
               else 
                  return 0;     //ESP8266 don't answer "OK"   
            }
         }
         else if(SaveToFlash == true)
               {
                  ESP8266_SendStr("AT+CWMODE_DEF=1\r\n");
            
                  if(ESP8266_WaitInMess())  //We have input message
                   {
                     if(ESP8266_SearchStringBuffer("OK") != 513)  //And in mes we have "OK"                 
                        return 1;     //Operation succesful, Wi-fi client
                     else 
                        return 0;     //ESP8266 don't answer "OK"   
                   }
               }
         else 
              return 0; //Eror input value
      //END Case 1
         
      case 2:                    //Soft AP
          if(SaveToFlash == false)
         {
            ESP8266_SendStr("AT+CWMODE_CUR=2\r\n");
            
            if(ESP8266_WaitInMess())  //We have input message
            {
               if(ESP8266_SearchStringBuffer("OK") != 513)  //And in mes we have "OK"                 
               {
                  return AT_WorkModeChange(3,0);    //Check mode value
               }
               else 
                  return 0;     //ESP8266 don't answer "OK"   
            }
            else 
                  return 0;     //Module don't answer
         }
         else if(SaveToFlash == true)
               {
                  ESP8266_SendStr("AT+CWMODE_DEF=2\r\n");
            
                  if(ESP8266_WaitInMess())  //We have input message
                   {
                     if(ESP8266_SearchStringBuffer("OK") != 513)  //And in mes we have "OK"                 
                     {  
                        return AT_WorkModeChange(3,0);    //Check mode value
                     }
                     else 
                        return 0;     //ESP8266 don't answer "OK"   
                   }
                   else 
                        return 0;     //Module don't answer
               }
         else 
              return 0; //Eror input value
       //END Case 2        

      case 3:                       //Check value
         
         ESP8266_SendStr("AT+CWMODE_CUR?\r\n");
      
         if(ESP8266_WaitInMess()) 
         {            
            //Answer:  " AT+CWMODE_CUR?\r\n\r\n+CWMODE_CUR:1
             // And we search ':' symbol pos, next return value('1')
            return g_ESP8266InBuffer[(ESP8266_SearchSymbolBuffer(':') + 1)];
         }        
         else
         {
            return 0;   //Module don't answer
         }
       //END Case 3   
   } //END Switch
   
   return 0; //Error input value
}


/*  Send "AT+CWJAP" to ESP8266, answer "OK"
*    This instruction connect module to WiFi, can't use in SoftAP mode
*   Input: SSID - wifi ssid, pass - wifi password
*   Output: '0'- error esp8266, '1' - Successful, '2' - another ERROR
*/
int AT_WiFiConnect(char* SSID, char* pass)
{
  ESP8266_RepealArrayWritePos();    //Repeal write pos
  ESP8266_NullInBuffer();       //Null input buffer
   
      //Comand structure: "AT+CWJAP="SSID","PASS"\r\n
  ESP8266_SendStr("AT+CWJAP_CUR=\"");    //Send command
  ESP8266_SendStr(SSID);                  //Send SSID   
  ESP8266_SendStr("\",\"");     
  ESP8266_SendStr(pass);                  //Send pass  
  ESP8266_SendStr("\"\r\n");              // End command 
   
  uint32_t sec = 20;    //wait answer 20 seconds
 for (; sec != 0; sec--)
	{
		for(int i = 0x5B8D80; i !=0; i--);										// 1 second delay if Fmax = 72 Mhz
      {
       if(g_UnreadMess  == true)
       {
          if(ESP8266_SearchStringBuffer("OK") != 513)  //And in mes we have "OK"                 
          return 1;     //Operation succesful
       else if(ESP8266_SearchStringBuffer("ER") != 513) //module answer "ERROR"                  
          return 0;     //ESP8266 don't answer "OK"   
       else
          return 2;  //another ERROR
          
       }
	  }
  }
            return 2;  //ESP8266_Don't Answer 
}


/*  Send "AT+CIPSTA" to ESP8266, answer "OK"
*    This instruction set module ip, gateway, and mask
*   Input: IP, gateway, mask
*   Output: '0'- error esp8266, '1' - Successful, '2' - another ERROR
*/
int AT_IPSet(char* IP, char* gateway, char* mask)
{
   ESP8266_RepealArrayWritePos();  //Repeal write pos
   ESP8266_NullInBuffer();       //Null input buffer
   
   //Comand structure: "AT+CIPSTA_CUR="ip","gateway","mask"\r\n"
   ESP8266_SendStr("AT+CIPSTA_CUR=\"");  //Send command
   ESP8266_SendStr(IP);                   //Send IP
   ESP8266_SendStr("\",\"");
   ESP8266_SendStr(gateway);              //Send gateway
   ESP8266_SendStr("\",\"");
   ESP8266_SendStr(mask);                 //Send mask
   ESP8266_SendStr("\"\r\n");             //End command
       
   if(ESP8266_WaitInMess())  //We have input message
    {
      if(ESP8266_SearchStringBuffer("OK") != 513)  //And in mes we have "OK"                 
         return 1;     //Operation succesful
      else if(ESP8266_SearchStringBuffer("ER") != 513) //module answer "ERROR"                  
         return 0;     //ESP8266 don't answer "OK"   
      else
         return 2;  //another ERROR
    }
    else
       return 2;  //another ERROR
            
}


/*  Send "AT+CIPSERVER" to ESP8266, answer "OK"
*    This instruction starts up TCP server, or restart him
*   Input: Mode: '1' - start, '0' - close
*          Port - servers port
*   Output: '0'- error esp8266, '1' - Successful, '2' - another ERROR
*/
int AT_TCPServerStart(char* Mode, char* Port)
{
   ESP8266_RepealArrayWritePos();   //Repeal write pos
   ESP8266_NullInBuffer();       //Null input buffer
   
   //Command structures: "AT+CIPSERVER=Mode,Port\r\n"
   ESP8266_SendStr("AT+CIPSERVER=");   //Send command
   ESP8266_SendStr(Mode);              //Mode
   ESP8266_SendStr(",");
   ESP8266_SendStr(Port);
   ESP8266_SendStr("\r\n");
   
   if(ESP8266_WaitInMess())  //We have input message
    {
      if(ESP8266_SearchStringBuffer("OK") != 513)  //And in mes we have "OK"                 
         return 1;     //Operation succesful
      else if(ESP8266_SearchStringBuffer("ER") != 513) //module answer "ERROR"                  
         return 0;     //ESP8266 don't answer "OK"   
      else
         return 2;  //another ERROR
    }
    else
       return 2;  //another ERROR
}


/*  Send "AT+CIPMUX=1" to ESP8266, answer "OK"
*    This instruction set single or multiple connection mode
*   Input: Mode: '1' - multiply, '0' - single
*   Output: '0'- error esp8266, '1' - Successful, '2' - another ERROR
*/
int AT_ConnectNumbChange(uint8_t Mode)
{
   ESP8266_RepealArrayWritePos();   //Repeal write pos
   ESP8266_NullInBuffer();       //Null input buffer
   
   ESP8266_SendStr("AT+CIPMUX=");   //Send command
   ESP8266_Send(Mode + 48);
   ESP8266_SendStr("\r\n");  

   if(ESP8266_WaitInMess())  //We have input message
    {
      if(ESP8266_SearchStringBuffer("OK") != 513)  //And in mes we have "OK"                 
         return 1;     //Operation succesful
      else if(ESP8266_SearchStringBuffer("ER") != 513) //module answer "ERROR"                  
         return 0;     //ESP8266 don't answer "OK"   
      else
         return 2;  //another ERROR
    }
    else
       return 2;  //another ERROR   
}


/*  Send "AT+CIPSEND" to ESP8266, answer "OK"
*    This instruction create conection to the TCP client
*   Input: Identifier - client identifier, Lenght - message lenght
*   Output: '0'- error esp8266, '1' - Successful, '2' - another ERROR
*/
int AT_TCPServerCreateConn(int Identifier,char* Lenght)
{
   ESP8266_RepealArrayWritePos();    //Repeal write pos
   ESP8266_NullInBuffer();       //Null input buffer

   ESP8266_SendStr("AT+CIPSEND=");   //Send command
   ESP8266_Send(Identifier);
   ESP8266_SendStr(",");
   ESP8266_SendStr(Lenght);
   ESP8266_SendStr("\r\n");
   
   if(ESP8266_WaitInMess())  //We have input message
    {
      if(ESP8266_SearchStringBuffer("OK") != 513)  //And in mes we have "OK"                 
         return 1;     //Operation succesful
      else if(ESP8266_SearchStringBuffer("ER") != 513) //module answer "ERROR"                  
         return 0;     //ESP8266 don't answer "OK"   
      else
         return 2;  //another ERROR
    }
    else
       return 2;  //another ERROR
}


/*  Send HTML code to ESP8266, answer "OK"
*   Input: HTML - HTML code
*   Output: '0'- error esp8266, '1' - Successful, '2' - another ERROR
*/
int AT_TCPServerSendHTMLWithCheck(char* HTML)
{
   ESP8266_RepealArrayWritePos();   //Repeal write pos
   ESP8266_NullInBuffer();       //Null input buffer
   
   ESP8266_SendStr(HTML);
   
   if(ESP8266_WaitInMess())  //We have input message
    {
      if(ESP8266_SearchStringBuffer("OK") != 513)  //And in mes we have "OK"                 
         return 1;     //Operation succesful
      else if(ESP8266_SearchStringBuffer("ER") != 513) //module answer "ERROR"                  
         return 0;     //ESP8266 don't answer "OK"   
      else
         return 2;  //another ERROR
    }
    else
       return 2;  //another ERROR
}


/*  Send HTML code to ESP8266
*   Input: HTML - HTML code
*   Output: 
*/
void AT_TCPServerSendHTML(char* HTML)
{ 
   ESP8266_SendStr(HTML);
}


/*  Send symbol to the HTML code to ESP8266
*   Input: HTML - HTML code
*   Output: 
*/
void AT_TCPServerSendHTMLSymb(char HTML)
{ 
   ESP8266_Send(HTML);
}
 
