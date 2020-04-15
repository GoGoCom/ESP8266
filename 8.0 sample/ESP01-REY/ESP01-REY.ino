
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>


#define  RELAY_PIN  0     // gpio0
#define  RELAY_ON   LOW
#define  RELAY_OFF  HIGH

#define  LED_ON     LOW
#define  LED_OFF    HIGH

#define BLYNK_RED       "#D3435C"
#define BLYNK_GREEN     "#23C48E"
#define BLYNK_DARK_BLUE "#5F7CD8"
#define BLYNK_BLUE      "#04C0F8"
#define BLYNK_WHITE     "#FFFFFF"
#define BLYNK_YELLOW    "#ED9D00"

char* ssid = "COREANPLACE2";
char* pass = "COREAN000";
char* auth = "5X0JyZmYYAZH1MyseXJQtdGAc2wc6Iak";

WidgetLED  led(V1);

// BLYNK Subroutine
BLYNK_CONNECTED() {
    // Request Blynk server to re-send latest values for all pins
    Blynk.syncAll();
}

BLYNK_WRITE(V0) // Button
{
  int pinValue = param.asInt();

  if( pinValue == 1 ) {
     led.on();
     digitalWrite(LED_BUILTIN, LED_ON);
     digitalWrite(RELAY_PIN, RELAY_ON);
  } else {
     led.off();
     digitalWrite(LED_BUILTIN, LED_OFF);
     digitalWrite(RELAY_PIN, RELAY_OFF);
  }
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);   
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, RELAY_OFF); 
  
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
  
  led.setColor(BLYNK_BLUE); 
}

void loop()
{
  Blynk.run();
}
