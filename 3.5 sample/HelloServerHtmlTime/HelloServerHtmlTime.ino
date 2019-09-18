#include <TimeLib.h>       
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define TimeGab  9

#ifndef STASSID
#define STASSID "COREANHOUSE"
#define STAPSK  "COREAN000"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

const int led = 13;

void handleRoot() {
  time_t t;
  String strDateTime, strHtml; 
   
  t = now();

  strDateTime = String(hour(t)) + ":" + String(minute(t)) + ":" + String(second(t)) + " " + String(dayShortStr(weekday(t))) + " " + String(day(t)) + "/" + String(monthShortStr(month(t))) + "/" + String(year(t)) + " UTC" + String(TimeGab) + "h" ;

  strHtml = "<!DOCTYPE html><html><head><title>Page Title</title></head><body><h1>This is a Heading</h1><p>" + strDateTime + "</p></body></html>";
  
  Serial.println(strHtml);     

  digitalWrite(led, 1);
  server.send(200, "text/html", strHtml);
  digitalWrite(led, 0);  

}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

// Get time from Internaet  
  setTimeFromUTCToLocal( TimeGab );  
  
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
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
