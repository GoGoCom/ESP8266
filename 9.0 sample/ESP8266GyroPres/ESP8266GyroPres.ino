/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 by ThingPulse, Daniel Eichhorn
 * Copyright (c) 2018 by Fabrice Weinberg
 *
 */
 
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>               
#include <L3G.h>
#include <LSM303.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>
//
#define OLED_RESET 0  // GPIO0
//
L3G gyro;
LSM303 compass;
Adafruit_BMP085  bmp;
Adafruit_SSD1306 display(OLED_RESET);
BlynkTimer timer;
WidgetLCD lcd(V1);
//
char* ssid = "COREANPLACE2";
char* pass = "COREAN000";
char* auth = "N6RrEZR1-PbD6UanXgx7RB0ML0dUtqKH";

void setup() {
  
  Serial.begin(9600);
 
  Blynk.begin(auth, ssid, pass);  
  // Setup a function to be called every second
  timer.setInterval(1000L, sendSensor);  
  //
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }    
  //
  if (!gyro.init())
  {
    Serial.println("Failed to autodetect gyro type!");
    while (1);
  }
  gyro.enableDefault();
  //
  compass.init();
  compass.enableDefault();  
  compass.m_min = (LSM303::vector<int16_t>){-32767, -32767, -32767};
  compass.m_max = (LSM303::vector<int16_t>){+32767, +32767, +32767};  
  //
  lcd.clear(); //Use it to clear the LCD Widget
  //
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  display.clearDisplay();   
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);

}

void sendSensor(void)
{  
    char buf[100];
    
    float Temp = bmp.readTemperature();
    long  Pres = bmp.readPressure();
    long  Alti = bmp.readAltitude();
    float SeaPres = bmp.readSealevelPressure();
    float SeaAlti = bmp.readAltitude(101500);

    //
    memset(buf, 0, sizeof(buf) );
    sprintf(buf, "P:%6d,T:%3.1f",Pres, Temp );
    lcd.print(0, 0, buf); 
    sprintf(buf, "A:%6d,S:%3.1f",Alti, SeaAlti );
    lcd.print(0, 1, buf);   

    //
    compass.read();    

    float Heading = compass.heading();
    
    Serial.println(Heading);

    //
    int gx, gy, gz;

    gyro.read();

    gx = gyro.g.x;
    gy = gyro.g.y;
    gz = gyro.g.z;

    //
    display.setCursor(0,0);
    display.println((String) "H:" + (String) (int) Heading);    
    display.setCursor(40,0);
    display.println((String) "T:" + (String) (int) Temp);    
    display.setCursor(0,8);
    display.println((String) "P:" + (String) Pres);    //SeaPres); 
    display.setCursor(0,16);
    display.println((String) "A:" + (String) SeaAlti);    // Alti);    
  
    display.setCursor(0,24);
    display.println((String) "Gx:" + (String) gx);    
    display.setCursor(0,32);
    display.println((String) "Gy:" + (String) gy);    
    display.setCursor(0,40);
    display.println((String) "Gz:" + (String) gz);    
  
    display.display();  

    //
    Blynk.virtualWrite(V5, Heading);   

    int x, y;

    x=cos((float) Heading/180*PI) * 512;
    y=sin((float) Heading/180*PI) * 512;
    
    x=map(x, -512,512, 0, 1023);
    y=map(y, -512,512, 0, 1023);
    
    Blynk.virtualWrite(V6, x);   
    Blynk.virtualWrite(V7, y);   
    Blynk.virtualWrite(V8, gz);   

}
/*
  snprintf(report, sizeof(report), "A: %6d %6d %6d    M: %6d %6d %6d",
    compass.a.x, compass.a.y, compass.a.z,
    compass.m.x, compass.m.y, compass.m.z);
*/
void loop() {

  Blynk.run();
  timer.run(); 
  
}
