/*Input packet structure: 8266 1 30 365 60 50 33651
 *     When: 8266 - head, 1 - packet prefix, 30 - temp, 
 *           365 - day, 60 - hour, 50 - minute
 *           33651 - CRC
 *    CRC calculate: 
 *    [first symbol temp][first symbol day][first symbol hour][first symbol minute][packet prefix]       
 * Prefix MAP: 1 - get temp[2 symb], day[3 symb], hour[2 symb], minute[2 symb]
 *             2 - get water level[2 symb], unknown[3 symb],unknown[2 symb],unknown[2 symb]
 *  Answer: CRC
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

bool Serial_SearchInputMess(); //Search data in Serial Port
void HTML_PageRefresh();  //HTML_PageRefresh
bool Serial_CRCMessCheck(); //CRC message check
bool Serial_AdressMessCheck(); //Check Adress message
void HTML_HomePage();  //HTML_HomePage

void SearchPrefixOneMess();  //Search prefix One mess in buffer
void SearchPrefixTwoMess();  //Search prefix Two mess in buffer
void SearchPrefixThirdMess();  //Search prefix Third mess in buffer
void SearchPrefixFourthMess();  //Search prefix Fourth mess in buffer
void SearchPrefixFifthMess();  //Search prefix Fifth mess in buffer

MDNSResponder mdns;

// Replace with your network credentials
const char* ssid = "ASUS";
const char* password = "13062110";

ESP8266WebServer server(80);

String webPage = "";

int g_InMessReady = 0; // if mess ready '1', if not '0'
int g_Temp = 1;
int g_TimeDay,g_TimeHour,g_TimeMin;
int g_WaterLevel;
int g_WateringCountAll;
int g_LastWateringMin;
int g_WateringCountPerDay;
int g_DayLengthHour; 
int g_WaterConsPerDay;
int g_WaterGNDLengthSec;
int g_FreezingWaterState;
int g_HeatingWaterState;
int g_LastFreezingDay, g_LastFreezingHour, g_LastFreezingMin;
int g_LastTankFeelingHour, g_LastTankFeelingMin;
int g_LastWaterAirHour, g_LastWaterAirMin;
int g_LightState;

byte g_aInMessData[18];

void setup() 
{
  
  HTML_HomePage();     //create home page
 
                                              //END INIT
  delay(1000);
  Serial.begin(9600);
  WiFi.begin(ssid, password);             //CONECT TO WIFI Function
  Serial.println("");
                                  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
 //   Serial.print(".");                  //Write "." to COM Port
  }
 // Serial.println("");
//  Serial.print("Connected to ");
//  Serial.println(ssid);
//  Serial.print("IP address: ");
 // Serial.println(WiFi.localIP());
 
  if (mdns.begin("esp8266", WiFi.localIP())) 
  {            //DNS
  //  Serial.println("MDNS responder started");
  }
  
  server.on("/", [](){
    server.send(200, "text/html", webPage);
  });
  server.on("/refresh", [](){                               //If Button 1 created "/socket1On" page
    HTML_PageRefresh();                                    //Get new data to HTML
    server.send(200, "text/html", webPage);                   //and Create "IP/socketON" page
   // g_Temp++;
   
    delay(1000);
  });
  
   server.begin();   
  Serial.println("00");   //Module ready
}
            //we have input array from serial port, need separate him and add to HTML
void loop() 
{
 server.handleClient();  
 if(Serial_SearchInputMess() == true)
 {  //8266 1 30 365 60 50 33651
 
  if(Serial_CRCMessCheck() == true)
  {
    Serial.print(g_aInMessData[14]);  //CRC answer
    Serial.print(g_aInMessData[15]);
    Serial.print(g_aInMessData[16]);
    Serial.print(g_aInMessData[17]);    
    Serial.println(g_aInMessData[18]);

   SearchPrefixOneMess();
   SearchPrefixTwoMess(); 
   
    delay(1000);  
  }
 }
 //Serial.println(g_aInMessData[4]); //Last symbol before 8266_
 //delay(100);
}



/* ==============================================================================================================
 * ====================================  Search prefix In Mess Part  ============================================
 ==============================================================================================================*/
 
/* 
 *  Search and set data from Prefix One mess
 */
void SearchPrefixOneMess()
{
 if(g_aInMessData[4] == 1)
    {
     g_Temp = g_aInMessData[5]*10 + g_aInMessData[6];
     g_TimeDay = g_aInMessData[7]*100 + g_aInMessData[8]*10 + g_aInMessData[9];
     g_TimeHour = g_aInMessData[10]*10 + g_aInMessData[11];
     g_TimeMin = g_aInMessData[12]*10 + g_aInMessData[13];
    }
}

/* 
 *  Search and set data from Prefix Two mess
 */
void SearchPrefixTwoMess()
{
 if(g_aInMessData[4] == 2)
    {      
     g_WaterLevel = g_aInMessData[5]*10 + g_aInMessData[6];
     g_WateringCountAll = g_aInMessData[7]*100 + g_aInMessData[8]*10 + g_aInMessData[9];
     g_LastWateringMin = g_aInMessData[10]*10 + g_aInMessData[11];
     g_WateringCountPerDay = g_aInMessData[12]*10 + g_aInMessData[13]; 
    }
}

/* 
 *  Search and set data from Prefix Third mess
 */
void SearchPrefixThirdMess()
{
 if(g_aInMessData[4] == 3)
    {      
     g_FreezingWaterState = g_aInMessData[5];
     g_HeatingWaterState = g_aInMessData[6];
     g_LastFreezingDay = g_aInMessData[7]*100 + g_aInMessData[8]*10 + g_aInMessData[9];
     g_LastFreezingHour = g_aInMessData[10]*10 + g_aInMessData[11];
     g_LastFreezingMin = g_aInMessData[12]*10 + g_aInMessData[13]; 
    }
}

/* 
 *  Search and set data from Prefix Fourth mess
 */
void SearchPrefixFourthMess()
{
 if(g_aInMessData[4] == 4)
    {
     g_WaterConsPerDay = g_aInMessData[5]*10 + g_aInMessData[6];
     g_LightState = g_aInMessData[7];
     g_DayLengthHour = g_aInMessData[8]*10 + g_aInMessData[9];
     g_LastWaterAirHour = g_aInMessData[10]*10 + g_aInMessData[11];
     g_LastWaterAirMin = g_aInMessData[12]*10 + g_aInMessData[13];
    }
}

/* 
 *  Search and set data from Prefix fifth mess
 */
void SearchPrefixFifthMess()
{
 if(g_aInMessData[4] == 5)
    {
     g_WaterGNDLengthSec = g_aInMessData[5]*10 + g_aInMessData[6];
     //g_TimeDay = g_aInMessData[7]*100 + g_aInMessData[8]*10 + g_aInMessData[9];  //unknown 
     g_LastTankFeelingHour = g_aInMessData[10]*10 + g_aInMessData[11];
     g_LastTankFeelingMin = g_aInMessData[12]*10 + g_aInMessData[13];
    }
}




/* ==============================================================================================================
 * ======================================        HTML part          =============================================
 ==============================================================================================================*/

  
/*   HTML_HomePage
 * 
 */
void HTML_HomePage()
{
  webPage = "";
  webPage += "<html><head><meta charset=\"utf-8\"><title>Aeroponics</title><style>body{background: #333;";
  webPage +="color: #fc0;}</style>";
  
  webPage += "<h1>ESP8266 Aeroponics</h1></head><body><table width=\"1000\" cellpadding=\"7\" cellspacing=\"3\"><tr>";
  //First strind in table
  webPage += "<td width=\"200\"><b>Temperature: </b> 29 &#176;</td>";
  webPage += "<td width=\"300\"><b>Watering count All: </b>20</td>";
  webPage += "<td width=\"200\"><b>Day length(hour): </b>hour</td>";
  webPage += "<td width=\"300\"><b>Freezing water state: </b>ON/OFF</td>";
  webPage += "</tr><tr>";
  //Second string in table
  webPage += "<td width=\"200\"><b>Water level: </b>30 %</td>";
  webPage += "<td width=\"200\"><b>Last watering(min): </b>minute</td>";
  webPage += "<td width=\"200\"><b>Last tank feeling: </b>hour,minute</td>";
  webPage += "<td width=\"200\"><b>Heating water state: </b>ON/OFF</td>";
  webPage += "</tr><tr>";
  //Third string in table
  webPage += "<td width=\"200\"><b>Time: </b>day,hour,minute</td>";
  webPage += "<td width=\"300\"><b>Watering count per day: </b>12</td>";
  webPage += "<td width=\"300\"><b>Water consuption per day: </b>10 &#176; </td>";
  webPage += "<td width=\"200\"><b>Last freezing/heating: </b>day,hour,minute </td>";
  webPage += "</tr><tr>";
 //Fourth string in table
  webPage += "<td width=\"200\"><b>Light: </b>ON/OFF</td>";
  webPage += "<td width=\"200\"><b>Last WaterAir: </b>hour,minute</td>";
  webPage += "<td width=\"200\"><b>WaterGND length: </b>second</td>";
  webPage += "</tr></table>";
  //Table END
  //Refresh Button
  webPage += "<p> <a href=\"refresh\"><button>REFRESH</button></a>&nbsp;</p></body></html> ";
}
/*   HTML_PageRefresh
 * 
 */
void HTML_PageRefresh()
{
  webPage = "";
  webPage += "<html><head><meta charset=\"utf-8\"><title>Aeroponics</title><style>body{background: #333;";
  webPage +="color: #fc0;}</style>";
  
  webPage += "<h1>ESP8266 Aeroponics</h1></head><body><table width=\"1000\" cellpadding=\"7\" cellspacing=\"3\"><tr>";
  
  //=============================================================  
  //First strind in table
  webPage += "<td width=\"200\"><b>Temperature: </b>";                     //TEMP
  webPage += g_Temp;
  webPage += " &#176;</td>";
  
  webPage += "<td width=\"300\"><b>Watering count All: </b>";             //Waterint count ALL
  webPage += g_WateringCountAll;
  webPage += "20</td>";
  
  webPage += "<td width=\"200\"><b>Day length(hour): </b>";                     //Day length
  webPage += g_DayLengthHour;
  webPage += "</td>";
  
  webPage += "<td width=\"300\"><b>Freezing water state: </b>";            //Freezing water state
  webPage += g_FreezingWaterState;
  webPage += "</td>";
  webPage += "</tr><tr>";

  //=============================================================  
  //Second string in table
  webPage += "<td width=\"200\"><b>Water level: </b>";                      //Water level
  webPage += g_WaterLevel;
  webPage += "%</td>";
  
  webPage += "<td width=\"200\"><b>Last watering(min): </b>";                      //Last watering
  webPage += g_LastWateringMin;
  webPage += "</td>";
  
  webPage += "<td width=\"200\"><b>Last tank feeling: </b>";                  //Last tank feeling
  webPage += g_LastTankFeelingHour;
  webPage += ":";
  webPage += g_LastTankFeelingMin;
  webPage += "</td>";
  
  webPage += "<td width=\"200\"><b>Heating water state: </b>";                 //Heating water state
  webPage += g_HeatingWaterState;
  webPage += "</td>";  
  webPage += "</tr><tr>";

  //=============================================================
  //Third string in table
  webPage += "<td width=\"200\"><b>Time: </b>";                                 //Time
  webPage += g_TimeDay;
  webPage += ":";
  webPage += g_TimeHour;
  webPage += ":";
  webPage += g_TimeMin;
  webPage += "</td>";
  webPage += "<td width=\"300\"><b>Watering count per day: </b>";               //Watering count
  //Watering count
  webPage += "</td>";
  
  webPage += "<td width=\"300\"><b>Water consuption per day: </b>";             //Water consuption per day
  webPage += g_WaterConsPerDay;
  webPage += " &#176; </td>";
  
  webPage += "<td width=\"200\"><b>Last freezing/heating: </b>";                //Last freezing/heating time
  webPage += g_LastFreezingDay;
  webPage += ":";
  webPage += g_LastFreezingHour;
  webPage += ":";
  webPage += g_LastFreezingMin;
  webPage += "</td>";
  webPage += "</tr><tr>";

  //=============================================================
  //Fourth string in table
  webPage += "<td width=\"200\"><b>Light: </b>";                                 //Light state
  webPage += g_LightState;
  webPage += "</td>";
  
  webPage += "<td width=\"200\"><b>Last WaterAir: </b>";                        //Last WaterAir Time
  webPage += g_LastWaterAirHour;
  webPage += ":";
  webPage += g_LastWaterAirMin;
  webPage += "</td>";
  
  webPage += "<td width=\"200\"><b>WaterGND length: </b>";                     //length
  webPage += g_WaterGNDLengthSec;
  webPage += "</td>";
  webPage += "</tr></table>";
 
  //Table END
  //Refresh Button
  webPage += "<p> <a href=\"refresh\"><button>REFRESH</button></a>&nbsp;</p></body></html> ";
}


/* ==============================================================================================================
 * ======================================   Check Message and CRC   =============================================
 ==============================================================================================================*/

 
/* Search data in Serial Port
 * Input message structures:
 *     "82661TempWaterLVLTimeDayTimeHourTimeMinute"
 *     8266- prefix, 1 - packed value
 */   
bool Serial_SearchInputMess()
{
  if (Serial.available() > 3)  //If we have input data
  {
      for(int i=0; i<= 18; i++)
      {
       g_aInMessData[i] = Serial.read()-48;    
       if(i == 3 || i == 8 || i == 12 || i == 16)
           delay(10);   
      }             
      return true;
  }  
  return false;
} 

/*  CRC message check
 * 
 */
bool Serial_CRCMessCheck()    
{   //8266130365605033651 example
    //Check CRC corrects. Packet Struct: "8266PrefixTempDataDayDataHourDataMinuteCRC"
    //CRC - [first byte temp] [first byte day] [first byte hour] [first byte minute] [prefix]
    if(g_aInMessData[4] == g_aInMessData[18] && g_aInMessData[5] == g_aInMessData[14] &&
       g_aInMessData[7] == g_aInMessData[15] && g_aInMessData[10] == g_aInMessData[16] &&
      g_aInMessData[12] == g_aInMessData[17])
     {
        return true;
     }  
  return false;
}

/* Check Adress message
 * 
 */
bool Serial_AdressMessCheck()
{
  //Check '8266' prefix on message
  if(g_aInMessData[0] == 8 && g_aInMessData[1] == 2 && g_aInMessData[2] == 6 && g_aInMessData[3] == 6)
  {
    return true;
  }
  return false;
} 
