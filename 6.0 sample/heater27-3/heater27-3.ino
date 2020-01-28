 /*********
  WIFISWITCH control Program Copyright by Jons & Lee Pty Ltd in Australia 2020 for Blynk
*********/

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <math.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
//#include "DHT.h"

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 13

#define  RELAY_ON   LOW
#define  RELAY_OFF  HIGH

#define  LED_ON   HIGH
#define  LED_OFF  LOW

#define BLYNK_RED       "#D3435C"

#define BLYNK_GREEN     "#23C48E"

#define BLYNK_DARK_BLUE "#5F7CD8"
#define BLYNK_BLUE      "#04C0F8"
#define BLYNK_WHITE     "#FFFFFF"
#define BLYNK_YELLOW    "#ED9D00"

// DHT Sensor
//const int DHTPin = 13;
// Uncomment one of the lines below for whatever DHT sensor type you're using!
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321


#define NUM_LEDS 2
#define BRIGHTNESS 60

#define  HeaterNowON     strip.Color(255,255, 0)
#define  HeaterNowOFF    strip.Color(0, 0, 255)

#define  HeaterBookON    strip.Color(0, 255, 255)
#define  HeaterBookOFF   strip.Color(255, 0, 0)

#define  BookON          strip.Color(255, 0, 255)
#define  BookOFF         strip.Color(0, 255, 0)

#define  STEPAON         strip.Color(  0,255, 0)
#define  STEPBON         strip.Color(255,  0, 0)
#define  STEPOFF         strip.Color(  0,  0, 0)

const String  VERSION = "V6.00";
  
const int relay_pin = 4;   // gpio4
const int buzzer_pin = 12; // gpio15
const int led_pin = 14;    // gpio12
const int led_pin2 = 16;    // gpio12

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
BlynkTimer timer;
//WidgetLCD lcd(V7);
WidgetRTC rtc;
WidgetLED led1(V7), led2(V8);
ESP8266WebServer server(80);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, led_pin, NEO_GRB + NEO_KHZ800);

// Initialize DHT sensor.
//DHT dht(DHTPin, DHTTYPE);


char DeviceID[10] = "JNL000000";

// Replace with your network credentials
char* ssid         = "COREANPLACE2";
char* password     = "COREAN000";
char* auth = "B4358wtshnop0smZ20SOcjpF_68iSoth"; //"NXpXDaKLD5sbrSU6QucIuE0EUk5eoNZS";

String strShortMessage, webPageBody = "";
String ScanList[30];
int    ScanListCount, ScanResult, ConnectResult;

int  s_led_now;
int  s_eco;

float  g_temperature;  
float  TempC = 0.0, TempF = 0.0, TempH = 0.0, TempAC = 0.0, TempAF = 0.0;

bool     PixelJigJag;
uint8_t  PixelPos;


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

static String contentHead = "<!DOCTYPE html><html><head><meta charset='utf-8'> <style>"+StyleContent+"</style> </head>";
 
struct WIFI {
  char  SID[21];
  char  PWD[21];
  char  AUT[51];
  char  REV[21];
};

struct NOW {
  boolean  g_checked;
  uint8_t  g_duration;
};

struct TEMP {
  float    g_temperature;
  uint8_t  g_start_hour;
  uint8_t  g_end_hour;
};

struct {
  struct WIFI W;
  struct NOW  N;
  struct TEMP T[4];
} Info;

// BLYNK Subroutine
BLYNK_CONNECTED() {
    // Request Blynk server to re-send latest values for all pins
    Blynk.syncAll();
}

BLYNK_WRITE(V1) // Button
{
  int pinValue = param.asInt();

  if( pinValue == 1 ) {
       led2.off(); // time mode 
       HeaterOn(1, "by button");  
  } else {
       HeaterOff(0, "by button");
  }
  Info.N.g_checked = pinValue;
}

BLYNK_WRITE(V2) // Duration Slider
{
  Info.N.g_duration = param.asInt();
}

BLYNK_WRITE(V3) // 22-04h Slider 
{
  Info.T[0].g_temperature = param.asFloat();
}

BLYNK_WRITE(V4) // 04-10h Slider
{
  Info.T[1].g_temperature = param.asFloat();
}

BLYNK_WRITE(V5) // 10-17h Slider
{
  Info.T[2].g_temperature = param.asFloat();
}

BLYNK_WRITE(V6) // 17-22h Slider
{
  Info.T[3].g_temperature = param.asFloat(); 
}

//
void Initialize()
{ 

//ESP.wdtDisable();
  ESP.wdtEnable(WDTO_8S);

  s_eco      = 0;
  
  g_temperature = 0;
  s_led_now   = 0;
  strShortMessage  = "";
  PixelPos    = 0;
  PixelJigJag = false;
  ScanListCount = 0;
  ScanResult    = 0;
  ConnectResult = -1;
  
  pinMode(relay_pin, OUTPUT);
  pinMode(buzzer_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
  pinMode(led_pin2, OUTPUT);
  
  digitalWrite(led_pin2,   LED_OFF);    
  digitalWrite(relay_pin, RELAY_OFF); 

  Serial.begin(9600); //115200);

  sprintf(&DeviceID[3], "%06X", ESP.getChipId());       

  tone(buzzer_pin, 392, 1000);
  //dht.begin();
  sensors.begin();
  
  EEPROM.begin( sizeof(Info) );

  ReadAllFromEEPROM();

  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  colorWipe(strip.Color(255, 0, 0), 50); // Red
  
  Serial.println("MSG\tSystem Initialize !");
}

void Check_status()
{
  static int gab_minute;
  int    t_hour;

  if( Info.N.g_checked == true ) {       // Now On
     gab_minute ++;           
     if( gab_minute  >  Info.N.g_duration  ) {
        Info.N.g_checked  =  false;
        Blynk.virtualWrite(V1, Info.N.g_checked); // button off automatically 
        HeaterOff(0, "by time");              
        gab_minute = 0; 
     }
     return;
  }    
  if( Info.N.g_checked == false ) {       // Now Off & Start by temp     
     t_hour = hour();        
     for(int i=0; i<4; i++ ) {      
        g_temperature = Info.T[i].g_temperature;
        if( i == 0 ) {
          if(( t_hour >= Info.T[0].g_start_hour ) || ( t_hour < Info.T[0].g_end_hour ) ) {
              led2.setColor(BLYNK_DARK_BLUE); // 22-04h temp mode
              HeaterByTemperature();
          }          
        } else {  
          if(( Info.T[i].g_start_hour <= t_hour) && ( t_hour < Info.T[i].g_end_hour ) ) {
              switch(i) {
                case 1:
                          led2.setColor(BLYNK_BLUE); // 04-10h temp mode
                          break;
                case 2:
                          led2.setColor(BLYNK_WHITE); // 10-17h temp mode
                          break;
                case 3:
                          led2.setColor(BLYNK_YELLOW); // 17-22h temp mode
                          break;
              }
              HeaterByTemperature();
          }          
        }
    }
    led2.on(); // temp mode          
  }
}

void SendStatus()
{
 //String strTime;
 
 getTemperature();
 Blynk.virtualWrite(V0, TempC);  // Gadge 
 
 //strTime = String(hour()) + ":" + String(minute()) + " " + dayShortStr(weekday()) + " " + String(day()) + "/" + monthShortStr(month()) + "/" + String(year());

 //lcd.clear();
 //lcd.print(0, 0, strShortMessage );
 //lcd.print(0, 1, strTime );   
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
            rtc.begin();
            // Setup a function to be called every timing
            timer.setInterval(50L,    TimeBow1 );                   
            timer.setInterval(1000L,  TimeBow2 );                   
            timer.setInterval(10000L, SendStatus ); 
            timer.setInterval(60000L, Check_status );
            led1.setColor(BLYNK_RED);
            tone(buzzer_pin, 329, 1000);      
            break;
          }          
       case 0:       
       case -1:
          APMode();   
       default : 
          break;
  } 
}

void loop(void)
{
  ESP.wdtFeed();  

  if( ConnectResult == 0 ) {
     Blynk.run();
     timer.run();   
  } else {
    server.handleClient();  //
  }
  
}

 
//
void TimeBow1( ) { // 50 mSec
        
  if ( ( ConnectResult == 0 ) && (  s_led_now  == 0 ) ) { // when success, waiting !
    strip.setPixelColor(0, Wheel(PixelPos & 255));
    strip.setPixelColor(1, Wheel(PixelPos & 255));
    strip.show();               
    PixelPos ++;
  }
}

void TimeBow2( ) { // 1 sec
   
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

void HeaterOn(int Status, String shortMsg)
{
     led1.on();
     s_led_now  = Status;
     digitalWrite(relay_pin, RELAY_ON);
     
     strShortMessage = " ON:" + shortMsg;
     Serial.println("MSG\t"+strShortMessage );
}

void HeaterOff(int Status, String shortMsg)
{
     led1.off();
     s_led_now  = Status;
     digitalWrite(relay_pin, RELAY_OFF);
     
     strShortMessage = "OFF:" + shortMsg;
     Serial.println("MSG\t"+strShortMessage );
}

void HeaterByTemperature(void)
{
  getTemperature();
  
  if( s_eco == 0)  {
     if(  TempC < (float) g_temperature ) {
          HeaterOn(2,  "by temperature!");              
      } else {
          HeaterOff(0, "by temperature!");
      }
   }
   if( s_eco == 1)  {
     if(  TempAC < (float) g_temperature ) {
          HeaterOn(3, "by eco-temperature!");
     } else {
          HeaterOff(0, "by eco-temperature!");
      }
  }               
}

void getTemperature(void)
{  

  sensors.requestTemperatures();

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

int Connect_WIFI()
{
  ssid     = Info.W.SID;
  password = Info.W.PWD;
  auth     = Info.W.AUT;
  
  colorWipe(strip.Color(0, 255, 0), 50); // Green  

  Blynk.begin(auth, ssid, password);

  return 0;
}

void SystemReset()
{
  Serial.println("MSG\tPreparing for reset!"); 
  server.send(404, "text/plain", "Now preparing for reset!\nWait for a moment, please!");    
  colorWipe(strip.Color(0, 255, 255), 50); // Cyan

  memset(Info.W.SID, 0, sizeof(Info) );

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

  colorWipe(strip.Color(0, 0, 0), 50);  // No color
  
  ESP.reset();                    

}


/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */

//root page can be accessed only if authentification is ok
void handleRoot()
{
  String content = contentHead + "<body><H2>Hello, you successfully connected to Heater Controller!</H2>";
  
  content += "<div class='D2'>";  
  content += "<table><tr><td>";
  if (server.hasHeader("User-Agent")){
    content += server.header("User-Agent");
  }  
  content += "</td></tr><tr><td align='center'> Device : " + String(DeviceID) + ", MAC Adr : " + String(WiFi.macAddress()) + "</td></tr></table>";    

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
  content += "<tr><td class='B4R'>SSID Password</td><td align=left><input type='text' name='SSIDPASS' placeholder='' value=\""+String(Info.W.PWD)+"\"></td><td></td></tr>";
  content += "<tr><td class='B4R'>Authentification</td><td align=left><input type='text' name='AUTHEN' placeholder='' value=\""+String(Info.W.AUT)+"\"></td><td></td></tr>";
 
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
  String    tSID, tPWD, tAUT;
  int       len;

  Serial.println("MSG\tPreparing for reset!");
  server.send(404, "text/plain", "Now preparing for reset!\nWait for a moment, please!"); 

  colorWipe(strip.Color(0, 255, 255), 50); // Cyan

  tSID = server.arg("SSIDNAME");
  tPWD = server.arg("SSIDPASS");
  tAUT = server.arg("AUTHEN");

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

  tAUT.trim();
  len = tAUT.length();
  if( len > 50 ) len = 50;
  if( len != 0 )
    tAUT.toCharArray( Info.W.AUT, len + 1);
  else 
    memset(Info.W.AUT, 0, sizeof(Info.W.AUT) );
    
  WriteToEEPROM(long(Info.W.SID), sizeof(Info.W));  

  server.send(404, "text/plain", "Now, System Rebooting!"); 
  Serial.println("MSG\tSystem Rebooting!");

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
  WiFi.mode(WIFI_AP); // "192.168.4.1"
  
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP("WIFISWITCH");
  delay(1000);

  server.on("/",      handleRoot);
  server.on("/setup", handleSetup);
  server.on("/reset", SystemReset);
  server.onNotFound(handleNotFound);
 
  server.begin();
  Serial.println("MSG\tHTTP AP server started");    
}
