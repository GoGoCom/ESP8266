
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 

#include <DHT.h>
#include <TimeLib.h>       

#include <PubSubClient.h>
#include <ArduinoJson.h>

#define  TimeGab    9
#define  RELAY_ON   LOW
#define  RELAY_OFF  HIGH

#define  LED_ON     HIGH
#define  LED_OFF   LOW

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
// DHT Sensor
const int DHTPin = 13;
const int RELAYPin = 4;   // gpio4

// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

// MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Global variables
char DeviceID[10] = "JNL000000";

const char* ssid         = "COREANHOUSE";
const char* password     = "COREAN000";
const char* mqtt_server  = "broker.mqttdashboard.com";
const int   mqtt_port    = 1883; 

String strMessage;
float  TempC, TempF, TempH;
int    gab_second, n_second;  

void setup() {
  delay(1000);
  
  Serial.begin(9600); //115200);
  Serial.println("MSG\tSystem Initialize !");

  n_second   = 0;  
  strMessage  = "";

  pinMode(RELAYPin, OUTPUT);
  digitalWrite(RELAYPin, RELAY_OFF); // off

  dht.begin();

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Get time from Internaet  
  setTimeFromUTCToLocal( TimeGab );
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);  

  
}

void loop() {
  time_t  localTime;

   if (!client.connected()) {
    reconnect(); 
    n_second = second(now());
  }

  client.loop();

  localTime = now();
  
  if( second(localTime) - n_second >= 10 )    
      gab_second = second(localTime) - n_second;
  else 
      gab_second = 60 + second(localTime) - n_second;
  
  if( gab_second == 10 ) {
    n_second = (char) second(localTime);
    
    getTemperature();
    Send_BasicInfo();     
  }    

  //delay(10000);  // 10 sec
}

void setTimeFromUTCToLocal(time_t tgab )
{
  String  tmp_s, strDateTime;
  int     i;
  int     c_year, c_month, c_day, c_hour, c_minute, c_sec;  
  time_t  ltc;
     
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

    ltc    = now() + tgab * 3600;
    
    setTime(ltc); // Set Local Time
    tmp_s  = String(tgab) + "h";
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


//Function to print time with time zone
void printTime(time_t t, char *tf)
{
    char  buf[200];
   String week_s, month_s;

    week_s  = String(dayShortStr(weekday(t)));
    month_s = String(monthShortStr(month(t)));

    sprintf(buf, "MSG\t%02d:%02d:%02d %s %2d-%s-%4d UTC%s", hour(t), minute(t), second(t), week_s.c_str(), day(t), month_s.c_str(), year(t), tf );
    Serial.println(buf);    
    delay(2000);
}

void Send_BasicInfo()
{
  String week_s, month_s;
  char json[150], strbuf[100];
  time_t  localTime;

   strMessage.toCharArray( strbuf, strMessage.length()+1);
                
   localTime = now();

    week_s  = String(dayShortStr(weekday(localTime)));
    month_s = String(monthShortStr(month(localTime)));
    
   sprintf (json, "{\"msg\":\"%s\",\"tempC\":%2.0f,\"tempF\":%2.0f,\"humi\":%2.0f, \"time\": \"%02d:%02d:%02d %s %2d-%s-%4d\" }", strbuf,  TempC, TempF , TempH, hour(localTime), minute(localTime), second(localTime), week_s.c_str(), day(localTime), month_s.c_str(), year(localTime) );

   Serial.print("Publish message: ");
   Serial.println(json);
   client.publish("JNL000000/wifi/basicInfo", json);
}
      
void callback(char* topic, byte* payload, unsigned int length)
{
  const char* JOnOff;
  char OnOff[length];

  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]( ");
  Serial.print(length);
  Serial.print(") "); 
  
  for (int i = 0; i < length; i++) {
    // Serial.print((char)payload[i]);
    OnOff[i] = (char)payload[i];
  }
 
  if ( OnOff[0] == '1') {         
     RelayOn();          
  }
  if ( OnOff[0] == '0') {
     RelayOff();
  }  

}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("MSG\tMQTT connecting...");   
    
    // Attempt to connect    
    if (client.connect( DeviceID ) ) {
      Serial.println("MSG\tMQTT connected");
      //      
      if(client.subscribe("JNL000000/wifi/switch/relay", 1) == 0 ) {
        Serial.println("MSG\tMQTT subscribe failed!");
      }
    } else {
      delay(5000);
      Serial.print("MSG\tfailed, rc=");
      Serial.println(client.state());
    }
  }
  
} 

void RelayOn()
{
     digitalWrite(RELAYPin, RELAY_ON);
     
     strMessage = "Relays on now";
     Serial.println("MSG\t"+strMessage );
}

void RelayOff()
{
     digitalWrite(RELAYPin, RELAY_OFF);
     
     strMessage = "Relays off now";
     Serial.println("MSG\t"+strMessage );
}

void getTemperature(void){  
    // Read humidity
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);
    
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      //Serial.println("MSG\tFailed from DHT!");
      TempC = -0;
      TempF = -0;
      TempH = -0;
    }
    else{
      // Computes temperature values in Celsius + Fahrenheit and Humidity
      float hic = dht.computeHeatIndex(t, h, false);       
      float hif = dht.computeHeatIndex(f, h);
      
      TempC = hic;
      TempF = hif;
      TempH = h;      
    }   
    
}
