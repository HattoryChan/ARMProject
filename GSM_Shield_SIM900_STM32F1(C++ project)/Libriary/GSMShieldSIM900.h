/*

Header File for GSMShieldSIM900

*/


int CheckGSMStatus(void); 			//Returned '0' if all so bed, '1' if module ready
int IncomingTrap(void);				  //Trap for incoming call
int CLIPSetting(int chose);			//Set CLIP parametrs.   chose:   '1' - On CLIP, '0' - Off CLIP, '2' - Check status. 
int SendMessage(int Format,char* Number, char* Text);   //Sending message for Number in structures 
int CMGFSetting(int chose);     // chose: '1' - AT+CMGF=1(Text Mode), '0' - AT+CMGF=0(UPD mode), '2' - check status
int CPASStatus(void);						//AT+CPAS - if '0' - ready, '2' -unknown, '3' - Ringing, '4' - Call in progress, '1' - ERROR
int CREGStatus(int param);			//Param - '0' - return first param, '1' - second param. Return '9' if ERROR	
int SendEmailToAuthSMTP(char* CID,char* WaitDelay, char* ServerName, char* Port,
							char* Login, char* Pass, char* SendEmail, char* SendName,
							char* FromEmail, char* FromName, char* Subject, char* Body);      // Send email to SMTP server with SSL and Authification, Time function
int NumberFind(char* Number);      //Find input number, return '1' if called know to us number
int GPRSSetupSetting(char* ContyteCont, char* APNCont, char* UserCont, char* PassCont);  			//Make GPRS Connections
int OKCheck(void);				//Check answer: "OK" in second string		
void ClearInputBuffer(void);      //Clear message buffer for New message
                     //FTP Work
int FTPServerConnection(char* IpServer, char* UserName, char* Pass);   //Set setting for FTP connection, return: '0' - error, '1' - OK
int FTPServerGETChoseFile(char * FileName, char * Path);    //FTP Server SET File and Path,return: '0' - ERROR, '1'- OK
int FTPServerGETCheckConnection();              //FTP Server check connections,return: '0' - error, '1' - OK    
int FTPServerGetData(char * ReadByte);          //Get 100 byte packet
int FTPServerNumberOfGetByte();                 //Find and returned number readed byte from FTPServer
int FTPServerPutChoseFile(char * FileName, char * Path); //FTP Server SET File and Path,return: '0' - ERROR, '1'- OK
int FTPServerPutCheckConnection();        //FTP Server check connections,return: '0' - error, '1' - OK
int FTPServerPutData(int ByteCount);      //FTP Server PUT data. Put ByteCount byte from MessFTP[100].MAX 100 BYTE! Return: '1' - it's OK, '0' - ERROR
                    //HTTP Work
int HTTPConnectInit(bool InitStatus, char* URL);   //HTTP conntection Init, InitStatus: '1' - Init, '0' - terminate
int HTTPGetMethod();    //HTTP Get Method, reading initializated HTTP page.          