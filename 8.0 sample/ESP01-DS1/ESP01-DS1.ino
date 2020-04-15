
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

char* ssid = "COREANPLACE2";
char* pass = "COREAN000";
char* auth = "N6RrEZR1-PbD6UanXgx7RB0ML0dUtqKH";

OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
BlynkTimer timer;

//
void sendSensor(void)
{  
  sensors.requestTemperatures();

  float t = sensors.getTempCByIndex(0);

 // Check if reading was successful
  if(t == DEVICE_DISCONNECTED_C) 
  {
    Serial.println("Error: Could not read temperature data");
    return;
  }

  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V1, t); 
  
}

void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
  
  sensors.begin();
 
  // Setup a function to be called every second
  timer.setInterval(5000L, sendSensor);  
}

void loop()
{
  Blynk.run();
  timer.run(); 
}
