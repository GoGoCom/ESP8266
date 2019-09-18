
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define  RELAY_ON   LOW
#define  RELAY_OFF  HIGH
#define  DHTTYPE    DHT22   // DHT 22  (AM2302), AM2321
// DHT Sensor and Relay
const int DHTPin = 13;
const int RELAYPin = 4;   // gpio4

// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

// MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Global variables
const char* ssid         = "COREANHOUSE";
const char* password     = "COREAN000";
const char* mqtt_server  = "broker.mqttdashboard.com";
const int   mqtt_port    = 1883; 

String strMessage;
float  TempC, TempF, TempH;

void setup() {
  delay(1000);
  
  Serial.begin(9600); //115200);
  Serial.println("MSG\tSystem Initialize !");

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

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);   
}

void loop() {

   if (!client.connected()) {
    reconnect(); 
  }

  client.loop();

  getTemperature();
  Send_BasicInfo();     

  delay(10000);  // 10 sec
}


void Send_BasicInfo()
{
   String  payloadStr;

   payloadStr=String(TempC);
   client.publish("testTopicTemperature", payloadStr.c_str());
   payloadStr=String(TempH);
   client.publish("testTopicHumidity", payloadStr.c_str());
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
    if (client.connect( "testDevice01" ) ) {
      Serial.println("MSG\tMQTT connected");
      //      
      if(client.subscribe("testTopicRelay", 1) == 0 ) {
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
