

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

#define  DHTPIN  2           // What digital pin we're connected to

#define  RELAY_PIN  5     // gpio5
#define  RELAY_ON   HIGH
#define  RELAY_OFF  LOW

#define  LED_ON     LOW
#define  LED_OFF    HIGH

#define BLYNK_RED       "#D3435C"
#define BLYNK_GREEN     "#23C48E"
#define BLYNK_DARK_BLUE "#5F7CD8"
#define BLYNK_BLUE      "#04C0F8"
#define BLYNK_WHITE     "#FFFFFF"
#define BLYNK_YELLOW    "#ED9D00"

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11     // DHT 11
#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
//#define DHTTYPE DHT21   // DHT 21, AM2301

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;
WidgetLED  led(V2);

char* ssid = "COREANPLACE2";
char* pass = "COREAN000";
char* auth = "Fn3gSkp26O7UZ9_csjdUTzz3F2wH3a_B";


// BLYNK Subroutine
BLYNK_CONNECTED() {
    // Request Blynk server to re-send latest values for all pins
    Blynk.syncAll();
}

BLYNK_WRITE(V3) // Button
{
  int pinValue = param.asInt();

  if( pinValue == 1 ) {
     led.on();
     digitalWrite(RELAY_PIN, RELAY_ON);
  } else {
     led.off();
     digitalWrite(RELAY_PIN, RELAY_OFF);
  }
}
//
void sendSensor(void)
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V0, h);
  Blynk.virtualWrite(V1, t);
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);   
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, RELAY_OFF); 
    
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);

  dht.begin();
  led.setColor(BLYNK_BLUE); 
  
  // Setup a function to be called every second
  timer.setInterval(5000L, sendSensor);
}

void loop()
{
  Blynk.run();
  timer.run();
}
