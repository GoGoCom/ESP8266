 /*********
  WIFISWITCH control Program Copyright by Jons & Lee Pty Ltd in Australia 2017
*********/
/*
#define  c     3830    // 261 Hz 
#define  d     3400    // 294 Hz 
#define  e     3038    // 329 Hz 
#define  f     2864    // 349 Hz 
#define  g     2550    // 392 Hz 

#define  a     2272    // 440 Hz 
#define  b     2028    // 493 Hz 
#define  C     1912    // 523 Hz 
*/
ADC_MODE(ADC_VCC);  

#include <Adafruit_NeoPixel.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h> 
#include <EEPROM.h>

#include <math.h>

#include <TimeLib.h>       
//#include <Timezone.h>    
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 13

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

//#include "DHT.h"
#include "FS.h"

const String  VERSION = "V4.7";
  
const int relay_pin = 4;   // gpio4
const int buzzer_pin = 12; // gpio15
const int led_pin = 14;    // gpio12
const int led_pin2 = 16;    // gpio12

// Uncomment one of the lines below for whatever DHT sensor type you're using!
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

#define  RELAY_ON   LOW
#define  RELAY_OFF  HIGH

#define  LED_ON   HIGH
#define  LED_OFF  LOW

// DHT Sensor
//const int DHTPin = 13;
// Initialize DHT sensor.
//DHT dht(DHTPin, DHTTYPE);

MDNSResponder mdns;
ESP8266WebServer server(80);

#define NUM_LEDS 2
#define BRIGHTNESS 60

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, led_pin, NEO_GRB + NEO_KHZ800);


#define  HeaterNowON     strip.Color(255,255, 0)
#define  HeaterNowOFF    strip.Color(0, 0, 255)

#define  HeaterBookON    strip.Color(0, 255, 255)
#define  HeaterBookOFF   strip.Color(255, 0, 0)

#define  BookON          strip.Color(255, 0, 255)
#define  BookOFF         strip.Color(0, 255, 0)

#define  STEPAON         strip.Color(  0,255, 0)
#define  STEPBON         strip.Color(255,  0, 0)
#define  STEPOFF         strip.Color(  0,  0, 0)

char DeviceID[10] = "JNL000000";

static String strHour[25] = { "-12","-11","-10","-09","-08","-07","-06","-05","-04","-03","-02","-01","0","+01","+02","+03","+04","+05","+06","+07","+08","+09","+10","+11","+12" };
static String strMin[4] = { "00","15","30","45" };
static String month_s[13] = { "", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static char *week_s[8]   = { "ERR", "SUN", "MON", "TUE", "WED", "THR", "FRI", "SAT" };

// Temporary variables
//static char celsiusTemp[10];
//static char fahrenheitTemp[10];
//static char humidityTemp[10];

// Replace with your network credentials
char* ssid         = "coreanplace";
char* password     = "COREAN000";

String strShortMessage, strLongMessage;
String  webPageBody = "", webPageTemp = "";

String ScanList[20];
int    ScanListCount, ScanResult, ConnectResult;

const int  g_duration[8]    = { 0, 10, 15, 20, 30, 45, 60, 60 };  // diffrent with MQ version
const int  g_temperature[8] = { 0, 16, 17, 18, 19, 20, 22, 24 };  

int  c_year = 0, c_month = 0, c_day = 0, c_hour = 0, c_minute = 0, c_sec = 0;
int  gab_minute, gab_second;
int  n_hour, n_minute, n_second;  
int  t_hour, t_minute, t_second;  
int  s_led_now, s_temperature;
int  s_now, s_duration, s_eco;
int  s_now_tmp, s_duration_tmp, s_eco_tmp;  
uint8_t  g_hour_tmp[24];

float  TempC = 0.0, TempF = 0.0, TempH = 0.0, TempAC = 0.0, TempAF = 0.0;

unsigned long PixelTi, RainbowTi;
bool PixelJigJag;
uint8_t  PixelPos;

bool Authority;

static String StyleContent="td{vertical-align:middle;} \
.T0{border-color:#000000;border-style:groove;background-color:#1A1A1A;width:100%;border:0;} \
.T1{border-color:#33cccc;border-style:solid;background-color:#ffffff;border-width:1px;width:100%;border:1;} \
.B3R{width:30%;text-align:right} \
.B3L{width:30%;text-align:left} \
.B4R{width:40%;text-align:right} \
.B4L{width:40%;text-align:left} \
.S0{border-color:#36b0ed;border-style:outset;border-width:1.5px;width:20%} \
.S1{border-color:#63dcff;border-style:hidden;border-width:1px;text-align:center} \
.S2{border-color:#DD6400;border-style:outset;border-width:0.5px;} \
.S3{border-color:#cccccc;border-style:outset;border-width:1px;} \
.S4{border-color:#1b1b1b;border-style:solid;border-width:1px;width:60%} \
.S5{border-color:#cccccc;border-style:solid;border-width:1px;width:60%} \
.S6{border-color:#848484;border-style:solid;border-width:1px;width:60%;text-align:center} \
.D1{color:white;font-family:Tw Cen MT;text-align:left;text-indent:0px;padding:0px0px0px0px;margin:0px0px0px0px;border-color:#a1a1a1;border-style:hidden;border-width:2px;font-size:28px} \
.D2{text-align:left;text-indent:0px;padding:0px0px0px0px;margin:0px0px0px0px;border-color:#99cc00;border-style:solid;border-width:2px;font-size:20px} \
input.MB1{ width:200px; padding:8px; cursor:pointer; font-weight:bold; font-size:69%; font-family:Tw Cen MT; background:#1B1B1B; \
color:#D1D1D1; border:2px outset #00C6FF; border-radius:10px; -moz-box-shadow::6px 6px 5px #999;  \
-webkit-box-shadow::6px 6px 5px #999; box-shadow::6px 6px 5px #999; } \
input.MB1:hover { color:#ffffff; background:#151515; border:2px inset #fff; \
-moz-box-shadow::5px 5px 4px #929292; -webkit-box-shadow::5px 5px 4px #929292; box-shadow::5px 5px 4px #929292; }		\
input.MB2{ width:200px; padding:10px; cursor:pointer; font-weight:bold; font-size:70%; background:#3366cc; color:#fff; border:1px solid #3366cc; \
border-radius:10px; -moz-box-shadow::6px 6px 5px #999; -webkit-box-shadow::6px 6px 5px #999; box-shadow::6px 6px 5px #999; } \
input.MB2:hover { color:#ffff00; background:#000; border:1px solid #fff; -moz-box-shadow::5px 5px 4px #adadad; -webkit-box-shadow::5px 5px 4px #adadad; \
box-shadow::5px 5px 4px #adadad; } \
input.MB3{ width:200px; padding:10px; cursor:pointer; font-weight:bold; font-size:70%; background:#ffff00; color:#000000; \
border:1px solid #3366cc; border-radius:10px; -moz-box-shadow::6px 6px 5px #999; -webkit-box-shadow::6px 6px 5px #999; \
box-shadow::6px 6px 5px #999; } \
input.MB3:hover { color:#f1f1f1; background:#1a1a1a; border:1px solid #fff; -moz-box-shadow::5px 5px 4px #adadad; -webkit-box-shadow::5px 5px 4px #adadad; \
box-shadow::5px 5px 4px #adadad; }";



static String contentHead = "<!DOCTYPE html><html><head><meta charset='utf-8'> <link rel='stylesheet' href='/style.css'> </head>";

static String  contentLogin = contentHead + "<body><H2>To log in, please use : admin/admin</H2> \
  <div class='D2'> <form method='POST'action='/login'> \
  <table cellpadding='2'cellspacing='3'class='T1'> \
  <tr><td class='B4R'>User Name:</td><td align=left><input type='text' name='USERNAME' placeholder='user name'></td><td class='B3L'></td></tr> \
  <tr><td class='B4R'>Password: </td><td align=left><input type='password' name='PASSWORD' placeholder='password'></td><td class='B3L'></td></tr> \
  </table><table cellpadding='2'cellspacing='3'class='T1'> \
<tr><td  width='100%' align=center><input type='submit' class='MB1' name='SUBMIT' value='Submit'></td></tr> </table></form></div></body></html> ";

 
struct WIFI {
  char  SID[21];
  char  PWD[21];
  char  PIN[21];
  char  IPA[21];
  uint8_t   HRI;
  uint8_t   MNI;
  boolean   SFS;
  char  REV[39];
};

struct HOUR {
  uint8_t  g_hour[24];
};

struct TEMP {
  boolean  g_checked;
  uint8_t  g_start_hour;
  uint8_t  g_end_hour;
  uint8_t  g_temp_index;
};

struct MQTT {
  char  SVR[51];
  uint16_t  POT;
  char  DID[51];
  char  USR[21];
  char  PAS[21];
  char  TPC[51];
  uint8_t   QOS;
  boolean   RET;
  char  MES[51];
}; 

struct MODE {
  char     LocalName[21];
  char     HostName[21];
};

struct {
  struct WIFI W;
  struct HOUR H;
  struct TEMP T[4];
  struct MQTT M;  
  struct MODE V;  
} Info;

struct TEMP g_temp_tmp[4];

//
void Initialize()
{ 

//ESP.wdtDisable();
  ESP.wdtEnable(WDTO_8S);

  s_now      = 0;
  s_duration = 1;
  s_eco      = 0;
  n_hour     = 0;
  n_minute   = 0;
  n_second   = 0;  
  s_temperature = 2;
  s_led_now   = 0;
  strShortMessage  = "";
  strLongMessage  = "";  
  PixelPos    = 0;
  PixelJigJag = false;
  ScanListCount = 0;
  ScanResult    = 0;
  ConnectResult = -1;
  Authority = false;
  
  pinMode(relay_pin, OUTPUT);
  pinMode(buzzer_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
  pinMode(led_pin2, OUTPUT);
  
  digitalWrite(led_pin2,   LED_OFF);    // On
  digitalWrite(relay_pin, RELAY_OFF); // off

  Serial.begin(9600); //115200);

  sprintf(&DeviceID[3], "%06X", ESP.getChipId());       

  //ESP.getMacAdress
  //WiFi.macAddress();
   
  tone(buzzer_pin, 392, 1000);
  //dht.begin();
   sensors.begin();
  
  EEPROM.begin( sizeof(Info) );

  ReadAllFromEEPROM();

  Serial.println("MSG\tStyle !");

  MakeStyleFile();

  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  colorWipe(strip.Color(255, 0, 0), 50); // Red
  
  Serial.println("MSG\tSystem Initialize !");

}

void setup(void)
{   
  Initialize();    

  Scanning();     

  switch( ScanResult ) {
       case 1:
          ConnectResult = Connect_WIFI();
          if( ConnectResult == 0 ) {
             // Success            
            setTimeFromUTCToLocal( ); // Set System Time from Internet
            Connect_WebServer();            
            tone(buzzer_pin, 329, 1000);      
            break;
          }          
       case 0:       
       case -1:
          APMode();   
       default : break;
  }
 
  n_second = second(now());  
  PixelTi =  millis();
  RainbowTi  =  millis();
}
 

void loop(void)
{
  time_t  localTime;
   
  ESP.wdtFeed();
  
  server.handleClient();    

  SerialBle();

  timebow( ); // Strip LED per second

  if( ConnectResult == 0 ) {
  
    localTime = now();
    
    t_second = second(localTime);
    if( t_second < n_second  ) t_second = 60 + t_second;
    
     gab_second = t_second - n_second;
    if( gab_second >= 10 ) {

      n_second = second(localTime);

      if( ESP.getVcc() < 2750 ) {
        tone(buzzer_pin, 3038, 1500);
        digitalWrite(led_pin2, LED_ON);
        Serial.println("MSG\tLow voltage!" );
      } else {
        digitalWrite(led_pin2, LED_OFF);
      }              
      
      if( s_now == 1 ) {       // Now On
          t_minute = minute(localTime);
          if( t_minute < n_minute  ) t_minute = 60 + t_minute ;
          
          gab_minute = t_minute - n_minute;        
         if( gab_minute  >  g_duration[s_duration]   ) {
            s_now = 0;
         } else {
            HeaterOn(1, "N run!", "Turned on!");              
         }         
      }
      if( s_now == 3 ) {        // hbook function
          t_hour   = hour(localTime);
          t_minute = minute(localTime);
        
         if( Info.H.g_hour[ t_hour ] == 1 ) { 
            if ( g_duration[s_duration] > t_minute ) {
                HeaterOn(2, "HB run!", "Turned on by hbooking!");              
            } else {            
                HeaterOff(3, "HB stop!", "Turned off by hbooking!");             
            }             
          }
          else {            
            HeaterOff(3, "No hbook!", "HBooked inactive!");
          }                 
      }      

      if( s_now == 6 ) {        // tbook function

         t_hour = hour(localTime);         
         if( Info.T[0].g_checked == true ) {
            if(( t_hour >= Info.T[0].g_start_hour ) || ( t_hour < Info.T[0].g_end_hour ) ) {
                s_temperature = Info.T[0].g_temp_index;              
                HeaterByTemperature();
            }          
         }
        for(int i=1; i<4; i++ ) {
             if( Info.T[i].g_checked == true ) {    
                if(( Info.T[i].g_start_hour <= t_hour) && ( t_hour < Info.T[i].g_end_hour ) ) {
                    s_temperature = Info.T[i].g_temp_index;                  
                    HeaterByTemperature();
                }          
             }

         } 
      }      

      if( ( s_now == 0 ) || ( s_now == 2 ) ) {
          HeaterOff(0, "Ready!", "Ready");
      } 

	  }
	}		
}

void MakeStyleFile()
{
   File f;
   uint16_t  f_size;
    
  if( !SPIFFS.begin())
  {
    Serial.println("MSG\tSPIFFS mount failed!");
    delay(2000);
    return;
  } 
       
  if( Info.W.SFS != true ) {  
    // Next lines have to be done ONLY ONCE!!!!!When SPIFFS is formatted ONCE you can comment these lines out!!
    Serial.println("MSG\tSPIFFS to be formatted!");
    SPIFFS.format();
    Info.W.SFS = true;
    WriteToEEPROM(long(&Info.W.SFS), sizeof(Info.W.SFS) ) ;    
 }  

  
  f = SPIFFS.open("/style.css", "r");
  if (!f) {
      Serial.println("MSG\tfile reading failed");
      f_size = 0;
  } else {
    f_size = f.size();
    f.close();
  }
  
  if( (f_size - StyleContent.length()) != 2 ) {
      // open file for writing
      f = SPIFFS.open("/style.css", "w");
      if (!f) {
          Serial.println("MSG\tfile writing failed!");
          delay(2000);
          return;
      }
      Serial.println("MSG\tWriting style file");  
      delay(1000);
      f.println(StyleContent);
      f.close();
  }
  
  // Preparing common server factors
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent"}; //,"Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize );  
  // set url -> file system
  server.serveStatic("/style.css",  SPIFFS, "/style.css");
  
  server.onNotFound(handleNotFound);

}

void timebow( ) {
        
  if(  millis() - RainbowTi > 50  ) {  
    if ( ( ConnectResult == 0 ) && (  s_led_now  == 0 ) ) { // when success, waiting !
      strip.setPixelColor(0, Wheel(PixelPos & 255));
      strip.setPixelColor(1, Wheel(PixelPos & 255));
      strip.show();               
      PixelPos ++;
    }
    RainbowTi =  millis();    
  } 
    
  if(  millis() - PixelTi > 1000  ) {    
    if( ConnectResult == -1 ) {
      if( PixelJigJag ) {
        if( ScanResult == 1 ) {
           strip.setPixelColor(0, STEPAON);
           strip.setPixelColor(1, STEPOFF);
        } else {
           strip.setPixelColor(0, STEPBON);
           strip.setPixelColor(1, STEPOFF);
        }
        PixelJigJag = false;    
      }
      else {
        if( ScanResult == 1 ) {
          strip.setPixelColor(0, STEPOFF);
          strip.setPixelColor(1, STEPAON);
        } else {     
          strip.setPixelColor(0, STEPOFF);
          strip.setPixelColor(1, STEPBON);
        }
        PixelJigJag = true;        
      }
      strip.show();
    }    
    
    if( ConnectResult == 0 ) {
     if(  s_led_now  != 0 ) {      
      if( PixelJigJag ) {  
        if(  s_led_now  == 1) {
          strip.setPixelColor(0, HeaterNowON);
          strip.setPixelColor(1, HeaterNowOFF);
        }
        if(  s_led_now  == 2) {
          strip.setPixelColor(0, HeaterBookON);
          strip.setPixelColor(1, HeaterBookOFF);
        }
        if(  s_led_now  == 3) {
          strip.setPixelColor(0, BookON);
          strip.setPixelColor(1, BookOFF);
        }  
        PixelJigJag = false;  
      }
      else {
        if(  s_led_now  == 1) {
            strip.setPixelColor(0, HeaterNowOFF);
            strip.setPixelColor(1, HeaterNowON);
        }
        if(  s_led_now  == 2) {
            strip.setPixelColor(0, HeaterBookOFF);
            strip.setPixelColor(1, HeaterBookON);
        }
        if(  s_led_now  == 3) {
            strip.setPixelColor(0, BookOFF);
            strip.setPixelColor(1, BookON);
        }    
        PixelJigJag = true;        
      }
      strip.show();
     }
    }    
    PixelTi =  millis();    
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void MakeWebpageBodyHead( )
{
 time_t t;
 String strDateTime, strTemperature, strFeelTemperature, strHumidity;

  getTemperature();
  
  strTemperature     =  String(TempC)  + "°C , " + String(TempF)  + "°F";
  strFeelTemperature =  String(TempAC) + "°C , " + String(TempAF) + "°F";
  strHumidity        =  String(TempH)  + " %";
  
  t = now();
  
  strDateTime = String(hour(t)) + ":" + String(minute(t)) + ":" + String(second(t)) + " " + dayShortStr(weekday(t)) + " " + String(day(t)) + "/" + monthShortStr(month(t)) + "/" + String(year(t)) + "&nbsp;&nbsp;"+ "UTC" + strHour[Info.W.HRI] + "h" + strMin[Info.W.MNI] + "m" ;

  webPageBody = contentHead + "<body style='background-color:#1A1A1A;'><div><header><div><h1 style='color:white;font-family:Tw Cen MT'>Welcome to Wifi-Switch GUI!("+ VERSION + ")</h1></header></div>";
  webPageBody += "<div class='D1'>";
  webPageBody += "<table cellpadding='2'cellspacing='2'class='T0'><tr>";
  webPageBody += "<td class='S1'>" + strDateTime + "</td>";  
  webPageBody += "</tr></table>";    
  webPageBody += "<table cellpadding='2'cellspacing='2'class='T0'><tr>";
  webPageBody += "<td class='S1'>Current Temperature <br />" + strTemperature + "</td>";   
  webPageBody += "<td class='S1'>Feel Like Temperature <br />" + strFeelTemperature + "</td>";  
  webPageBody += "<td class='S1'>Humidity <br />"  + strHumidity + "</td>";  
  webPageBody += "</tr></table>";  
}

void MakeWebpageBody1( )
{
 
  MakeWebpageBodyHead( );

  webPageBody += "<form method='POST'action='exec'>";


  webPageBody += "<table cellpadding='2'cellspacing='3'class='T0'>";
  webPageBody += "<tr>";  
  webPageBody += "<td class='S0'>Time-mode(on for)</td>";
  
  webPageTemp = "<td class='S3'><input type='radio'name='DR'value='";
  
  if(s_duration == 0 ) {
	  webPageBody += webPageTemp + "1'>10</td>";
	  webPageBody += webPageTemp + "2'>15</td>";
	  webPageBody += webPageTemp + "3'>20</td>";
	  webPageBody += webPageTemp + "4'>30</td>";
	  webPageBody += webPageTemp + "5'>45</td>";
	  webPageBody += webPageTemp + "6'>60</td>";
	}
  if(s_duration == 1 ) {
	  webPageBody += webPageTemp + "1'checked>10</td>";
	  webPageBody += webPageTemp + "2'>15</td>";
	  webPageBody += webPageTemp + "3'>20</td>";
    webPageBody += webPageTemp + "4'>30</td>";
    webPageBody += webPageTemp + "5'>45</td>";
    webPageBody += webPageTemp + "6'>60</td>";
	}
  if(s_duration == 2 ) {
	  webPageBody += webPageTemp + "1'>10</td>";
	  webPageBody += webPageTemp + "2'checked>15</td>";
	  webPageBody += webPageTemp + "3'>20</td>";
    webPageBody += webPageTemp + "4'>30</td>";
    webPageBody += webPageTemp + "5'>45</td>";
    webPageBody += webPageTemp + "6'>60</td>";
	}
  if(s_duration == 3 ) {
	  webPageBody += webPageTemp + "1'>10</td>";
	  webPageBody += webPageTemp + "2'>15</td>";
	  webPageBody += webPageTemp + "3'checked>20</td>";
    webPageBody += webPageTemp + "4'>30</td>";
    webPageBody += webPageTemp + "5'>45</td>";
    webPageBody += webPageTemp + "6'>60</td>";
	}
  if(s_duration == 4 ) {
	  webPageBody += webPageTemp + "1'>10</td>";
	  webPageBody += webPageTemp + "2'>15</td>";
	  webPageBody += webPageTemp + "3'>20</td>";
	  webPageBody += webPageTemp + "4'checked>30</td>";
	  webPageBody += webPageTemp + "5'>45</td>";
	  webPageBody += webPageTemp + "6'>60</td>";
	}
  if(s_duration == 5 ) {
	  webPageBody += webPageTemp + "1'>10</td>";
	  webPageBody += webPageTemp + "2'>15</td>";
	  webPageBody += webPageTemp + "3'>20</td>";
	  webPageBody += webPageTemp + "4'>30</td>";
	  webPageBody += webPageTemp + "5'checked>45</td>";
	  webPageBody += webPageTemp + "6'>60</td>";
	}
  if(s_duration == 6 ) {
	  webPageBody += webPageTemp + "1'>10</td>";
	  webPageBody += webPageTemp + "2'>15</td>";
	  webPageBody += webPageTemp + "3'>20</td>";
	  webPageBody += webPageTemp + "4'>30</td>";
	  webPageBody += webPageTemp + "5'>45</td>";
	  webPageBody += webPageTemp + "6'checked>60</td>";
	}  
  webPageBody += "</tr>";
  webPageBody += "</table>";
  
  webPageBody += "<table cellpadding='2'cellspacing='2'class='T0'>";  
  webPageBody += "<tr>";
  webPageBody += "<td class='S0'>Current status</td><td class='S5'>";
  webPageBody += strLongMessage ;  
  webPageBody += "</td><td><input class='MB1'type='button'value='Status'onclick=\"window.location.href=\'/\'\"></td></tr></table>";  


  webPageBody += "<table cellpadding='2'cellspacing='3'class='T0'>";
  webPageBody += "<tr>";  
	webPageBody += "<td class='S0'>Now mode</td><td class='S5'><input type='radio'name='NW'value='1'";
  if( s_now == 0 || s_now > 2 ) 
	  webPageBody += ">On&nbsp;&nbsp;&nbsp;<input type='radio'name='NW'value='2'>Off</td><td><input class='MB1'type='submit'name='SMT'value='Submit'></td>";  
  if( s_now == 1 ) 
	  webPageBody += "checked>On&nbsp;&nbsp;&nbsp;<input type='radio'name='NW'value='2'>Off</td><td><input class='MB1'type='submit'name='SMT'value='Submit'></td>";	  
  if( s_now == 2 ) 
	  webPageBody += ">On&nbsp;&nbsp;&nbsp;<input type='radio'name='NW'value='2' checked>Off</td><td><input class='MB1'type='submit'name='SMT'value='Submit'></td>";  
  
  webPageBody += "</tr>";
  webPageBody += "</table>";
  
  webPageBody += "<table cellpadding='2'cellspacing='3'class='T0' >";
  webPageBody += "<tr>";
  
  if( s_eco == 0 ) 
    webPageBody += "<td class='S0'><input type='checkbox'name='EC'value='1'>Eco Mode</td>";
  else
    webPageBody += "<td class='S0'><input type='checkbox'name='EC'value='1'checked>Eco Mode</td>";

   webPageBody += "<td class='S6'><input type='radio'name='NW'value='3'";
  
  if( s_now < 3 || s_now == 6 || s_now == 7 || s_now == 8) 
    webPageBody += ">Book&nbsp;&nbsp;&nbsp;<input type='radio'name='NW'value='4'>All Set&nbsp;&nbsp;&nbsp;<input type='radio'name='NW'value='5'>All Reset</td><td><input class='MB1'type='submit'name='SMT'value='Time Table'></td>";   
  if( s_now == 3 ) 
    webPageBody += "checked>Book&nbsp;&nbsp;&nbsp;<input type='radio'name='NW'value='4'>All Set&nbsp;&nbsp;&nbsp;<input type='radio'name='NW'value='5'>All Reset</td><td><input class='MB1'type='submit'name='SMT'value='Time Table'></td>";  
  if( s_now == 4 ) 
    webPageBody += ">Book&nbsp;&nbsp;&nbsp;<input type='radio'name='NW'value='4' checked>All Set&nbsp;&nbsp;&nbsp;<input type='radio'name='NW'value='5'>All Reset</td><td><input class='MB1'type='submit'name='SMT'value='Time Table'></td>";  
  if( s_now == 5 ) 
    webPageBody += ">Book&nbsp;&nbsp;&nbsp;<input type='radio'name='NW'value='4'>All Set&nbsp;&nbsp;&nbsp;<input type='radio'name='NW'value='5' checked>All Reset</td><td><input class='MB1'type='submit'name='SMT'value='Time Table'></td>";  

  webPageBody += "</tr>";
  webPageBody += "</table>";
  
  webPageBody += "<table cellpadding='2'cellspacing='3'class='T0' >";
  webPageBody += "<tr>";

  webPageBody += "<td class='S0'>Temperature mode</td>";

  webPageBody += "<td class='S6'><input type='radio'name='NW'value='6'";  
  
  if( s_now < 3 || s_now == 3 || s_now == 4 || s_now == 5) 
    webPageBody += ">Book&nbsp;&nbsp;&nbsp;<input type='radio'name='NW'value='7'>All Set&nbsp;&nbsp;&nbsp;<input type='radio'name='NW'value='8'>All Reset</td><td><input class='MB1'type='submit'name='SMT'value='Temp Table'></td>";   
  if( s_now == 6 ) 
    webPageBody += "checked>Book&nbsp;&nbsp;&nbsp;<input type='radio'name='NW'value='7'>All Set&nbsp;&nbsp;&nbsp;<input type='radio'name='NW'value='8'>All Reset</td><td><input class='MB1'type='submit'name='SMT'value='Temp Table'></td>";  
  if( s_now == 7 ) 
    webPageBody += ">Book&nbsp;&nbsp;&nbsp;<input type='radio'name='NW'value='7' checked>All Set&nbsp;&nbsp;&nbsp;<input type='radio'name='NW'value='8'>All Reset</td><td><input class='MB1'type='submit'name='SMT'value='Temp Table'></td>";  
  if( s_now == 8 ) 
    webPageBody += ">Book&nbsp;&nbsp;&nbsp;<input type='radio'name='NW'value='7'>All Set&nbsp;&nbsp;&nbsp;<input type='radio'name='NW'value='8' checked>All Reset</td><td><input class='MB1'type='submit'name='SMT'value='Temp Table'></td>";  
    
   webPageBody += "</tr></table></form></div></body></html>"; 
   
}


void MakeWebpageBody2( )
{
  int i;
 
  MakeWebpageBodyHead( );

  webPageBody += "<form method='POST'action='exec'>";     
  webPageBody += "<table cellpadding='2' cellspacing='4' class='T0'><tr>";
  
  for(i=0; i<6; i++)
  {
	  if( g_hour_tmp[i] == 0 ) 
	  	webPageBody += "<td class='S3'><input type='checkbox'name='H0"+ String(i) +"'value='1'>0"+ String(i) +"h</td>";
	  else 
	  	webPageBody += "<td class='S3'><input type='checkbox'name='H0"+ String(i) +"'value='1'checked>0"+ String(i) +"h</td>";
  }
  
  webPageBody += "</tr><tr>";

  for(i=6; i<10; i++)
  {
	  if( g_hour_tmp[i] == 0 ) 
	  	webPageBody += "<td class='S3'><input type='checkbox'name='H0"+ String(i) +"'value='1'>0"+ String(i) +"h</td>";
	  else 
	  	webPageBody += "<td class='S3'><input type='checkbox'name='H0"+ String(i) +"'value='1'checked>0"+ String(i) +"h</td>";
  }
  
  for(i=10; i<12; i++)
  {
		 if( g_hour_tmp[i] == 0 ) 
		  webPageBody += "<td class='S3'><input type='checkbox'name='H"+ String(i) +"'value='1'>"+ String(i) +"h</td>";
	  else 
		  webPageBody += "<td class='S3'><input type='checkbox'name='H"+ String(i) +"'value='1'checked>"+ String(i) +"h</td>";
 	}
 	 
  webPageBody += "</tr><tr>";

  for(i=12; i<18; i++)
  {
		 if( g_hour_tmp[i] == 0 ) 
		  webPageBody += "<td class='S3'><input type='checkbox'name='H"+ String(i) +"'value='1'>"+ String(i) +"h</td>";
	  else 
		  webPageBody += "<td class='S3'><input type='checkbox'name='H"+ String(i) +"'value='1'checked>"+ String(i) +"h</td>";
 	}
 	
  webPageBody += "</tr><tr>";

  for(i=18; i<24; i++)
  {
		 if( g_hour_tmp[i] == 0 ) 
		  webPageBody += "<td class='S3'><input type='checkbox'name='H"+ String(i) +"'value='1'>"+ String(i) +"h</td>";
	  else 
		  webPageBody += "<td class='S3'><input type='checkbox'name='H"+ String(i) +"'value='1'checked>"+ String(i) +"h</td>";
 	}

  webPageBody += "<input type='hidden'name='EC'value='"+String(s_eco_tmp)+"'>";
  webPageBody += "<input type='hidden'name='NW'value='"+String(s_now_tmp)+"'>";
  webPageBody += "<input type='hidden'name='DR'value='"+String(s_duration_tmp)+"'>";
    
//    webPageBody += "</tr></table></div><br><input class='MB2'style='font-size:150%'type='submit'value='Submit'><input class='MB3'style='font-size:150%'Type='button'VALUE='Status' onclick=\"window.location.href=\'/\'\"></form></body></html>";
  webPageBody += "</tr></table></div><br><input class='MB1'style='font-size:120%'type='submit'name='SMT'value='HBook'><input class='MB1'style='font-size:120%'Type='button'VALUE='Back' onClick='history.go(-1);return true;'></form></body></html>";
 
}

void MakeWebpageBody3( )
{
  int i;
 
  MakeWebpageBodyHead( );

  webPageBody += "<form method='POST'action='exec'>";     
  webPageBody += "<table cellpadding='2' cellspacing='4' class='T0'><tr>";

  for(i=0; i<4; i++)
  {
      if( g_temp_tmp[i].g_checked == false ) 
        webPageBody += "<td class='S0'><input type='checkbox'name='T0"+ String(i) +"'value='1'>"+ String(g_temp_tmp[i].g_start_hour) +"h~"+ String(g_temp_tmp[i].g_end_hour) +"h</td>";
      else 
        webPageBody += "<td class='S0'><input type='checkbox'name='T0"+ String(i) +"'value='1'checked>"+ String(g_temp_tmp[i].g_start_hour) +"h~"+ String(g_temp_tmp[i].g_end_hour) +"h</td>";

      webPageTemp = "<td class='S2'><input type='radio'name='T1"+ String(i) +"'value='"; 
   
                  
     if(g_temp_tmp[i].g_temp_index == 0 ) {
        webPageBody += webPageTemp + "1'>"+String(g_temperature[1])+"°C</td>";
        webPageBody += webPageTemp + "2'>"+String(g_temperature[2])+"°C</td>";
        webPageBody += webPageTemp + "3'>"+String(g_temperature[3])+"°C</td>";
        webPageBody += webPageTemp + "4'>"+String(g_temperature[4])+"°C</td>";
        webPageBody += webPageTemp + "5'>"+String(g_temperature[5])+"°C</td>";
        webPageBody += webPageTemp + "6'>"+String(g_temperature[6])+"°C</td>";
        webPageBody += webPageTemp + "7'>"+String(g_temperature[7])+"°C</td>";
      }
     if(g_temp_tmp[i].g_temp_index == 1 ) {
        webPageBody += webPageTemp + "1'checked>"+String(g_temperature[1])+"°C</td>";
        webPageBody += webPageTemp + "2'>"+String(g_temperature[2])+"°C</td>";
        webPageBody += webPageTemp + "3'>"+String(g_temperature[3])+"°C</td>";
        webPageBody += webPageTemp + "4'>"+String(g_temperature[4])+"°C</td>";
        webPageBody += webPageTemp + "5'>"+String(g_temperature[5])+"°C</td>";
        webPageBody += webPageTemp + "6'>"+String(g_temperature[6])+"°C</td>";
        webPageBody += webPageTemp + "7'>"+String(g_temperature[7])+"°C</td>";
      }
     if(g_temp_tmp[i].g_temp_index == 2 ) {
        webPageBody += webPageTemp + "1'>"+String(g_temperature[1])+"°C</td>";
        webPageBody += webPageTemp + "2'checked>"+String(g_temperature[2])+"°C</td>";
        webPageBody += webPageTemp + "3'>"+String(g_temperature[3])+"°C</td>";
        webPageBody += webPageTemp + "4'>"+String(g_temperature[4])+"°C</td>";
        webPageBody += webPageTemp + "5'>"+String(g_temperature[5])+"°C</td>";
        webPageBody += webPageTemp + "6'>"+String(g_temperature[6])+"°C</td>";
        webPageBody += webPageTemp + "7'>"+String(g_temperature[7])+"°C</td>";
      }
     if(g_temp_tmp[i].g_temp_index == 3 ) {
        webPageBody += webPageTemp + "1'>"+String(g_temperature[1])+"°C</td>";
        webPageBody += webPageTemp + "2'>"+String(g_temperature[2])+"°C</td>";
        webPageBody += webPageTemp + "3'checked>"+String(g_temperature[3])+"°C</td>";
        webPageBody += webPageTemp + "4'>"+String(g_temperature[4])+"°C</td>";
        webPageBody += webPageTemp + "5'>"+String(g_temperature[5])+"°C</td>";
        webPageBody += webPageTemp + "6'>"+String(g_temperature[6])+"°C</td>";
        webPageBody += webPageTemp + "7'>"+String(g_temperature[7])+"°C</td>";
      }
     if(g_temp_tmp[i].g_temp_index == 4 ) {
        webPageBody += webPageTemp + "1'>"+String(g_temperature[1])+"°C</td>";
        webPageBody += webPageTemp + "2'>"+String(g_temperature[2])+"°C</td>";
        webPageBody += webPageTemp + "3'>"+String(g_temperature[3])+"°C</td>";
        webPageBody += webPageTemp + "4'checked>"+String(g_temperature[4])+"°C</td>";
        webPageBody += webPageTemp + "5'>"+String(g_temperature[5])+"°C</td>";
        webPageBody += webPageTemp + "6'>"+String(g_temperature[6])+"°C</td>";
        webPageBody += webPageTemp + "7'>"+String(g_temperature[7])+"°C</td>";
      }
     if(g_temp_tmp[i].g_temp_index == 5 ) {
        webPageBody += webPageTemp + "1'>"+String(g_temperature[1])+"°C</td>";
        webPageBody += webPageTemp + "2'>"+String(g_temperature[2])+"°C</td>";
        webPageBody += webPageTemp + "3'>"+String(g_temperature[3])+"°C</td>";
        webPageBody += webPageTemp + "4'>"+String(g_temperature[4])+"°C</td>";
        webPageBody += webPageTemp + "5'checked>"+String(g_temperature[5])+"°C</td>";
        webPageBody += webPageTemp + "6'>"+String(g_temperature[6])+"°C</td>";
        webPageBody += webPageTemp + "7'>"+String(g_temperature[7])+"°C</td>";
      }
     if(g_temp_tmp[i].g_temp_index == 6 ) {
        webPageBody += webPageTemp + "1'>"+String(g_temperature[1])+"°C</td>";
        webPageBody += webPageTemp + "2'>"+String(g_temperature[2])+"°C</td>";
        webPageBody += webPageTemp + "3'>"+String(g_temperature[3])+"°C</td>";
        webPageBody += webPageTemp + "4'>"+String(g_temperature[4])+"°C</td>";
        webPageBody += webPageTemp + "5'>"+String(g_temperature[5])+"°C</td>";
        webPageBody += webPageTemp + "6'checked>"+String(g_temperature[6])+"°C</td>";
        webPageBody += webPageTemp + "7'>"+String(g_temperature[7])+"°C</td>";
      }
     if(g_temp_tmp[i].g_temp_index == 7 ) {
        webPageBody += webPageTemp + "1'>"+String(g_temperature[1])+"°C</td>";
        webPageBody += webPageTemp + "2'>"+String(g_temperature[2])+"°C</td>";
        webPageBody += webPageTemp + "3'>"+String(g_temperature[3])+"°C</td>";
        webPageBody += webPageTemp + "4'>"+String(g_temperature[4])+"°C</td>";
        webPageBody += webPageTemp + "5'>"+String(g_temperature[5])+"°C</td>";
        webPageBody += webPageTemp + "6'>"+String(g_temperature[6])+"°C</td>";
        webPageBody += webPageTemp + "7'checked>"+String(g_temperature[7])+"°C</td>";
      }
      webPageBody += "</tr><tr>";
  }
    
  webPageBody += "<input type='hidden'name='EC'value='"+String(s_eco_tmp)+"'>";
  webPageBody += "<input type='hidden'name='NW'value='"+String(s_now_tmp)+"'>";
  webPageBody += "<input type='hidden'name='DR'value='"+String(s_duration_tmp)+"'>";
    
//    webPageBody += "</tr></table></div><br><input class='MB2'style='font-size:150%'type='submit'value='Submit'><input class='MB3'style='font-size:150%'Type='button'VALUE='Status' onclick=\"window.location.href=\'/\'\"></form></body></html>";
  webPageBody += "</tr></table></div><br><input class='MB1'style='font-size:120%'type='submit'name='SMT'value='TBook'><input class='MB1'style='font-size:120%'Type='button'VALUE='Back' onClick='history.go(-1);return true;'></form></body></html>";
 
}


void HeaterOn(int Status, String shortMsg, String longMsg)
{
     s_led_now  = Status;
     digitalWrite(relay_pin, RELAY_ON);
     
     strShortMessage = " ON:" + shortMsg;
     strLongMessage = longMsg;
     Serial.println("MSG\t"+strShortMessage );
}

void HeaterOff(int Status, String shortMsg, String longMsg)
{
     s_led_now  = Status;
     digitalWrite(relay_pin, RELAY_OFF);
     
     strShortMessage = "OFF:" + shortMsg;
     strLongMessage = longMsg;
     Serial.println("MSG\t"+strShortMessage );
}

void HeaterByTemperature(void)
{
      getTemperature();
      
      if( s_eco == 0)  {
         if(  TempC < (float) g_temperature[ s_temperature ] ) {
              HeaterOn(s_led_now, "TB run!", "Turned on by tbooking!");              
          } else {
              HeaterOff(s_led_now, "Over temp!", "Turned off by over temperature!");
          }
       }
       if( s_eco == 1)  {
         if(  TempAC < (float) g_temperature[s_temperature] ) {
              HeaterOn(s_led_now, "TBE run!", "Turned on by eco tbooking!");              
         } else {
              HeaterOff(s_led_now, "Over eco temp!", "Turned off by over temperature!");
          }
      }               
}

void getTemperature(void)
{  


  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();
  Serial.println("DONE");
  TempC = sensors.getTempCByIndex(0);

 // Check if reading was successful
  if(TempC != DEVICE_DISCONNECTED_C) 
  {
    Serial.print("Temperature : ");
    Serial.println(TempC);
  } 
  else
  {
    TempC = -0.0;
    Serial.println("Error: Could not read temperature data");
  }

  TempF  = DallasTemperature::toFahrenheit(TempC);
  TempH  = 0.0;
  TempAC = 0.0;
  TempAF = 0.0;
  
/*  
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);
    
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      TempC  = -0.0;
      TempF  = -0.0;
      TempH  = -0.0;
      TempAC = -0.0;
      TempAF = -0.0;
    }
    else{
      // Computes temperature values in Celsius + Fahrenheit and Humidity
      //float hic = dht.computeHeatIndex(t, h, false);       
      //dtostrf(hic, 6, 0, celsiusTemp);             
      //float hif = dht.computeHeatIndex(f, h);
      //dtostrf(hif, 6, 0, fahrenheitTemp);         
      //dtostrf(h, 6, 0, humidityTemp);
      //TempC  = hic;
      //TempF  = hif;
      
      TempC = t;
      TempF = f;
      TempH = h;

      double e = (double) exp( (17.27*TempC)/(237.7+TempC) ) * 6.105 * (TempH/100);

      TempAC = TempC + 0.348 * e - 4.25; // - 0.7*V );      // Apparent Temperature 
      TempAF = dht.convertCtoF(TempAC);

    }
*/
    
}

void setTimeFromUTCToLocal(void )
{
  String  tmp_s, strDateTime;
  int     i;
  time_t ltc;
     
    strDateTime = getTimeFromInternet( );
    
    tmp_s = strDateTime.substring(8 , 11);
    for(i=1; i<13 ; i++)
    {
      if ( month_s[i].equals(tmp_s) ) c_month = i;
    }
   
    c_day = strDateTime.substring(5 , 7).toInt();
    c_year = strDateTime.substring(12,16).toInt();
    c_hour = strDateTime.substring(17,19).toInt();
    c_minute = strDateTime.substring(20,22).toInt();        
    c_sec = strDateTime.substring(23,25).toInt();    

    setTime(c_hour, c_minute, c_sec, c_day, c_month, c_year); // Set Time to UTC

    if( strHour[Info.W.HRI].toInt() < 0 )
       ltc = (strHour[Info.W.HRI].toInt() * 60 - strMin[Info.W.MNI].toInt())*60;
    else       
       ltc = (strHour[Info.W.HRI].toInt() * 60 + strMin[Info.W.MNI].toInt())*60;
       
    ltc    = now() + ltc;
    setTime(ltc); // Set Local Time
    tmp_s  = strHour[Info.W.HRI] + "h" + strMin[Info.W.MNI] + "m";
    printTime(ltc, const_cast<char*> ( tmp_s.c_str()) );     
}

String getTimeFromInternet(void )
{
 String tmp_s;
  
  WiFiClient client;
  while (!!!client.connect("google.com", 80)) {
    Serial.println("MSG\tconnection failed, retrying...");
    delay(2000);
  }

  client.print("HEAD / HTTP/1.1\r\n\r\n");
 
  while(!!!client.available()) {
     yield();
  }

  while(client.available()){
    if (client.read() == '\n') {    
      if (client.read() == 'D') {    
        if (client.read() == 'a') {    
          if (client.read() == 't') {    
            if (client.read() == 'e') {    
              if (client.read() == ':') {    
                client.read();
                tmp_s = client.readStringUntil('\r');
                client.stop();
                return tmp_s;
              }
            }
          }
        }
      }
    }
  }
}

//Function to return the compile date and time as a time_t value
time_t compileTime(void)
{
#define FUDGE 25        //fudge factor to allow for compile time (seconds, YMMV)

    char *compDate = __DATE__, *compTime = __TIME__, *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
    char chMon[3], *m;
    int d, y;
    tmElements_t tm;
    time_t t;

    strncpy(chMon, compDate, 3);
    chMon[3] = '\0';
    m = strstr(months, chMon);
    tm.Month = ((m - months) / 3 + 1);

    tm.Day = atoi(compDate + 4);
    tm.Year = atoi(compDate + 7) - 1970;
    tm.Hour = atoi(compTime);
    tm.Minute = atoi(compTime + 3);
    tm.Second = atoi(compTime + 6);
    t = makeTime(tm);
    return t + FUDGE;        //add fudge factor to allow for compile time
}

//Function to print time with time zone
void printTime(time_t t, char *tf)
{
    char  buf[200];

    sprintf(buf, "MSG\t%02d:%02d:%02d %s %2d-%s-%4d UTC%s", hour(t), minute(t), second(t), week_s[weekday(t)], day(t), monthShortStr(month(t)), year(t), tf );
    Serial.println(buf);    
    delay(2000);
}

void ReadAllFromEEPROM()
{
 int i;
byte value;

  // read the value to the appropriate byte of the EEPROM.
  for( i=0; i<sizeof(Info); i ++) {
    value = EEPROM.read(i);
    *(Info.W.SID + i) =  value;
    //delay(100);
  }
  
}

void WriteAllToEEPROM()
{
   int  i;
  byte value;

  // write the value to the appropriate byte of the EEPROM.
  for( i=0; i<sizeof(Info); i ++) {
    value = *(Info.W.SID + i);
    EEPROM.write(i, value);
    delay(100);
  }  

  EEPROM.commit();
}

void WriteToEEPROM(long taddr, int len)
{
  int addr, i;
  byte value;

  addr = taddr - long( &Info );  
  for( i=0; i<len; i ++) {
    value = *(Info.W.SID + addr + i);    
    EEPROM.write(addr + i, value);
    delay(100);
  }

  EEPROM.commit();
}


void SerialBle()
{

String oPIN, tCMD, tSID, tPWD, tPIN, tIPA, tHRI, tMNI, tLNM, tHNM;
String mSVR, mPOT, mUSR, mPAS, mTPC, mQOS, mRET, mMES, mTmp;
char   cPIN[10];
int    len;


   if (Serial.available() > 0 ) { // Serial()
    char inChar = (char) Serial.read();
    if( inChar == '@' ) {
     tCMD = Serial.readStringUntil('\t');


     if( tCMD.compareTo("SET") == 0 ) {

          oPIN = Serial.readStringUntil('\t');
          mSVR = Serial.readStringUntil('\t');
          mPOT = Serial.readStringUntil('\t');
          mUSR = Serial.readStringUntil('\t');
          mPAS = Serial.readStringUntil('\t');
          mTPC = Serial.readStringUntil('\t');
          mQOS = Serial.readStringUntil('\t');
          mRET = Serial.readStringUntil('\t');
          mMES = Serial.readStringUntil('\n');


                memset(Info.M.SVR, 0, sizeof(Info.M) );

                mSVR.trim();
                len = mSVR.length();
                if( len > 50 ) len = 50;
                mSVR.toCharArray( Info.M.SVR, len + 1);

                Info.M.POT = mPOT.toInt();
                Info.M.QOS = mQOS.toInt();
                Info.M.RET = mRET.toInt();

                mUSR.trim();
                len = mUSR.length();
                if( len > 20 ) len = 20;               
                mUSR.toCharArray( Info.M.USR, len + 1);

                mPAS.trim();
                len = mPAS.length();
                if( len > 20 ) len = 20;
                mPAS.toCharArray( Info.M.PAS, len + 1);

                mTPC.trim();
                len = mTPC.length();
                if( len > 50 ) len = 50;
                mTPC.toCharArray( Info.M.TPC, len + 1);

                mMES.trim();
                len = mMES.length();
                if( len > 50 ) len = 50;
                mMES.toCharArray( Info.M.MES, len + 1);

                WriteToEEPROM(long(Info.M.SVR), sizeof(Info.M));

                Serial.println("MSG\tSystem Rebooting!");

                delay(2000);
                
                ESP.reset();

                        
        }

       if( tCMD.compareTo("LET") == 0 ) 
        {
            oPIN = Serial.readStringUntil('\n');

              char buf[200];

              sprintf(buf, "ENQ\t%s\t%d\t%s\t%s\t%s\t%d\t%d\t%s", Info.M.SVR, Info.M.POT, Info.M.USR, Info.M.PAS, Info.M.TPC, Info.M.QOS, Info.M.RET, Info.M.MES );
              Serial.println(buf);
              delay(2000);
          
        } 
  
     
     if( tCMD.compareTo("PUT") == 0 ) {
            oPIN = Serial.readStringUntil('\t');
            tSID = Serial.readStringUntil('\t');
            tPWD = Serial.readStringUntil('\t');
            tPIN = Serial.readStringUntil('\t');
            tHRI = Serial.readStringUntil('\t');
            tMNI = Serial.readStringUntil('\t');
            tLNM = Serial.readStringUntil('\t');
            tHNM = Serial.readStringUntil('\n');
            tIPA = "";

            memset(cPIN, 0, sizeof(cPIN) );

            oPIN.trim();
            len = oPIN.length();
            if( len > 4 ) len = 4;            
            oPIN.toCharArray( cPIN, len + 1);
             
            if( strcmp(cPIN, Info.W.PIN) == 0 ) {

                memset(Info.W.SID, 0, sizeof(Info.W) );

                tSID.trim();
                len = tSID.length();
                if( len > 20 ) len = 20;
                tSID.toCharArray( Info.W.SID, len + 1);

                tPWD.trim();
                len = tPWD.length();
                if( len > 20 ) len = 20;               
                tPWD.toCharArray( Info.W.PWD, len + 1);

                tPIN.trim();
                len = tPIN.length();
                if( len > 20 ) len = 20;
                tPIN.toCharArray( Info.W.PIN, len + 1);

                Info.W.HRI = tHRI.toInt();
                Info.W.MNI = tMNI.toInt();

                WriteToEEPROM(long(Info.W.SID), sizeof(Info.W));

                memset(Info.V.LocalName, 0, sizeof(Info.V) );

                tLNM.trim();
                len = tLNM.length();
                if( len > 20 ) len = 20;
                tLNM.toCharArray( Info.V.LocalName, len + 1);

                tHNM.trim();
                len = tHNM.length();
                if( len > 20 ) len = 20;
                tHNM.toCharArray( Info.V.HostName, len + 1);

                WriteToEEPROM(long(Info.V.LocalName), sizeof(Info.V));

                Serial.println("MSG\tSystem Rebooting!");

                delay(2000);
                
                ESP.reset();
                        
            } else {            
                Serial.println("MSG\tWrong PIN!");
                delay(2000);
            }
        }
        
       if( tCMD.compareTo("GET") == 0 ) 
        {
            oPIN = Serial.readStringUntil('\n');
            oPIN.trim();
            memset(cPIN, 0, sizeof(cPIN) );
            len = oPIN.length();
            if( len > 4 ) len = 4;            
            oPIN.toCharArray( cPIN, len + 1);

            if( strcmp(cPIN, Info.W.PIN) == 0 ) {

              char buf[150];

              sprintf(buf, "ANS\thttp://%s\t%s\t%s\t%d\t%d\t%s\t%s", Info.W.IPA, Info.W.SID, Info.W.PWD, Info.W.HRI, Info.W.MNI, Info.V.LocalName , Info.V.HostName ); 
              Serial.println(buf);
              delay(2000);
                              
            } else {            
                Serial.println("MSG\tWrong PIN!");
                delay(2000);
            }             
            
        } 

       if( tCMD.compareTo("CID") == 0 ) 
        {
            oPIN = Serial.readStringUntil('\n');

              char buf[150];

              sprintf(buf, "CID\t%s", DeviceID );
              Serial.println(buf);
              delay(2000);          
        }         
        
       if( tCMD.compareTo("RST") == 0 ) 
        {
            oPIN = Serial.readStringUntil('\n');

            Authority = true;

            SystemReset();           
        } 
     }  
    } 
  
}

void SystemReset()
{
 // if( Authority == false ) return;

  Serial.println("MSG\tPreparing for reset!"); 
  server.send(404, "text/plain", "Now preparing for reset!\nWait for a moment, please!");    
  colorWipe(strip.Color(0, 255, 255), 50); // Cyan

  memset(Info.W.SID, 0, sizeof(Info) );

  strcpy(Info.W.PIN, "0000");
  strcpy(Info.V.LocalName, "heater");
  strcpy(Info.V.HostName,  "wfshost");
  
  Info.W.HRI = 12;
  Info.W.MNI = 0;

  Info.T[0].g_start_hour = 22;
  Info.T[0].g_end_hour = 4;
  Info.T[1].g_start_hour = 4;
  Info.T[1].g_end_hour = 10;
  Info.T[2].g_start_hour = 10;
  Info.T[2].g_end_hour = 16;
  Info.T[3].g_start_hour = 16;
  Info.T[3].g_end_hour = 22;    
    
  WriteAllToEEPROM();

  server.send(404, "text/plain", "Now, System Rebooting!"); 
  Serial.println("MSG\tSystem Rebooting!");

  colorWipe(strip.Color(255, 0, 255), 50); // Magenta

  delay(2000);

  colorWipe(strip.Color(0, 0, 0), 50);  // No color
  
  ESP.reset();                    

}

void TakeLocaIP(String TempIPAddress)
{
  char buf[100];
  
  TempIPAddress.toCharArray(Info.W.IPA, TempIPAddress.length() + 1 );

  sprintf(buf, "MSG\thttp://%s", Info.W.IPA  );
  Serial.println(buf);

  delay(2000);
}

int Connect_WIFI()
{
  long   check_time  ;
  char   buf[50];
  int    l_count;


  ssid     = Info.W.SID;
  password = Info.W.PWD;

/*
  check_time = millis();

  while ( strlen(Info.W.SID) == 0 )
  {
    Serial.println("MSG\tInput SSID , please !");
    while( millis() - check_time < 5000 ) { 
      SerialBle();
    }
    check_time = millis();
  }
  
 // check_time = millis();

 */
  colorWipe(strip.Color(0, 255, 0), 50); // Green  
 
  WiFi.mode(WIFI_STA);
  
  delay(1000);  

  if( WiFi.hostname(Info.V.HostName) == true ) {  
    Serial.println("MSG\tHostname set to " + WiFi.hostname() );
    delay(2000);
  }
  
  WiFi.begin(ssid, password);
  // Wait for connection
  l_count = 0;
  check_time = millis();  
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("MSG\tWaiting to connect !");
    while( millis() - check_time < 5000 ) { 
      SerialBle();
    }
    check_time = millis();
    l_count += 1;
    if(l_count > 4 ) return -1; // Wrong Password!
  }
  TakeLocaIP(  WiFi.localIP().toString( ) );

  return 0;
}

void infoFunc()
{   
  server.send(200, "text/html", contentLogin);
}

void rootFunc()
{
    MakeWebpageBody1();
    server.send(200, "text/html", webPageBody );
}

void branchFunc()
{
   String whButton;

    whButton = server.arg("SMT");

    if( whButton.compareTo("HBook") == 0 ) 
        hbookFunc();
    if( whButton.compareTo("TBook") == 0 ) 
        tbookFunc();
    if( whButton.compareTo("Submit") == 0 ) 
        execFunc(0);
    if( whButton.compareTo("Time Table") == 0 ) 
        execFunc(1);
    if( whButton.compareTo("Temp Table") == 0 ) 
        execFunc(2);

}

void execFunc(int bt)
{
        s_eco_tmp         = server.arg("EC").toInt();
        s_now_tmp         = server.arg("NW").toInt();
        s_duration_tmp    = server.arg("DR").toInt();

        if( bt == 2 ) {
          if( s_now_tmp > 2  )  {
            if( s_now_tmp == 6 ) {     
              memcpy(&g_temp_tmp[0].g_checked, &Info.T[0].g_checked,  sizeof(Info.T) );
            }

            if( s_now_tmp == 7 ) {     // set
              for(int i=0; i<4; i++) { 
                g_temp_tmp[i].g_checked =true;
                g_temp_tmp[i].g_temp_index = 1;
              }              
            }    
            if( s_now_tmp == 8 ) {     // reset
              for(int i=0; i<4; i++) {
                g_temp_tmp[i].g_checked =false;
                g_temp_tmp[i].g_temp_index = 0;
              }  
            }
            MakeWebpageBody3(); // book temp table
          } else
            MakeWebpageBody1(); // now
        }
        if( bt == 1 ) {
          if( s_now_tmp > 2  )  {
            if( s_now_tmp == 3 ) {     
              memcpy(g_hour_tmp, &Info.H.g_hour[0], sizeof(Info.H));
            }
            if( s_now_tmp == 4 ) {     
              memset(g_hour_tmp, 1, sizeof(Info.H));      
            }    
            if( s_now_tmp == 5 ) {     
              memset(g_hour_tmp, 0, sizeof(Info.H));      
            }
            MakeWebpageBody2(); // book time table
          } else
            MakeWebpageBody1();  // now
        }
        if( bt == 0 ) {
          if( s_now_tmp < 3 )  {
              s_eco         = s_eco_tmp;
              s_now         = s_now_tmp;
              s_duration    = s_duration_tmp;
            
              n_hour     = hour(now());
              n_minute   = minute(now());
                  
              if( s_now_tmp == 1 ) {
                HeaterOn(1, "Now", "Turned on now!");  
              } 
              
              if( s_now_tmp == 2 ) {     
                HeaterOff(0, "Now", "Turned off now!");
              }
      
              MakeWebpageBody1();  // now
          } else
              MakeWebpageBody1();  // now
        }    
        server.send(200, "text/html", webPageBody );   
        
}

void hbookFunc()
{

    s_eco         = server.arg("EC").toInt();
    s_duration    = server.arg("DR").toInt();

	  Info.H.g_hour[0]  =  server.arg("H00").toInt();
	  Info.H.g_hour[1]  =  server.arg("H01").toInt();
	  Info.H.g_hour[2]  =  server.arg("H02").toInt();
	  Info.H.g_hour[3]  =  server.arg("H03").toInt();
	  Info.H.g_hour[4]  =  server.arg("H04").toInt();
	  Info.H.g_hour[5]  =  server.arg("H05").toInt();
	  Info.H.g_hour[6]  =  server.arg("H06").toInt();
	  Info.H.g_hour[7]  =  server.arg("H07").toInt();
	  Info.H.g_hour[8]  =  server.arg("H08").toInt();
	  Info.H.g_hour[9]  =  server.arg("H09").toInt();
	  Info.H.g_hour[10] =  server.arg("H10").toInt();
	  Info.H.g_hour[11] =  server.arg("H11").toInt();
	  Info.H.g_hour[12] =  server.arg("H12").toInt();
	  Info.H.g_hour[13] =  server.arg("H13").toInt();
	  Info.H.g_hour[14] =  server.arg("H14").toInt();
	  Info.H.g_hour[15] =  server.arg("H15").toInt();
	  Info.H.g_hour[16] =  server.arg("H16").toInt();
	  Info.H.g_hour[17] =  server.arg("H17").toInt();
	  Info.H.g_hour[18] =  server.arg("H18").toInt();
	  Info.H.g_hour[19] =  server.arg("H19").toInt();
	  Info.H.g_hour[20] =  server.arg("H20").toInt();
	  Info.H.g_hour[21] =  server.arg("H21").toInt();
	  Info.H.g_hour[22] =  server.arg("H22").toInt();
	  Info.H.g_hour[23] =  server.arg("H23").toInt();
	  
    n_hour     = hour(now());
    n_minute   = minute(now());

    int  hour_sw = 0;
    for(int i=0; i<24; i++)
    {
        if( Info.H.g_hour[i] == 1 ) 
           hour_sw = 1;
    }
    if( hour_sw == 1 ) {
      s_now          = 3;
      strLongMessage = "Hour Booked!" ;
    } else {
      s_now          = 0;
      strLongMessage = "Canceled booking!" ;
    }
      
    MakeWebpageBody1();
  
    server.send(200, "text/html", webPageBody );
                
    WriteToEEPROM(long(Info.H.g_hour), sizeof(Info.H.g_hour) ) ;

}

void tbookFunc()
{

    s_eco         = server.arg("EC").toInt();
    s_duration    = server.arg("DR").toInt();

	  Info.T[0].g_checked  =  server.arg("T00").toInt();
	  Info.T[1].g_checked  =  server.arg("T01").toInt();
	  Info.T[2].g_checked  =  server.arg("T02").toInt();
	  Info.T[3].g_checked  =  server.arg("T03").toInt();

/*
    Info.T[0].g_start_hour = 22;
    Info.T[0].g_end_hour = 4;
    Info.T[1].g_start_hour = 4;
    Info.T[1].g_end_hour = 10;
    Info.T[2].g_start_hour = 10;
    Info.T[2].g_end_hour = 16;
    Info.T[3].g_start_hour = 16;
    Info.T[3].g_end_hour = 22;    
*/

    Info.T[0].g_temp_index  =  server.arg("T10").toInt();
    Info.T[1].g_temp_index  =  server.arg("T11").toInt();
    Info.T[2].g_temp_index  =  server.arg("T12").toInt();
    Info.T[3].g_temp_index  =  server.arg("T13").toInt();

	  
    n_hour     = hour(now());
    n_minute   = minute(now());

    int  temp_sw = 0;
    for(int i=0; i<4; i++)
    {
        if( Info.T[i].g_checked == true ) 
           temp_sw = 1;
    }
    if( temp_sw == 1 ) {
      s_now          = 6;
      strLongMessage = "Temperature Booked!" ;
    } else {
      s_now          = 0;
      strLongMessage = "Canceled booking!" ;
    }
      
    MakeWebpageBody1();
  
    server.send(200, "text/html", webPageBody );
                
    WriteToEEPROM(long(&Info.T[0].g_checked ), sizeof(Info.T) ) ;

}


void Connect_WebServer()
{
   String strMDNS;
   
  if (mdns.begin(Info.V.LocalName, WiFi.localIP())) {
    strMDNS = "MSG\tMDNS " + String(Info.V.LocalName) + " started";
    Serial.println(strMDNS);
    delay(2000);
  }
  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);  
    
  server.on("/",  rootFunc);
  server.on("/exec",   HTTP_POST, branchFunc);
  server.on("/info",   infoFunc);
  server.on("/login",  handleLogin);
  server.on("/setup",  HTTP_POST,  handleSetup);
  server.on("/reset",  SystemReset);

  server.begin();

  colorWipe(strip.Color(0, 0, 255), 50); // Blue
  
  Serial.println("MSG\tHTTP ST server started");
  delay(2000);	
	
}

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */

//login page, also called for disconnect
void handleLogin()
{
  String msg;
  
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")){
    if (server.arg("USERNAME") == "admin" &&  server.arg("PASSWORD") == "admin" ){
        Authority = true;
        
      Serial.println("MSG\tLog in Successful!");
      delay(2000);
      
      handleRoot();
      
      return;
    }
    Authority = false;

    msg = "Wrong username/password! try again.";
    Serial.println("MSG\tLog in Failed");
    delay(2000);
  }
  
  server.send(200, "text/html", contentLogin);
}

//root page can be accessed only if authentification is ok
void handleRoot()
{

  Authority = false; 

  String content = contentHead + "<body><H2>Hello, you successfully connected to Heater Controller!</H2>";
  content += "<div class='D2'>";  
  content += "<table><tr><td>";
  if (server.hasHeader("User-Agent")){
    content += server.header("User-Agent");
  }  
  content += "</td></tr><tr><td align='center'> Device : " + String(DeviceID) + ", MAC Adr : " + String(WiFi.macAddress())+ ", Current IP Address : "+ String( Info.W.IPA ) + "</td></tr></table>";    

  content += "<form method='POST'action='/setup'>";
  content += "<table cellpadding='2'cellspacing='3'class='T1'>";
  
  if( ScanListCount == 0 )
     content += "<tr><td class='B3L'></td><td class='B4L'> No SSID Lists</td><td class='B3L'></td></tr>";
  else {    
    for(int i=0; i< ScanListCount; i++)
     content += ScanList[i];
  }
     
  content += "</table>";
  
  content += "<table  cellpadding='2'cellspacing='3'class='T1'>";
  content += "<tr><td class='B3R'>Time diffrence from GMT</td><td class='B4L'>";
  content += "<select name='HR' style='width:25%'>";  
  for(int i=0; i<25; i++) {
     if( i == Info.W.HRI )
       content += "<option value='" + String(i) + +"' selected >"+ strHour[i]+"</option>";
     else
       content += "<option value='" + String(i) + +"'>"+ strHour[i]+"</option>";
  }
  content += "</select>h&nbsp;";
  content += "<select name='MN' style='width:25%'>";
  for(int i=0; i<4; i++) {
     if( i == Info.W.MNI )
       content += "<option value='" + String(i) + +"' selected>"+ strMin[i]+"</option>";
     else
       content += "<option value='" + String(i) + +"'>"+ strMin[i]+"</option>";
  } 
  content += "</select>m";
  content += "</td><td class='B3L'></td></tr>";
  content += "</table>";  
  
  content += "<table  cellpadding='2'cellspacing='3'class='T1'>";
  content += "<tr><td class='B4R'>SSID Password</td><td align=left><input type='text' name='SSIDPASS' placeholder='' value=\""+String(Info.W.PWD)+"\"></td><td></td></tr>";
  content += "<tr><td class='B4R'>.local MDNS Name</td><td align=left><input type='text' name='LNAME' placeholder='heater' value=\""+String(Info.V.LocalName)+"\"></td><td></td></tr>";
  content += "<tr><td class='B4R'>Host Name</td><td align=left><input type='text' name='HNAME' placeholder='WIFISWITCH' value=\""+String(Info.V.HostName)+"\"></td><td></td></tr>";
 
  content += "<tr><td class='B4R'><td align=left><input class='MB1' type='submit' value='Submit'></td><td></td></tr>";
  content += "</table></form>";
  //content += "<table><tr><td align=left>You can reset all settings <a href=\"/reset\">Reset All</a></td></tr></table>";
  content += "</div></body></html>";
  
  server.send(200, "text/html", content);
}

//no need authentification
void handleNotFound(){
  String message = "File Not Found\nBad Argument!\n";
  /*
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  */
  server.send(404, "text/plain", message);
}


void handleSetup()
{
  String    tSID, tPWD, tName1, tName2, tHR, tMN;
  int       len;

  Serial.println("MSG\tPreparing for reset!");
  server.send(404, "text/plain", "Now preparing for reset!\nWait for a moment, please!"); 

  colorWipe(strip.Color(0, 255, 255), 50); // Cyan

  tSID = server.arg("SSIDNAME");
  tPWD = server.arg("SSIDPASS");
  tName1 = server.arg("LNAME");
  tName2 = server.arg("HNAME");

  tHR = server.arg("HR");
  tMN = server.arg("MN");

  tSID.trim();
  len = tSID.length();
  if( len > 20 ) len = 20;
  if( len != 0 ) tSID.toCharArray( Info.W.SID, len + 1);

   tPWD.trim();
  len = tPWD.length();
  if( len > 20 ) len = 20;               
  if( len != 0 )
    tPWD.toCharArray( Info.W.PWD, len + 1);
  else 
    memset(Info.W.PWD, 0, sizeof(Info.W.PWD) );
    
  strcpy(Info.W.PIN,"0000");
  
  Info.W.HRI = server.arg("HR").toInt();
  Info.W.MNI = server.arg("MN").toInt();

  WriteToEEPROM(long(Info.W.SID), sizeof(Info.W));  

  Serial.println("MSG\tSaving!");  
  colorWipe(strip.Color(255, 255, 0), 50); // Yellow

  memset(Info.V.LocalName, 0, sizeof(Info.V) );
  
  tName1.trim();
  len = tName1.length();
  if( len > 20 ) len = 20;
  if( len == 0 ) tName1 = "heater";
  tName1.toCharArray( Info.V.LocalName, tName1.length() + 1);  

  tName2.trim();
  len = tName2.length();
  if( len > 20 ) len = 20;
  if( len == 0 ) tName2 = "WIFISTWITCH";
  tName2.toCharArray( Info.V.HostName, tName2.length() + 1);  
  
  WriteToEEPROM(long(Info.V.LocalName), sizeof(Info.V));
  
  server.send(404, "text/plain", "Now, System Rebooting!"); 
  Serial.println("MSG\tSystem Rebooting!");

  colorWipe(strip.Color(255, 0, 255), 50); // Magenta

  delay(2000);

  colorWipe(strip.Color(0, 0, 0), 50);  // No color
  
  ESP.reset();

}


void Scanning()
{
  char buf[50];
  
 // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  delay(1000);

  Serial.println("MSG\tStart WiFi Scan!");
  delay(2000);

  // WiFi.scanNetworks will return the number of networks found
  ScanListCount = WiFi.scanNetworks();
  if (ScanListCount == 0) {
    ScanResult = -1;
    Serial.println("MSG\tNo networks found!");
   delay(2000);
  } else {
    sprintf(buf,"MSG\t%d networks found!",ScanListCount);
    Serial.println(buf);
    delay(2000);
    for (int i = 0; i < ScanListCount; i++)
    {
      ScanList[i] = "<tr><td width='30%'></td><td align='left'> <input type='radio' name='SSIDNAME' value=\""+WiFi.SSID(i)+"\"";
      if( WiFi.SSID(i).compareTo(Info.W.SID) == 0 ) { 
        Serial.println("MSG\tMatched!");
        delay(2000);
        ScanList[i] += "checked";
        ScanResult = 1;
      }
      ScanList[i] += ">"+ WiFi.SSID(i)+ "(" + WiFi.RSSI(i) +")</td><td width='30%'></td></tr>";
      delay(10);
    }
  }
  
}

void APMode()
{
  WiFi.mode(WIFI_AP); 

  Serial.println("MSG\tConfiguring AP Server!");
  delay(1000);  
  
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP("WIFISWITCH");
  delay(1000);

  TakeLocaIP("192.168.4.1");
  
  server.on("/",      handleRoot);
  server.on("/setup", handleSetup);
  server.on("/reset", SystemReset);
  
  server.begin();
  Serial.println("MSG\tHTTP AP server started");    
}
