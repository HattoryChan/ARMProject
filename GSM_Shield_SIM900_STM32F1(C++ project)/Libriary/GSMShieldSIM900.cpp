/*
 
  AT command for SIM900. 
 
*/

#include "stm32f10x.h"
#include "Periphery_Init.h"                           // Periphery init
#include <string.h>												//strcmp library
#include <stdio.h>												//	sprintf
#include <stdlib.h>												//atoi
#include "delay.h"
#include "GSMShieldSIM900.h"


uint16_t RxStatus = 0;              //'1' - ready, '0' - busy, function must reset this value
uint16_t LenghtMess = 0;     	      // Array position value
uint16_t MessReadStatus = 0;	      //If last message read == '0'
uint16_t MessGSM[100];               //Array for Message from GMS module
uint16_t *pMessGSM = &MessGSM[0];  
uint16_t PreMessGsm;
uint16_t MessFTP[100];      //Array for message from FTP server
uint16_t MessHTTP[100];     //Array for message from HTTP page


//=============================================================  Usart1 Handler (RX interrupt)			Write Input message in MessGsm int array
extern "C"              //To see interrupt
{   
void USART1_IRQHandler(void)
{
	if (USART1->SR & USART_SR_RXNE)				//what causind interruption, if byte went to RxD - treats
	{	
		  USART1->SR &= ~USART_SR_RXNE;					//Reset Interription 
		  	RxStatus = 0;																//Set status to busy and write data to MessGSM
				PreMessGsm = USART1->DR;											//Write Usart data on variable
	  	MessGSM[LenghtMess] = PreMessGsm ;										//Write valiable on your place in array
		
	if( MessGSM[LenghtMess] == '\n')							//If we have "end symbol" in they Array,
		{															
			RxStatus = 1;															//SET '1' when first string complited, if we get second string, 
																								//      this value refresh and set to '1' when second string ready
		}
	     LenghtMess++;	                      //GSM array lenght, you must refresh this valiable in next function
	}
 }
}

//============================================================================= Clear message buffer for New message
void ClearInputBuffer(void)         
{
   LenghtMess = 0;                            //Clear message buffer for New message(RING,SMS)
		
}

//=============================================================================		 Check Module Status, Returned '0' if all so bed, '1' if module ready
int CheckGSMStatus()																
{
	LenghtMess = 0;																							//Write mode on start
	USART1_Send_String("AT\r");																				//AT Comand + CR('\r') symbol. Answer: "OK"
	// Input message structure: AT'\r''\r''\n'OK'\r''\n'  We need second string, so:     O -'0' symbol, K - '1' symbol
	
		while(RxStatus == 1);																												//Wait first string
		while(RxStatus == 0){};																									//first string ready
	LenghtMess = 0;																										//Message have 3 string, we need second string,so we clear first string
		while(RxStatus == 1){};																										//Wait secong string
		while(RxStatus == 0){};																											//second string ready
				
	while(RxStatus == 0);													//We have message on RX
	delay(1);					//Wait message complite
	if (MessGSM[0] == 'O' && MessGSM[1] == 'K')
	{
      LenghtMess = 0;			//Clear message buffer
		return 1;
	}	
	else	
				return 0;
}


//=============================================================================		 Trap for the incoming call, returned '1' if we have call
int IncomingTrap()
	{										//Input message strictures: '\r''\n'RING'\r''\n'    'R' - 2, 'G' - 5
	if(MessGSM[2] == 'R' && MessGSM[5] == 'G')	
	{				
		LenghtMess = 0;                 //Message be reading, and we clear array		
		return 1;
	} 
	else	
	return 0;	
}


//================================================================  			 Check and select AT+CLIP(Calling Line Identification Presentation) status 
int CLIPSetting(int chose)																									//chose:   '1' - On CLIP, '0' - Off CLIP, '2' - Check status
{																																						//If CLIP on, return '1', else '0'. Error '2'
	LenghtMess = 0;																														 //Clear mass lenght
	USART1_Send_String("AT+CLIP?\r");																	//Delay 2 second.asks status, Answer: "+CLIP: 1,1"
																	//Message structure:  "AT+CLIP?'\r''\r''\n'+CLIP: 1,1'\r''\n''\r''\n'OK'\r''\n'"  3 string, we need second string	
		while(RxStatus == 1);																												//Wait first string
		while(RxStatus == 0){};																									//first string ready
	LenghtMess = 0;																										//Message have 3 string, we need second string,so we clear first string
		while(RxStatus == 1){};																										//Wait secong string
		while(RxStatus == 0){};																											//second string ready
				
	if(MessGSM[1] == 'C' && MessGSM[4] == 'P' )						//If MessGSM contains +CLIP: n;m, when n - status value, second string: 'O' - 1, 'K' - 4 number in array
	{
		switch (chose)																															//Choose Action
		{
			case 0:										
						LenghtMess = 0;   															 //Write the first position
					USART1_Send_String("AT+CLIP=0\r");																		//Off CLIP
								//Message structure: "AT+CLIP=0'\r''\r''\n'OK'\r''\n'"			2 string, we need second string. So:	'O' - 0, 'K' - 1
						while(RxStatus == 1);																												//Wait first string
						while(RxStatus == 0){};																									//first string ready
					LenghtMess = 0;																										//Message have 3 string, we need second string,so we clear first string
						while(RxStatus == 1){};																										//Wait secong string
						while(RxStatus == 0){};																								//second string ready
							
			    if(MessGSM[0] == 'O' && MessGSM[1] == 'K')																						//Check status
							return 0;
						else
							return 2;																											//Action will be ending with ERROR
			
			case 1:
						LenghtMess = 0;   															 //Write the first position
					USART1_Send_String("AT+CLIP=1\r");																	//ON CLIP
								//Message structure: "AT+CLIP=1'\r''\r''\n'OK'\r''\n'"				2 string, we need second string. So: 'O' - 0, 'K' - 1
						while(RxStatus == 1);																												//Wait first string
						while(RxStatus == 0){};																									//first string ready
					LenghtMess = 0;																										//Message have 3 string, we need second string,so we clear first string
						while(RxStatus == 1){};																										//Wait secong string
						while(RxStatus == 0){};																											//second string ready

					if(MessGSM[0] == 'O' && MessGSM[1] == 'K')																					 //Check status
							return 1;	
						else
							return 2;																										//Action will be ending with ERROR
			
		  case 2:																																	//Check status CLIP
					//Message structure:  "+CLIP: 1,1'\r''\n''\r''\n'OK'\r''\n'"  '1' or '0' will be in 7 array position
					if(MessGSM[7] == '1')																									//CPIL ON
						return 1;
					else if(MessGSM[7] == '0')																						//CLIP OFF
						return 0;
					else
						return 2;																														//CLIP Status ERROR
			default:
						return 2;																										//ERROR chose
    }   
	}
	else																																				//Error Invite Message
		return 2;
}


//====================================================================     Check,select SMS Format(text,UPD) and Send Message. Before you need create SendSms structure 
int SendMessage(int Format, char* Number, char* Text)						//return '1' if send complited, '0' if bad sending, '2' if we have another Error
{																																												//Format - '1' - text, '0' - UPD(UPD mode not work) 
																											//====== Function will be send TEXT message ONLY!
	if(CMGFSetting(Format) == 1)																	//If Text format -> Send message
	{
		USART1_Send_String("AT+CMGS=\"");																	//AT+CMGS="number"'\r'
		USART1_Send_String(Number);
		USART1_Send_String("\"\r");																				
			delayms(400);																											// Delay for module be ready to send								
		USART1_Send_String(Text);																				//Add Text into message
			delayms(500);																								//Delay for module be ready send message
		
		while(!(USART1->SR & USART_SR_TC)){};								//Send message
						USART1->DR = 0x1a;													//Ctrl+Z			
		
		//Return Message structure: +CMGS: <n>'\r''\n''\r''\n'OK            Have two string we need second string. "OK"			
					LenghtMess = 0;																																							//Wait first string
						delayms(100);
			
	if(MessGSM[10] == 'K' || MessGSM[11] == 'K' || MessGSM[12] == 'K' || MessGSM[13] == 'K')				// <n> may have several symbol 
	{		
		return 1;																															//It's OK
	}
	else 
		return 2;																																		//ERROR
	} 	
	else if(CMGFSetting(Format) == 0)													//If UPD format NOT WORK!!!!!!!
	{
			SendMessage(1,Number,Text);															//SET TEXT format, new value for input value		
		return 1;
	}
	else																											//Else -> ERROR
	   return 2;		 		 
}


//=====================================================================     Check and select CMGF(Text or UPD format in message) status.
int CMGFSetting(int chose)    																							// chose: '1' - AT+CMGF=1(Text Mode), '0' - AT+CMGF=0(UPD mode), '2' - check status
{																																							//If CMGF text mode on, return '1', else '0'. Error '2'	
	LenghtMess = 0;																												//We need new message. Refresh lenght array
	USART1_Send_String("AT+CMGF?\r");																//asks status. Answer: "+CMGF: 1"
																						//Return message structure:  AT+CMGF?'\r''\r''\n'+CMGF: 1'\r''\n''\r''\n'OK'\r''\n' 
																								// Have five string. We need "+CMGF: 1", so we need second string
						while(RxStatus == 1);																												//Wait first string
						while(RxStatus == 0){};																									//first string ready
					LenghtMess = 0;																										//Message have 5 string, we need second string,so we clear first string
						while(RxStatus == 1){};																										//Wait secong string
						while(RxStatus == 0){};																								//second string ready
							
	
	if(MessGSM[0] == '+' && MessGSM[4] == 'F')											//We have string: "+CMGF: 1   so '+' - 0, 'F' - 4, '1' - 7
	{
		switch (chose)
		{
			case 0:																												//On UPD mode
					USART1_Send_String("AT+CMGF=0\r");									//Add new request
			delayms(600);																								//Wait Answer
						if(CMGFSetting(2) == 0)																			//Check status
							return 0;																									//It's OK
						else 
							return 2;																									//Another error
		  case 1:																											  //On Text mode
				  USART1_Send_String("AT+CMGF=1\r");		
			delayms(600);																												//Wait Answer
			      if(CMGFSetting(2) == 1)																			//Check Status
							return 1;																									//It's OK
						else
							return 2;																									//Another error
			case 2:																												//Check status
						if(MessGSM[7] == '1')																			//If Text mode
							return 1;
						else if(MessGSM[7] == '0')																//If UPD mode
							return 0;
						else 
							return 2;																								//Error
			default:
							return 2;
		}
	}
		else
			return 2;																											  //Error		
}


//====================================================================  Check the module doing status
int CPASStatus()																										//AT+CPAS - if '0' - ready, '2' -unknown, '3' - Ringing, '4' - Call in progress, '1' - ERROR
{
	USART1_Send_String("AT+CPAS\r");												//Answer: "+CPAS: 0"
												//Message strustures:AT+CPAS'\r''\r''\n'+CPAS: 0'\r''\n''\r''\n'OK'\r''\n' . We nees second string, so:	
						while(RxStatus == 1);																												//Wait first string
						while(RxStatus == 0){};																									//first string ready
					LenghtMess = 0;																										//Message have 5 string, we need second string,so we clear first string
						while(RxStatus == 1){};																										//Wait secong string
						while(RxStatus == 0){};																								//second string ready
																						//Second string:   "+CPAS: 0'\r''\n'"      '+' - 0, 'S' -4, '0' - 7
	if(MessGSM[0] == '+' && MessGSM[4] == 'S')						//Check message
	{
		if(MessGSM[7] == '0')	  																											//Module Ready
			return 0;		
		else if(MessGSM[7] == '2')																									  //Module status Unknown
			return 2;
		else if(MessGSM[7] == '3')																									  //Module "RING", input call
			return 3;
		else if(MessGSM[7] == '4')																										//Module busy input calling, Call on line
			return 4;
		else																																				//ERROR	
			return 1;																					
	} 
	else																																					//ERROR
		  return 1;
}


//====================================================================   Check state of the mobile network    
int CREGStatus(int param)																												//Param - '0' - return first param, '1' - second param			
{																												//First param:  '0' - no mobile network code, '1' - mobile code, '2' - mobile code + extra options
																												//Second param: '0' - search mobile network, '1' - registration, home mobile network
																												//              '2' - not registration, search new mobile network, '3' - registration disallow
																												//  						'4' - unknown, '5' - roaming
																												//Return '9' if have ERROR
																				
	USART1_Send_String("AT+CREG?\r");																	//Answer:  "+CREG: 0,1"
																						//Message structure:  "AT+CREG?'\r''\r''\n'+CREG: 0,1'\r''\n''\r''\n'OK'\r''\n'" 
																							// Message have third string, we need second. So:
						while(RxStatus == 1);																												//Wait first string
						while(RxStatus == 0){};																									//first string ready
					LenghtMess = 0;																										//Message have 5 string, we need second string,so we clear first string
						while(RxStatus == 1){};																										//Wait secong string
						while(RxStatus == 0){};																								//second string ready	
																												//Second string: "+CREG: 0,1'\r''\n''\r''\n'OK'\r''\n'" '+' - 0, 'G' - 4, '0' - 7, '1' - 9
	if(MessGSM[0] == '+' && MessGSM[4] == 'G')				//Check Message
	{
		switch (param)																																	//Chose check parametr
		{
			case 0:																																//If we check first param
				if(MessGSM[7] == '0')																							    //No mobile network
				  return 0;																											
				else if(MessGSM[7] == '1')																						//Mobile code			
					return 1;
				else if(MessGSM[7] == '2')																						//Mobile Code + options
					return 2;
				else																																//ERROR
					return 9;																											
			case 1:																																//If we check second param
				if(MessGSM[9] == '0')																									//Search
				  return 0;
				else if(MessGSM[9] == '1')																						//Registration
					return 1;	
				else if(MessGSM[9] == '2')																						//Not registration
					return 2;
				else if(MessGSM[9] == '3')																						//Register disallow
					return 3;
				else if(MessGSM[9] == '4')																						//Unknown
					return 4;
				else if(MessGSM[9] == '5')																						//Roaming
					return 5;
				else																																//ERROR
					return 9;	
			default:																															//ERROR
					return 9;
		}
	}
  else																																			//Input Message ERROR
		    	return 9;
}



//==================================================================  Find the caller number
int NumberFind(char* Number)														//When we have input calling this function can find his number,
{																																	//and return '1' - if number identified, '2' - error
																													//Input message structures: '\r''\n'RING'\r''\n''\r''\n'+CLIP: "+79193286730",145,"",,"",0'\r''\n''\r''\n'
																													// '+' - 10, 'P' - 14, Start number: '+' - 18
 int i;																								//For cycle (for)
 int cout = 0;																					//For count true value in number
	
 
 if(MessGSM[2] == 'R' && MessGSM[5] == 'G')																					//Trap for input Called
	{	 
		
		if(MessGSM[10] == '+' && MessGSM[14] == 'P' && MessGSM[18] == '+')					//If we have needed input message
		{		
			for(i=0; i <= 11; i++)																							//input number equals with Added number
			{
				if(Number[i] == MessGSM[18+i])																		//If value is
				{
					cout++;																														//cout +=1
				}
			}					
		}		
		
			if(cout == 12)																									//If All 12 symbol equals
			{
				LenghtMess = 0;																										//clear Mess Array
				return 1;																									//It's OK
			}
			else																												//ERROR
			{				
				return 0;
			}
	} 
 else																														//ERROR, we dont have needed message
	{	      
			return 2;	
	}
}

//=======================================================================  GPRS Connection setting
int GPRSSetupSetting(char* ContyteCont, char* APNCont, char* UserCont, char* PassCont)
{																																																//SET connections to GPRS setting, return '1' - of all OK, '0' - if error on connect to Gprs(last command)
																																																							//'2' - Enother Error
	
		USART1_Send_String("AT+SAPBR=0,1\r");										//Disconnect to gprs
		delay(3);																														//Delay to wait Disconnect
	
		LenghtMess = 0;
	  USART1_Send_String("AT+SAPBR=3,1,\"CONTYPE\",\"");															//Sett CONTYPE 
	  USART1_Send_String(ContyteCont);
	  USART1_Send_String("\"\r");
																													//	Wait Answer "OK" part code
						while(RxStatus == 1);																												//Wait first string
						while(RxStatus == 0){};																									//first string ready
					LenghtMess = 0;																										//Message have 3 string, we need second string,so we clear first string
						while(RxStatus == 1){};																										//Wait secong string
						while(RxStatus == 0){};																											//second string ready
														
		if(MessGSM[0] == 'O' && MessGSM[1] == 'K')																					 //If CONTYPE Ready ->
		{			
						delayms(500);																																//delay for Rdy module
			
						LenghtMess = 0;
						USART1_Send_String("AT+SAPBR=3,1,\"APN\",\"");															//Sett APN
						USART1_Send_String(APNCont);
						USART1_Send_String("\"\r");
						
																								//	Wait Answer "OK" part code
						while(RxStatus == 1);																												//Wait first string
						while(RxStatus == 0){};																									//first string ready
					LenghtMess = 0;																										//Message have 3 string, we need second string,so we clear first string
						while(RxStatus == 1){};																										//Wait secong string
						while(RxStatus == 0){};																											//second string ready
												
			if(MessGSM[0] == 'O' && MessGSM[1] == 'K')																					 //If APN Ready ->
			{
						delayms(500);																										//delay for Rdy module
				
						LenghtMess = 0;
						USART1_Send_String("AT+SAPBR=3,1,\"USER\",\"");															//Sett Username
						USART1_Send_String(UserCont);
						USART1_Send_String("\"\r");
						 
						//	Wait Answer "OK" part code
						while(RxStatus == 1);																												//Wait first string
						while(RxStatus == 0){};																									//first string ready
					LenghtMess = 0;																										//Message have 3 string, we need second string,so we clear first string
						while(RxStatus == 1){};																										//Wait secong string
						while(RxStatus == 0){};																											//second string ready
				if(MessGSM[0] == 'O' && MessGSM[1] == 'K')																					 //If Username Set ready Ready ->
					{
						delayms(500);																												//delay for Rdy module
						
						LenghtMess = 0;
						USART1_Send_String("AT+SAPBR=3,1,\"PWD\",\"");															//Sett Password
						USART1_Send_String(PassCont);
						USART1_Send_String("\"\r");
						  
					if(MessGSM[0] == 'O' && MessGSM[1] == 'K')																					 //If Password Set ready Ready ->
					{
							delayms(500);																											//delay for Rdy module
						
							LenghtMess = 0;																												//try conn to GPRS
							USART1_Send_String("AT+SAPBR=1,1\r");
																												//	Wait Answer "OK" part code
							while(RxStatus == 1);																												//Wait first string
							while(RxStatus == 0){};																									//first string ready
						LenghtMess = 0;																										//Message have 3 string, we need second string,so we clear first string
							while(RxStatus == 1){};																										//Wait secong string
							while(RxStatus == 0){};																											//second string ready					
								
						if(MessGSM[0] == 'O' && MessGSM[1] == 'K')																					 //If Connect ready ->
					{
						delay(1);																												//delay for Rdy connect to mobile network
						
						LenghtMess = 0;
						USART1_Send_String("AT+SAPBR=2,1\r");
																						//Input message structure: AT+SAPBR=2,1'\r''\r''\n'+SAPBR: 1,1,"10.42.39.52"'\r''\n''\r''\n'OK'\r''\n'
																																//We need second string, so:																													
							while(RxStatus == 1);																												//Wait first string
							while(RxStatus == 0){};																									//first string ready
						LenghtMess = 0;																										//Message have 3 string, we need second string,so we clear first string
							while(RxStatus == 1){};																										//Wait secong string
							while(RxStatus == 0){};																											//second string ready	
					
							if(MessGSM[0] == '+' && MessGSM[5] == 'R')															//We have needed message
							{
								if(MessGSM[10] != '3' && MessGSM[10] != '2')											//GPRS NOT closed(3) and dont closing(2)
								{
									delay(2);																											//Wait ended connections
									return 1;
								}
							}
							else																																		//GPRS DONT connected(error on connections)	
								return 2;																		
								
					}	
						else																					  //GPRS DONT connected(error on answer last message)
							     return 0;

					}
					else
						return 2;
				}
				else
					 return 2;
						
			}
			else
							return 2;																										//Action will be ending with ERROR
		}
	else
							return 2;		
	return 2;
}

//===============================================================================			Check Answer OK Function
int OKCheck()																													//Check "OK" message, ignored first sting and search "OK" in second
{
//Message structure: "AT+XXXX=XX'\r''\r''\n'OK'\r''\n'"				2 string, we need second string. So: 'O' - 0, 'K' - 1
						while(RxStatus == 1);																												//Wait first string
						while(RxStatus == 0){};																									//first string ready
					LenghtMess = 0;																										//Message have 3 string, we need second string,so we clear first string
						while(RxStatus == 1){};																										//Wait secong string
						while(RxStatus == 0){};																											//second string ready

					if(MessGSM[0] == 'O' && MessGSM[1] == 'K')																					 //Check status
							return 1;	
						else
							return 2;																										//Action will be ending with ERROR

}	

//============================================================================      Sending Email Function, '1' if mail be sending, 
int SendEmailToAuthSMTP(char* CID,char* WaitDelay, char* ServerName, char* Port,												//  '0' if error in sending message, '2' another error
							char* Login, char* Pass, char* SendEmail, char* SendName,
							char* FromEmail, char* FromName, char* Subject, char* Body)
{
	LenghtMess = 0;													//clear input message
	USART1_Send_String("AT+EMAILSSL=1\r");									//SET parameter	SSL (enable)
	
	if(OKCheck() == 1)  								//We have "OK" answer
	{
		USART1_Send_String("AT+EMAILCID=");									
		USART1_Send_String(CID);
		USART1_Send_String("\r");												//SET parameter EMAILCID
		
	 if(OKCheck() == 1)
	 {
		 USART1_Send_String("AT+EMAILTO=");									
		 USART1_Send_String(WaitDelay);
	   USART1_Send_String("\r");												//SET parameter EMAILTO
		 
		if(OKCheck() == 1)
		{
			USART1_Send_String("AT+SMTPSRV=\"");									
			USART1_Send_String(ServerName);
			USART1_Send_String("\",");
			USART1_Send_String(Port);
			USART1_Send_String("\r");												//SET SMTP server and Port
			
		 if(OKCheck() == 1)
		 {
			 USART1_Send_String("AT+SMTPAUTH=1,\"");	
		   USART1_Send_String(Login);
			 USART1_Send_String("\",\"");
			 USART1_Send_String(Pass);
			 USART1_Send_String("\"\r");											//SET Login/Pass to autification to SMPT server
			 
			if(OKCheck() == 1)
			{
				USART1_Send_String("AT+SMTPFROM=\"");		
				USART1_Send_String(FromEmail);
				USART1_Send_String("\",\"");
				USART1_Send_String(FromName);
				USART1_Send_String("\"\r");											//SET EMAIL/NAme sending face	
				
			 if(OKCheck() == 1)
			 {
				 USART1_Send_String("AT+SMTPRCPT=0,0,\"");		
				 USART1_Send_String(SendEmail);
				 USART1_Send_String("\",\"");
				 USART1_Send_String(SendName);
			   USART1_Send_String("\"\r");										//Recipient of the letter
				 
				if(OKCheck() == 1)
				{
					USART1_Send_String("AT+SMTPSUB=\"");	
					USART1_Send_String(Subject);
					USART1_Send_String("\"\r");										//SET subject for email
					
				 if(OKCheck() == 1)
         {
					 USART1_Send_String("AT+SMTPBODY\r");
						delayms(300);																	//Wait Module ready
					 USART1_Send_String(Body);											// Write Email Body
						delayms(300);																	//Wait Module ready
					 
			while(!(USART1->SR & USART_SR_TC)){};								//End of the body
				USART1->DR = 0x1a;													//Ctrl+Z			
				    delay(1);
				
				  if(MessGSM[0] == 'O' && MessGSM[1] == 'K')																					 //Check status
					{
						USART1_Send_String("AT+SMTPSEND\r");					// SEND MESSAGE
			
																		//Message structure: "OK'\r''\n''\r''\n'+SMTPSEND: 1'\r''\r''\n'OK'\r''\n'"				2 string, we need second string. So: '+' - 6, '1' - 17, 'S' - 7
				    while(RxStatus == 1){};																										//Wait secong string
						while(RxStatus == 0){};																											//second string ready						
							LenghtMess = 0;																										//Message have 3 string, we need second string,so we clear first string
						while(RxStatus == 1){};																										//Wait secong string
						while(RxStatus == 0){};																											//second string ready				//We passed "OK'\r''\n'"
						while(RxStatus == 1){};																										//Wait secong string
						while(RxStatus == 0){};																											//second string ready					//And Passed "'\r''\n'"
			
					 if(MessGSM[6] == '+' && MessGSM[7] == 'S')																					 //If answer good
					{										
								delayms(100);
						
						 if(MessGSM[17] == '1')																																//and we have success('1')
						     return 1;																																		//It's OK
             else
							  return 0;																																//Error in sending message
					}
					else
						return 2;																																			//Another Error
				}
					else
						return 2;
				//	}
			//		else
				//		return 2;
				 }
				 else
					 return 2;
				}
        else
					return 2;
			 }
       else
				 return 2;
			}
      else
				return 2;
		 }
     else
      return 2;			 
		}
	  else
	    return 2;	
	 }
   else
     return 2;		 
		
	}
	else 
		return 2;
}	

//=============================================================      Set setting for FTP connection, return: '0' - error, '1' - OK
int FTPServerConnection(char* IpServer, char* UserName, char* Pass)
{
   LenghtMess = 0;
   USART1_Send_String("AT+FTPCID=1\r");            //Check FTP CID
    if(OKCheck() == 2)                          //if we dont get OK - error message "0"
   	    return 0;
    
   LenghtMess = 0; 
	USART1_Send_String("AT+FTPSERV=\"");	 // Set IP Server														
   USART1_Send_String(IpServer);
   USART1_Send_String("\"\r"); 
    if(OKCheck() == 2)
         return 0;
   
   LenghtMess = 0;
   USART1_Send_String("AT+FTPUN=\"");     //Set  Login
   USART1_Send_String(UserName);    
   USART1_Send_String("\"\r");
    if(OKCheck() == 2)
         return 0;

   LenghtMess = 0;
   USART1_Send_String("AT+FTPPW=\"");     //Set Password
   USART1_Send_String(Pass);
   USART1_Send_String("\"\r"); 
    if(OKCheck() == 2)
         return 0;
    
    return 1;           // It's OK
}    

//==============================================================     FTP Server SET File and Path,return: '0' - ERROR, '1'- OK
int FTPServerGETChoseFile(char * FileName, char * Path)
{
   LenghtMess = 0;
   USART1_Send_String("AT+FTPGETNAME=\"");   //Set file Name on FTP server
   USART1_Send_String(FileName);
   USART1_Send_String("\"\r");
    if(OKCheck() == 2)
         return 0;
    
   LenghtMess = 0; 
   USART1_Send_String("AT+FTPGETPATH=\"");     //Select Path on file
   USART1_Send_String(Path);
   USART1_Send_String("\"\r");
    if(OKCheck() == 2)
         return 0;
   
    return 1;       // It's OK    
}

//==============================================================   FTP Server check connections,return: '0' - error, '1' - OK
int FTPServerGETCheckConnection()
{
    LenghtMess = 0;
   USART1_Send_String("AT+FTPGET=1\r");            //Create connections with FTP server
    if(OKCheck() == 2)
         return 0;
                        //Wait answer
     while(RxStatus == 1);																												//Wait first string
	  while(RxStatus == 0){};																									//first string ready
       LenghtMess = 0;																										//Message have 3 string, we need second string,so we clear first string
	  while(RxStatus == 1){};																										//Wait secong string
	  while(RxStatus == 0){};	    
                  //Returned from GSM module Message structure: "+FTPGET:1,1"   '+' - 0, 'F' - 1, '1' -0, '1' - 10  
    if(MessGSM[0] != '+' && MessGSM[1] != 'F' && MessGSM[10] != '1')    //If we get anither message  
         return 0;    // Error
    
    return 1;   // It's OK
}

//============================================================== Get 100 byte packet
int FTPServerGetData(char * ReadByte)
{
    LenghtMess = 0;
   USART1_Send_String("AT+FTPGET=2,");          //Select number of byte
   USART1_Send_String(ReadByte);
   USART1_Send_String("\r");
   
   //Wait answer, Answer structure: "AT+FTPGET=2,[ReadByte]'\r''\n''\r''\n'+FTPGET:2,101[FTPMESSAGE]", three string in message
     while(RxStatus == 1);																									//Wait first string
	  while(RxStatus == 0){};																								//first string ready
       LenghtMess = 0;																										//Message have 3 string, we need second string,so we clear first string
	  while(RxStatus == 1){};																								//Wait secong string
	  while(RxStatus == 0){};	
       LenghtMess = 0; 
   if(MessGSM[0] != '+' && MessGSM[1] != 'F' && MessGSM[8] != '2')         //If we get another message
        return 0; //Error
   
   for(int i = 0; i < 100; i++)
   {
      MessFTP[i] = MessGSM[i];
   }
       return 1;        //It's OK
}

//===============================================================  Find and returned number readed byte from FTPServer
int FTPServerNumberOfGetByte()         
{
   return ((MessFTP[9] * 100) + (MessFTP[10] * 10) + MessFTP[11]);
}

//==============================================================     FTP Server SET File and Path,return: '0' - ERROR, '1'- OK
int FTPServerPutChoseFile(char * FileName, char * Path)
{
   LenghtMess = 0;
   USART1_Send_String("AT+FTPPUTNAME=\"");   //Set file Name on FTP server
   USART1_Send_String(FileName);
   USART1_Send_String("\"\r");
    if(OKCheck() == 2)
         return 0;
    
   LenghtMess = 0; 
   USART1_Send_String("AT+FTPPUTPATH=\"");     //Select Path on file
   USART1_Send_String(Path);
   USART1_Send_String("\"\r");
    if(OKCheck() == 2)
         return 0;
   
    return 1;       // It's OK    
}

//==============================================================   FTP Server check connections,return: '0' - error, '1' - OK
int FTPServerPutCheckConnection()
{
    LenghtMess = 0;
   USART1_Send_String("AT+FTPPUT=1\r");            //Create connections with FTP server
    if(OKCheck() == 2)
         return 0;
                        //Wait answer
     while(RxStatus == 1);																												//Wait first string
	  while(RxStatus == 0){};																									//first string ready
       LenghtMess = 0;																										//Message have 3 string, we need second string,so we clear first string
	  while(RxStatus == 1){};																										//Wait secong string
	  while(RxStatus == 0){};	    
                  //Returned from GSM module Message structure: "+FTPGET:1,1"   '+' - 0, 'F' - 1, '1' -0, '1' - 10  
    if(MessGSM[0] != '+' && MessGSM[1] != 'F' && MessGSM[10] != '1')    //If we get another message  
         return 0;    // Error
    
    return 1;   // It's OK
}

//============================================================== FTP Server PUT data. Put ByteCount byte from MessFTP[100].MAX 100 BYTE! Return: '1' - it's OK, '0' - ERROR
int FTPServerPutData(int ByteCount, uint16_t MessInFTP[100])
{
    LenghtMess = 0;
   USART1_Send_String("AT+FTPPUT=2,");
   USART1_Send_String((char*)ByteCount);
   USART1_Send_String("\r");
   
   for(int i=0; i < ByteCount; i++)
   {
      USART1_Send(MessInFTP[i]);
   }
   
   USART1_Send_String("AT+FTTPPUT=2,0\r");
   
   return 1;   
}

//============================================================== HTTP conntection Init, InitStatus: '1' - Init, '0' - terminate
int HTTPConnectInit(bool InitStatus, char* URL)                              //     return: '1' - OK, '0' - Error
{
   if(InitStatus == 1)
    {
      LenghtMess = 0;
      USART1_Send_String("AT+HTTPINIT\r");      //Init Http service
       if(OKCheck() == 2)                          // If module don't answer
         return 0;                                   //Error
       
      USART1_Send_String("AT+HTTPPARA=\"CID\",1\r");    //Send parameters for HTTP sessions
       if(OKCheck() == 2)                          // If module don't answer
         return 0;                                   //Error
       
      USART1_Send_String("AT+HTTPPARA=\"URL\",\"");   //Send URL
      USART1_Send_String(URL);
      USART1_Send_String("\"\r");       
       if(OKCheck() == 2)                          // If module don't answer
         return 0;                                  //Error
       
       return 1;           //It's OK
    } 
   if(InitStatus == 0)
    {
     LenghtMess = 0;
     USART1_Send_String("AT+HTTPTERM\r");         //Send HTTP terminate Command   
      
      return 1;    // It's OK
    }
   else
         return 0;                     //Error
}

//============================================================== HTTP Get Method, reading initializated HTTP page.
int HTTPGetMethod()                                              //Put text to MessHTTP(+HTTPREAD:xxx /r/n "HTML text")
{                                                                // return: '1' - It's OK, '0' -Error
   LenghtMess = 0;
   USART1_Send_String("AT+HTTPACTION=0\r");       //Get Session Start
    if(OKCheck() == 2)                          // If module don't answer
      return 0;                                  //Error
                      //Wait answer
     while(RxStatus == 1);																												//Wait first string
	  while(RxStatus == 0){};																									//first string ready
       LenghtMess = 0;																										//Message have 2 string, we need second string,so we clear first string
	  while(RxStatus == 1){};																										//Wait secong string
	  while(RxStatus == 0){};	                  //Message structure "+HTTPACTION:0,302,258:"
    if(MessGSM[0] !='+' && MessGSM[1] != 'H' && MessGSM[3] != 'T')     //If we have ANOTHER message
       return 0;                                            //ERROR

    USART1_Send_String("AT+HTTPREAD\r");
    for(int i = 0; i < 100; i++)
   {
      MessHTTP[i] = MessGSM[i];
   }
    return 1;           //It's OK   
}
