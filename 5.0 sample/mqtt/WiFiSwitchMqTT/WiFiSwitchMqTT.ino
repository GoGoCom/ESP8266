/*********
 * 
  Rui Santos
  Complete project details at http://randomnerdtutorials.com  
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
#include <EEPROM.h>

#include <PubSubClient.h>
#include <ArduinoJson.h>

#include <TimeLib.h>        //http://www.arduino.cc/playground/Code/Time
#include <Timezone.h>    //https://github.com/JChristensen/Timezone

#include "DHT.h"


const int relay_pin = 4;   // gpio4
const int buzzer_pin = 12; // gpio15
const int led_pin  = 14;    // gpio12
const int led_pin2 = 16;    // gpio12

// Uncomment one of the lines below for whatever DHT sensor type you're using!
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

#define  RELAY_ON   LOW
#define  RELAY_OFF  HIGH

#define  LED_ON   HIGH
#define  LED_OFF  LOW

// DHT Sensor
const int DHTPin = 13;
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

// MQTT
WiFiClient espClient;
PubSubClient client(espClient);
//
#define  SubTopicNumber       7

#define  SubTopicHours        0
#define  SubTopicSethours     1
#define  SubTopicTemperature  2
#define  SubTopicDuration     3
#define  SubTopicNow          4
#define  SubTopicBook         5
#define  SubTopicStatus       6
//
#define  PubTopicNumber       3

#define  PubTopicBasicInfo    0
#define  PubTopicStatusInfo   1
#define  PubTopicStatusHours  2

char DeviceID[10] = "JNL000000";

char *SubTopicMsg[SubTopicNumber] = { 
      "JNL000000/wifi/switch/hours",
      "JNL000000/wifi/switch/sethours",
      "JNL000000/wifi/switch/temperature",
      "JNL000000/wifi/switch/duration",
      "JNL000000/wifi/switch/now",
      "JNL000000/wifi/switch/book",
      "JNL000000/wifi/switch/Status"
};

char *PubTopicMsg[PubTopicNumber] = {
      "JNL000000/wifi/basicInfo",
      "JNL000000/wifi/statusInfo",
      "JNL000000/wifi/statusHours"
};


#define NUM_LEDS 2
#define BRIGHTNESS 20

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, led_pin, NEO_GRB + NEO_KHZ800);

#define  HeaterNowON     strip.Color(255,255, 0)
#define  HeaterNowOFF    strip.Color(0, 0, 255)

#define  HeaterBookON    strip.Color(0, 255, 255)
#define  HeaterBookOFF   strip.Color(255, 0, 0)

#define  BookON          strip.Color(255, 0, 255)
#define  BookOFF         strip.Color(0, 255, 0)

#define  MQTTConnect     strip.Color(255, 255, 255)

//Australia Eastern Time Zone (Sydney, Melbourne)
TimeChangeRule aEDT = {"AEDT", First, Sun, Oct, 2, 660};    //UTC + 11 hours
TimeChangeRule aEST = {"AEST", First, Sun, Apr, 3, 600};    //UTC + 10 hours
Timezone ausET(aEDT, aEST);

//Central European Time (Frankfurt, Paris)
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     //Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       //Central European Standard Time
Timezone CE(CEST, CET);

//United Kingdom (London, Belfast)
TimeChangeRule BST = {"BST", Last, Sun, Mar, 1, 60};        //British Summer Time
TimeChangeRule GMT = {"GMT", Last, Sun, Oct, 2, 0};         //Standard Time
Timezone UK(BST, GMT);

//US Eastern Time Zone (New York, Detroit)
TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240};  //Eastern Daylight Time = UTC - 4 hours
TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -300};   //Eastern Standard Time = UTC - 5 hours
Timezone usET(usEDT, usEST);

//US Central Time Zone (Chicago, Houston)
TimeChangeRule usCDT = {"CDT", Second, dowSunday, Mar, 2, -300};
TimeChangeRule usCST = {"CST", First, dowSunday, Nov, 2, -360};
Timezone usCT(usCDT, usCST);

//US Mountain Time Zone (Denver, Salt Lake City)
TimeChangeRule usMDT = {"MDT", Second, dowSunday, Mar, 2, -360};
TimeChangeRule usMST = {"MST", First, dowSunday, Nov, 2, -420};
Timezone usMT(usMDT, usMST);

//Arizona is US Mountain Time Zone but does not use DST
Timezone usAZ(usMST, usMST);

//US Pacific Time Zone (Las Vegas, Los Angeles)
TimeChangeRule usPDT = {"PDT", Second, dowSunday, Mar, 2, -420};
TimeChangeRule usPST = {"PST", First, dowSunday, Nov, 2, -480};
Timezone usPT(usPDT, usPST);

TimeChangeRule *tcr;        //pointer to the time change rule, use to get the TZ abbrev
time_t utc, ltc;

// Temporary variables
static char celsiusTemp[10];
static char fahrenheitTemp[10];
static char humidityTemp[10];

// Replace with your network credentials
char* ssid         = "COREANHOUSE";
char* password     = "COREAN000";
//char* mqtt_server  = "192.168.31.60"; //"broker.mqttdashboard.com";
char* mqtt_server  = "broker.mqttdashboard.com";
uint16_t mqtt_port = 1883; 

//IPAddress  mqtt_server(192,168,31,60);

String strShortMessage, strLongMessage;

const int  g_duration[7] = { 10, 15, 20, 25, 30, 45, 50 };

int  c_year = 0, c_month = 0, c_day = 0, c_hour = 0, c_minute = 0, c_sec = 0;
int  gab_minute, gab_second;
int  s_mqtt_now, s_led_now, s_temperature, s_now, s_duration, s_book, n_hour, n_minute, n_second;  

float TempC = 0.0, TempF = 0.0, TempH = 0.0;

unsigned long PixelTi, RainbowTi;
bool PixelJigJag;
uint8_t  PixelPos;

struct WIFI {
  char  SID[21];
  char  PWD[21];
  char  PIN[21];
  char  IPA[21];
  char  ZON[21];
  char  REV[21];
};

struct HOUR {
  char  g_hour[24];
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

struct {
  struct WIFI W;
  struct HOUR H;
  struct MQTT M;  
} Info;

void make_id(void)
{
  int i;

    sprintf(&DeviceID[3], "%06X", ESP.getChipId());    
    
    for(i=0; i<SubTopicNumber; i++)
    {
      strncpy( SubTopicMsg[i], DeviceID, 9 );
      //Serial.println(SubTopicMsg[i]);
    }
    
    for(i=0; i<PubTopicNumber; i++)
    {
      strncpy( PubTopicMsg[i], DeviceID, 9 );
      //Serial.println(PubTopicMsg[i]);
    }
  
}


void Initialize()
{
 

//ESP.wdtDisable();
  ESP.wdtEnable(WDTO_8S);

  s_now      = 0;
  s_duration = 0;
  n_hour     = 0;
  n_minute   = 0;
  n_second   = 0;  
  s_temperature = 15;
  s_led_now   = 0;
  s_mqtt_now  = 1;
  strShortMessage  = "";
  strLongMessage  = "";
  PixelPos    = 0;
  PixelJigJag = false;

  pinMode(relay_pin, OUTPUT);
  pinMode(buzzer_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
  pinMode(led_pin2, OUTPUT);
  
  digitalWrite(led_pin,    LED_OFF);    // On
  digitalWrite(led_pin2,   LED_ON);    // On
  digitalWrite(relay_pin, RELAY_OFF); // off
  
  Serial.begin(9600); //115200);

  //make_id() ;
 
  tone(buzzer_pin, 392, 1000);
  dht.begin();
  EEPROM.begin( sizeof(Info) );

  ReadAllFromEEPROM();
  
  Info.W.ZON[0] = 'U';
  Info.W.ZON[1] = 'T';
  Info.W.ZON[2] = 'C';            
  Info.W.ZON[3] = 0;            


  Serial.println("MSG\tSystem Initialize !");

  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  colorWipe(strip.Color(255, 0, 0), 50); // Red

    
}


void timebow( ) {

  if(  millis() - RainbowTi > 50  ) {

     if(  s_led_now  == 0 ) {
             strip.setPixelColor(0, Wheel(PixelPos & 255));
             if( s_mqtt_now == 0 ) 
              strip.setPixelColor(1, Wheel(PixelPos & 255));
             else
              strip.setPixelColor(1, MQTTConnect);
             strip.show();               
             PixelPos ++;
     }
    RainbowTi =  millis();    
  }

  if(  millis() - PixelTi > 1000  ) {

     if(  s_led_now  != 0 ) {
        if( PixelJigJag ) {
    
          if(  s_led_now  == 1) {
             strip.setPixelColor(0, HeaterNowON);
             if( s_mqtt_now == 0 ) 
              strip.setPixelColor(1, HeaterNowOFF);
             else
              strip.setPixelColor(1, MQTTConnect);
          }
          if(  s_led_now  == 2) {
             strip.setPixelColor(0, HeaterBookON);
             if( s_mqtt_now == 0 ) 
              strip.setPixelColor(1, HeaterBookOFF);
             else
              strip.setPixelColor(1, MQTTConnect);
          }
          if(  s_led_now  == 3) {
             strip.setPixelColor(0, BookON);
             if( s_mqtt_now == 0 ) 
              strip.setPixelColor(1, BookOFF);
             else
              strip.setPixelColor(1, MQTTConnect);
          }
    
          PixelJigJag = false;
    
        }
      else {
    
          if(  s_led_now  == 1) {
             if( s_mqtt_now == 1 ) 
              strip.setPixelColor(0, MQTTConnect);
             else
              strip.setPixelColor(0, HeaterNowOFF);
             strip.setPixelColor(1, HeaterNowON);
          }
          if(  s_led_now  == 2) {
             if( s_mqtt_now == 1 ) 
              strip.setPixelColor(0, MQTTConnect);
             else
              strip.setPixelColor(0, HeaterBookOFF);
             strip.setPixelColor(1, HeaterBookON);
          }
          if(  s_led_now  == 3) {
             if( s_mqtt_now == 1 ) 
              strip.setPixelColor(0, MQTTConnect);
             else
              strip.setPixelColor(0, BookOFF);
             strip.setPixelColor(1, BookON);
          }
    
          PixelJigJag = true;    
    
        }
        strip.show();        
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

void setup(void)
{

   Initialize();
   
   Connect_WIFI();

   Connect_MQTT();
   
   setTimeFromUTCToLocal( ); // Set System Time from Internet

   tone(buzzer_pin, 329, 1000);

   PixelTi =  millis();
   RainbowTi  =  millis();  
      
}

void Send_Status()
{
        char json[200];

          sprintf (json, "{\"Switch\":\"SetDuration\",\"Value\":%d }", s_duration );
          //Serial.print("Publish message: ");
          //Serial.println(json);
          client.publish(PubTopicMsg[PubTopicStatusInfo], json);
          
          delay(500);

          sprintf (json, "{\"Switch\":\"SetTemperature\",\"Value\":%d }", s_temperature );
          //Serial.print("Publish message: ");
          //Serial.println(json);
          client.publish(PubTopicMsg[PubTopicStatusInfo], json);
          
          delay(500);

          sprintf (json, "{\"Switch\":\"SetNow\",\"Value\":%d }",  s_now  );
          //Serial.print("Publish message: ");
          //Serial.println(json);
          client.publish(PubTopicMsg[PubTopicStatusInfo], json);

          delay(500);
          
          sprintf (json, "{\"Switch\":\"SetBook\",\"Value\":%d }", s_book );
          //Serial.print("Publish message: ");
          //Serial.println(json);
          client.publish(PubTopicMsg[PubTopicStatusInfo], json);
      
          delay(500);

          sprintf (json, "{\"hours\":[%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d] }",
          Info.H.g_hour[0], Info.H.g_hour[1], Info.H.g_hour[2], Info.H.g_hour[3], Info.H.g_hour[4], Info.H.g_hour[5], Info.H.g_hour[6], Info.H.g_hour[7]
          , Info.H.g_hour[8], Info.H.g_hour[9], Info.H.g_hour[10], Info.H.g_hour[11], Info.H.g_hour[12], Info.H.g_hour[13], Info.H.g_hour[14], Info.H.g_hour[15], Info.H.g_hour[16]
          , Info.H.g_hour[17], Info.H.g_hour[18], Info.H.g_hour[19], Info.H.g_hour[20], Info.H.g_hour[21], Info.H.g_hour[22], Info.H.g_hour[23] );
      
          //Serial.print("Publish message: ");
          //Serial.println(json);
          client.publish(PubTopicMsg[PubTopicStatusHours], json);
              
          delay(500);
 }

void Send_BasicInfo()
{
  char json[150], strbuf[100];
  time_t  localTime;
   String week_s, month_s;

    week_s  = String(dayShortStr(weekday(localTime)));
    month_s = String(monthShortStr(month(localTime)));

   strLongMessage.toCharArray( strbuf, strLongMessage.length()+1);
                
   localTime = now();
    
   sprintf (json, "{\"msg\":\"%s\",\"tempC\":%2.0f,\"tempF\":%2.0f,\"humi\":%2.0f, \"time\": \"%02d:%02d:%02d %s %2d-%s-%4d\" }", strbuf,  TempC, TempF , TempH, hour(localTime), minute(localTime), second(localTime), week_s.c_str(), day(localTime), month_s.c_str(), year(localTime) );

   //sprintf (json, "{\"Alive\":\"%s\",\"tempC\":%2.0f,\"tempF\":%2.0f,\"humi\":%2.0f, \"timestring\": \"%02d:%02d:%02d\" }", strbuf,  TempC, TempF , TempH, hour(localTime), minute(localTime), second(localTime) );
      
   //Serial.print("Publish message: ");
   //Serial.println(json);
   client.publish(PubTopicMsg[PubTopicBasicInfo], json);
}
      
void callback(char* topic, byte* payload, unsigned int length)
{
  const char* JOnOff;
  char OnOff[length];
  StaticJsonDocument<100> doc;

 /* 
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]( ");
  Serial.print(length);
  Serial.print(") "); 
  
  for (int i = 0; i < length; i++) {
    // Serial.print((char)payload[i]);
    OnOff[i] = (char)payload[i];
  }
  // Serial.println();
  */

  if( strcmp(topic, SubTopicMsg[SubTopicHours] ) == 0 ) {

      JsonObject root = doc.as<JsonObject>(); // get the root object     
      auto error = deserializeJson(doc, payload);
      if (error) {
          Serial.print(F("deserializeJson() failed with code "));
          Serial.println(error.c_str());
          return;
      }
      for(int i=0; i< 24; i++)
      {
        char key[4];
        
        sprintf(key, "H%02d", i );  
      
        if (root.containsKey(key))
        {
          JOnOff = root[key];
      
           //Serial.print(i);
      
          if ( JOnOff[0] == '1') {
             //Serial.println("Hour ON");
             Info.H.g_hour[i] = 1;
          }
          if ( JOnOff[0] == '0') {
            //Serial.println("Hour OFF");
             Info.H.g_hour[i] = 0;
          }
          WriteToEEPROM(long(&Info.H.g_hour[i]), sizeof(Info.H.g_hour[i]) ) ;
        }
      
      }

  } 

  if( strcmp(topic, SubTopicMsg[SubTopicSethours] ) == 0 ) {
      //OnOff = payload[0];
              
      if ( OnOff[0] == '1') {
      //Serial.println("Set ");
        for(int i=0; i< 24; i++)
        {
           Info.H.g_hour[i] = 1;
        }
      }
      if ( OnOff[0] == '0') {
      //Serial.println("Reset ");
        for(int i=0; i< 24; i++)
        {
           Info.H.g_hour[i] = 0;
        }
      }
           
      Send_Status();
  } 


  if( strcmp(topic, SubTopicMsg[SubTopicNow] ) == 0 ) {
	    //OnOff = payload[0];
	        
		  if ( OnOff[0] == '1') {
          char json[200];
           
          sprintf (json, "{\"Switch\":\"SetBook\",\"Value\":%d }", 0 );
          //Serial.print("Publish message: ");
          //Serial.println(json);
          client.publish(PubTopicMsg[PubTopicStatusInfo], json);   
      
		     //Serial.println("Now ON");
		     s_now  = 1;
         s_book = 0;

         n_hour   = hour(now());
         n_minute = minute(now());
          
		     HeaterOn(1, "Now", "Turned on now!");  
        
		  }
		  if ( OnOff[0] == '0') {
		    //Serial.println("Now OFF");
		     s_now = 0;
		     HeaterOff(0, "Now", "Turned off now!");
		  }  
  }
  
  if( strcmp(topic, SubTopicMsg[SubTopicBook] ) == 0 ) {
      //OnOff = payload[0];
        
		  if ( OnOff[0] == '1') {
          char json[200];
          
          sprintf (json, "{\"Switch\":\"SetNow\",\"Value\":%d }",  0  );
          //Serial.print("Publish message: ");
          //Serial.println(json);
          client.publish(PubTopicMsg[PubTopicStatusInfo], json);
                
		     //Serial.println("Book ON");
         s_now  = 0;
		     s_book = 1;
		  }
		  if ( OnOff[0] == '0') {
		    //Serial.println("Book OFF");
		     s_book = 0;
		  }
    
  }

  if( strcmp(topic, SubTopicMsg[SubTopicDuration] ) == 0 ) {
      //OnOff = payload[0];
     s_duration = atoi((char *) payload); //OnOff[0] - 0x30;

     //Serial.print(g_duration[s_duration] );
     //Serial.println(" minutes");
  }

  if( strcmp(topic, SubTopicMsg[SubTopicTemperature] ) == 0 ) {
      //OnOff = payload[0];
     s_temperature = atoi((char *) payload);

     //Serial.print( s_temperature );
     //Serial.println(" C");
  }
 
  if( strcmp(topic, SubTopicMsg[SubTopicStatus] ) == 0 ) {
     Send_Status();
	}  
 
}

void reconnect()
{
 long check_time  ;
 char TopicBuf[50];
 String Temp;

  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("MSG\tMQTT connecting...");   
    s_mqtt_now = 1;  
    // Attempt to connect
//    if (client.connect(DeviceID , Info.M.USR, Info.M.PAS, Info.M.TPC, Info.M.QOS, Info.M.RET, Info.M.MES) ) {
    
    if (client.connect( DeviceID ) ) {
      delay(2000);
      Serial.println("MSG\tMQTT connected");
      //Serial.println(DeviceID);
      s_mqtt_now = 0;
      //      

      for(int i=0; i<SubTopicNumber; i++)
      {      
        //Serial.println(SubTopicMsg[i]);
        if(client.subscribe(SubTopicMsg[i], 1) == 0 ) {
          Serial.println("MSG\tMQTT subscribe failed!");
        }
        delay(100);
      }  
   
/*      
      client.subscribe("gazet/wifi/switch/hours");
      client.subscribe("gazet/wifi/switch/sethours");
      client.subscribe("gazet/wifi/switch/temperature");
      client.subscribe("gazet/wifi/switch/duration");
      client.subscribe("gazet/wifi/switch/now");
      client.subscribe("gazet/wifi/switch/book");
      client.subscribe("gazet/wifi/switch/Status");
*/
      
      tone(buzzer_pin, 523, 1000);

      Send_Status();
      Send_BasicInfo();
      
    } else {
       delay(5000);
      Serial.print("MSG\tfailed, rc=");
      Serial.print(client.state());
      //Serial.println("MSG\tTry again in 5 sec");
      // Wait 5 seconds before retrying
      //check_time = millis();
      //while( millis() - check_time < 5000 ) { 
//        timebow( ); // Strip LED per second
        //SerialBle();
      //}

    }
  }
  
} 

void loop(void)
{
  time_t  localTime;

    ESP.wdtFeed();

  if (!client.connected()) {
    reconnect(); 
    n_second = second(now());
  }

    client.loop();

    timebow( ); // Strip LED per second
    
    SerialBle();
  
    localTime = now();

    if( second(localTime) - n_second >= 10 )    
        gab_second = second(localTime) - n_second;
    else 
        gab_second = 60 + second(localTime) - n_second;
    
    if( gab_second == 10 ) {

      n_second = (char) second(localTime);

      if( ESP.getVcc() < 2750 ) {
        tone(buzzer_pin, 3038, 1500);
        digitalWrite(led_pin2, LED_ON);
        //Serial.println( ESP.getVcc() );
      } else {
        digitalWrite(led_pin2, LED_OFF);
        //Serial.println("MSG\taLive ! " );
      }              
      
      if( ( s_now == 1 ) && ( s_book == 0 ) ) {       // Now function
          if( minute(localTime) - n_minute  >=  0 )
              gab_minute = minute(localTime) - n_minute;
          else 
              gab_minute = 60 + minute(localTime) - n_minute;
        
         if( gab_minute  >  g_duration[s_duration]   ) {
            s_now = 0;
            HeaterOff(0, "N stop!", " Now stopped!");
         } else {
            HeaterOn(1, "N run!", " Now running!");              
         }         
      }
      if( ( s_now == 0 ) && ( s_book == 1 ) ) {        // book function
         if( Info.H.g_hour[hour(localTime)] == 1 ) { 
            if ( g_duration[s_duration] > minute(localTime) ) {
                HeaterOn(2, "B run!", "Book running!");              
            } else {            
                HeaterOff(3, "B stop!", "Book stopped!");             
            }             
          }
          else {            
            HeaterOff(3, "No book!", "No book!");
          }                 
      }      
      if( ( s_now == 0 ) && ( s_book == 0 ) ) {       // Now On
            HeaterOff(0, "Ready!", "Ready");
      }
      getTemperature();
      Send_BasicInfo();
     
  	}

	
}

void HeaterOn(int Status, String shortMsg, String longMsg)
{
      if( TempC >= s_temperature ) {
          HeaterOff(0, "Set temp!", "Over or less temperature!");          
          return;
      }
      
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


void getTemperature(void){  


    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);
    
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      //Serial.println("MSG\tFailed from DHT!");
      //delay(2000);
      strcpy(celsiusTemp,"Failed");
      strcpy(fahrenheitTemp, "Failed");
      strcpy(humidityTemp, "Failed");         
    }
    else{
      // Computes temperature values in Celsius + Fahrenheit and Humidity
      float hic = dht.computeHeatIndex(t, h, false);       
      //dtostrf(hic, 6, 0, celsiusTemp);             
      float hif = dht.computeHeatIndex(f, h);
      //dtostrf(hif, 6, 0, fahrenheitTemp);         
      //dtostrf(h, 6, 0, humidityTemp);
      
      TempC = hic;
      TempF = hif;
      TempH = h;
      
      /*
      // You can delete the following Serial.print's, it's just for debugging purposes
      Serial.print("Humidity: ");
      Serial.print(h);
      Serial.print(" %\t Temperature: ");
      Serial.print(t);
      Serial.print(" *C ");
      Serial.print(f);
      Serial.print(" *F\t Heat index: ");
      Serial.print(hic);
      Serial.print(" *C ");
      Serial.print(hif);
      Serial.print(" *F");
      */
    }
    
    
}

void setTimeFromUTCToLocal(void ) {

  String  tmp_s, strDateTime;
  char   *city, *abbrev;
  int    i;


     
    strDateTime = getTimeFromInternet( );

    tmp_s = strDateTime.substring(8 , 11);
    for(i=1; i<13 ; i++)
    {
      if ( String(monthShortStr(i)).equals(tmp_s) ) c_month = i;
    }
   
    c_day = strDateTime.substring(5 , 7).toInt();
    c_year = strDateTime.substring(12,16).toInt();
    c_hour = strDateTime.substring(17,19).toInt();
    c_minute = strDateTime.substring(20,22).toInt();        
    c_sec = strDateTime.substring(23,25).toInt();

    setTime(c_hour, c_minute, c_sec, c_day, c_month, c_year); // Set Time to UTC

    utc    = now();
    ltc    = utc;
    city   = "Universal";
    //abbrev = "UTC";
    
    if( ( strcmp( Info.W.ZON, "AEDT" ) == 0 ) || ( strcmp( Info.W.ZON, "AEST" ) == 0 ) ){
        ltc    = ausET.toLocal(utc, &tcr);
        city   = "Sydney";
        abbrev = tcr -> abbrev;
    }
    if( strcmp( Info.W.ZON, "CE" ) == 0 ) {
        ltc    = CE.toLocal(utc, &tcr);
        city   = "Paris";
        abbrev = tcr -> abbrev;
    }
    if( strcmp( Info.W.ZON, "UK" ) == 0 ) {
        ltc    = UK.toLocal(utc, &tcr);
        city   = "London";
        abbrev = tcr -> abbrev;
    }
    if( strcmp( Info.W.ZON, "USET" ) == 0 ) {
        ltc    = usET.toLocal(utc, &tcr);
        city   = "New York";
        abbrev = tcr -> abbrev;
    }
   
    setTime(ltc); // Set Local Time
    printTime(ltc, Info.W.ZON, city);    
    
    delay(2000);
    
    /*
    printTime(utc, "UTC", " Universal Coordinated Time");
    printTime(usCT.toLocal(utc, &tcr), tcr -> abbrev, " Chicago");
    printTime(usMT.toLocal(utc, &tcr), tcr -> abbrev, " Denver");
    printTime(usAZ.toLocal(utc, &tcr), tcr -> abbrev, " Phoenix");
    printTime(usPT.toLocal(utc, &tcr), tcr -> abbrev, " Los Angeles");
    */
    
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
void printTime(time_t t, char *tz, char *loc)
{

    char  buf[200];
   String week_s, month_s;

    week_s  = String(dayShortStr(weekday(t)));
    month_s = String(monthShortStr(month(t)));

    sprintf(buf, "MSG\t%02d:%02d:%02d %s %2d-%s-%4d %s %s", hour(t), minute(t), second(t),week_s.c_str(), day(t), month_s.c_str(), year(t), tz, loc );
    Serial.println(buf);    
    
}

//Print an integer in "00" format (with leading zero).
//Input value assumed to be between 0 and 99.
void sPrintI00(int val)
{
    if (val < 10) Serial.print('0');
    Serial.print(val, DEC);
    return;
}

//Print an integer in ":00" format (with leading zero).
//Input value assumed to be between 0 and 99.
void sPrintDigits(int val)
{
    Serial.print(':');
    if(val < 10) Serial.print('0');
    Serial.print(val, DEC);
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

String oPIN, tCMD, tSID, tPWD, tPIN, tIPA, tZON;
String mSVR, mPOT, mUSR, mPAS, mTPC, mQOS, mRET, mMES;
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
            tZON = Serial.readStringUntil('\n');
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

                tZON.trim();
                len = tZON.length();
                if( len > 20 ) len = 20;
                tZON.toCharArray( Info.W.ZON, len + 1);

                WriteToEEPROM(long(Info.W.SID), sizeof(Info.W));

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

              sprintf(buf, "ANS\thttp://%s\t%s\t%s", Info.W.IPA, Info.W.SID, Info.W.PWD );
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

            memset(Info.W.SID, 0, sizeof(Info) );
            
            Info.W.PIN[0] = '0';
            Info.W.PIN[1] = '0';
            Info.W.PIN[2] = '0';
            Info.W.PIN[3] = '0';

            Info.W.ZON[0] = 'U';
            Info.W.ZON[1] = 'T';
            Info.W.ZON[2] = 'C';            

            WriteAllToEEPROM();

            Serial.println("MSG\tSystem Reset & booting!");

            delay(2000);
            
            ESP.reset();                    
           
        } 
     }  
    } 
  
}

void Connect_WIFI()
{
  long   check_time  ;
  String tempIP;
  char   buf[100];

  /*
  ssid     = Info.W.SID;
  password = Info.W.PWD;

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
  
  WiFi.begin(ssid, password);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("MSG\tWaiting to connect !");
    while( millis() - check_time < 5000 ) { 
      SerialBle();
    }
    check_time = millis();
  }

  tempIP = WiFi.localIP().toString( );

  memset(Info.W.IPA, 0, sizeof(Info.W.IPA) );
  tempIP.toCharArray(Info.W.IPA, tempIP.length() + 1 );

  WriteToEEPROM( long(&Info.W.IPA), sizeof(Info.W.IPA) );
  
  sprintf(buf, "MSG\thttp://%s", Info.W.IPA  );
  Serial.println(buf);

  colorWipe(strip.Color(0, 255, 0), 50); // Green   
   
  delay(2000);
}

void Connect_MQTT()
{
   long check_time  ;
    
  /* mqtt_server     = Info.M.SVR;
  mqtt_port       = Info.M.POT;

  check_time = millis();

  while ( strlen(Info.M.SVR) == 0 )
  {
    Serial.println("MSG\tInput MQTT Server, pls!");
    while( millis() - check_time < 5000 ) { 
      SerialBle();
    }
    check_time = millis();
  }
  */
     
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  colorWipe(strip.Color(0, 0, 255), 50); // Blue
}
