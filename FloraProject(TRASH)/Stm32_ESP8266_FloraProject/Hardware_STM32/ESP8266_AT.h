/* Header for ESP8266_AT.cpp
*
*
*
*
*
*/

#include "stdbool.h"
#include "stm32f10x.h"
#include "ESP8266_SendUart.h"

/*=======================================================================
                           Work with In Buffer Part
========================================================================*/
int ESP8266_SearchStringBuffer(char* SearchTwoSymb); //Search Input string in ESP8266 AT input buffer
int ESP8266_SearchStringBufferWithPos(char* SearchTwoSymb, int StartSearchPos); //Search Input string in ESP8266 AT input buffer from the starting position

int ESP8266_SearchSymbolBuffer(char SearchSymb); //Search symbol in ESP8266 AT input buffer 
int ESP8266_SearchSymbolBufferWithPos(char SearchSymb, int StartSearchPos); //Search symbol in ESP8266 AT input buffer
int ESP8266_InConnectIdentifier(); //Searched message structure:  "+IPD,0,395:GET / HTTP/1.1...."


void ESP8266_RepealArrayWritePos(); //Repeal g_ESP8266ArrayWritePos and make position "0"
void ESP8266_NullInBuffer(); //Null g_ESP8266InBuffer
bool ESP8266_UnreadFlagStatus(); //Return g_UnreadMess flag status
void ESP8266_UnreadResetFlag(); //Reset g_UnreadMess flag
bool ESP8266_WaitInMess(); //Wait for a responce 10 second, if don't have - return error.
int ESP8266_ReturnSymbFromInBuff(uint8_t Pos); //Return symbol from g_ESP8266InBuffer

/*=======================================================================
                     Work with AT command part     
========================================================================*/
int AT_CheckReady(); //Check whether the module is ready
int AT_ResetModule(); //Reset module
int AT_WorkModeChange(uint16_t Mode, bool SaveToFlash); //This insctuction change working mod
int AT_WiFiConnect(char* SSID, char* pass); //This instruction connect module to WiFi
int AT_IPSet(char* IP, char* gateway, char* mask); //This instruction set module ip, gateway, and mask
int AT_TCPServerStart(char* Mode, char* Port); // This instruction starts up TCP server
int AT_ConnectNumbChange(uint8_t Mode); //This instruction set single or multiple connection mode
int AT_TCPServerCreateConn(int Identifier, char* Lenght); //his instruction create conection to the TCP client
int AT_TCPServerSendHTMLWithCheck(char* HTML); //Send HTML code to ESP8266, answer "OK"
void AT_TCPServerSendHTML(char* HTML); //Send HTML code to ESP8266
void AT_TCPServerSendHTMLSymb(char HTML); //Send symbol to the HTML code to ESP8266
