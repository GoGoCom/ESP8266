#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <fs.h>

#ifndef STASSID
#define STASSID "COREANHOUSE"
#define STAPSK  "COREAN000"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

String strStyle = " \
<style> \
table { \
  border-collapse: collapse; \
  width: 100%; \
} \
td, th { \
  border: 1px solid black; \
  <!-border: 1px solid #dddddd;-> \
  text-align: left; \
  padding: 8px; \
} \
tr:nth-child(even) { \
  background-color: #dddddd; \
} \
</style>";


void handleRoot() {
String HTMLPage;

HTMLPage  = "<!DOCTYPE html>";
HTMLPage += "<html>";
HTMLPage += "<head>";
HTMLPage += "<link rel='stylesheet' href='/StyleTest.css'>";
HTMLPage += "</head>";
HTMLPage += "<body>";
HTMLPage += "<h2>HTML Table & Input type</h2>";
HTMLPage += "<form action='exec' method='POST'>";
HTMLPage += "<table style='width:100%'>";
HTMLPage += "  <tr>";
HTMLPage += "    <th>Text Input</th>";
HTMLPage += "    <th>Radio Buttons</th>";
HTMLPage += "    <th>Checkboxes</th>";
HTMLPage += "  </tr>";
HTMLPage += "  <tr>";
HTMLPage += "    <td>Ernst Handel</td>";
HTMLPage += "    <td>Roland Mendel</td>";
HTMLPage += "    <td>Austria</td>";
HTMLPage += "  </tr>";
HTMLPage += "  <tr>";
HTMLPage += "    <td>Island Trading</td>";
HTMLPage += "    <td>Helen Bennett</td>";
HTMLPage += "    <td>UK</td>";
HTMLPage += "  </tr>";
HTMLPage += "  <tr>";
HTMLPage += "    <td>";
HTMLPage += "      First name:<br>";
HTMLPage += "      <input type='text' name='firstname' value='Mickey'><br>";
HTMLPage += "      Last name:<br>";
HTMLPage += "      <input type='text' name='lastname' value='Mouse'><br><br>";
HTMLPage += "    </td>";
HTMLPage += "    <td>";
HTMLPage += "      <input type='radio' name='gender' value='male' checked> Male<br>";
HTMLPage += "      <input type='radio' name='gender' value='female'> Female<br>";
HTMLPage += "      <input type='radio' name='gender' value='other'> Other  ";
HTMLPage += "    </td>";
HTMLPage += "    <td>";
HTMLPage += "      <input type='checkbox' name='vehicle1' value='Bike'>I have a bike";
HTMLPage += "      <br>";
HTMLPage += "      <input type='checkbox' name='vehicle2' value='Car'>I have a car ";
HTMLPage += "    </td>";
HTMLPage += "  </tr>";
HTMLPage += "  <tr width='100%'>";
HTMLPage += "    <td colspan='3' style='text-align: center'>";
HTMLPage += "    <input type='submit' value='Submit'>";
HTMLPage += "    </td>";
HTMLPage += "  </tr>";
HTMLPage += "</table>";
HTMLPage += "</form>";
HTMLPage += "</body>";
HTMLPage += "</html>";

server.send(200, "text/html", HTMLPage);
  
}

void handleSubmit() {

String HTMLPage;

HTMLPage  = "<!DOCTYPE html>";
HTMLPage += "<html>";
HTMLPage += "<head>";
HTMLPage += "<link rel='stylesheet' href='/StyleTest.css'>";
HTMLPage += "</head>";
HTMLPage += "<body>";
HTMLPage += "<h2>HTML Table & Input type</h2>";
HTMLPage += "<form action='tableNinputtypes2.html' method='POST'>";
HTMLPage += "<table style='width:100%'>";
HTMLPage += "  <tr>";
HTMLPage += "    <th>Text Input</th>";
HTMLPage += "    <th>Radio Buttons</th>";
HTMLPage += "    <th>Checkboxes</th>";
HTMLPage += "  </tr>";
HTMLPage += "  <tr>";
HTMLPage += "    <td>";
HTMLPage += "      First name: " + server.arg("firstname");
HTMLPage += "       <br>";
HTMLPage += "       <br>";
HTMLPage += "      Last name: " + server.arg("lastname");
HTMLPage += "       <br>";
HTMLPage += "       <br>";
HTMLPage += "    </td>";
HTMLPage += "    <td>";
HTMLPage += "      Gender: " + server.arg("gender");
HTMLPage += "      <br>";
HTMLPage += "    </td>";
HTMLPage += "    <td>";
HTMLPage += "      Vehicle : " + server.arg("vehicle1") + server.arg("vehicle2") ;
HTMLPage += "      <br>";
HTMLPage += "    </td>";
HTMLPage += "  </tr>";
HTMLPage += "  <tr width='100%'>";
HTMLPage += "    <td colspan='3' style='text-align: center'>";
HTMLPage += "    <input Type='button' value='Back' onClick='history.go(-1);return true;'>";
HTMLPage += "    </td>";
HTMLPage += "  </tr>";
HTMLPage += "</table>";
HTMLPage += "</form>"; 
HTMLPage += "</body>";
HTMLPage += "</html>";

server.send(200, "text/html", HTMLPage);

}

bool MakeStyleFile()
{
  File f;
    
  if( !SPIFFS.begin())
  {
    Serial.println("MSG\tSPIFFS mount failed!");
    return false;
  } 
  // open file for writing
  f = SPIFFS.open("/StyleTest.css", "w");
  if (!f) {
      Serial.println("MSG\tfile writing failed!");
      return false;
  }
  Serial.println("MSG\tWriting style file");  
  f.println(strStyle);
  f.close();
  // set url -> file system
  server.serveStatic("/StyleTest.css",  SPIFFS, "/StyleTest.css");  

  return true;
}


void handleNotFound() {

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

}

void setup(void) {

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

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  if ( MakeStyleFile() == false ) {
    Serial.println("MSG\tStyle file, fail");  
  }
  else {  
    Serial.println("MSG\tStyle file, all done");  
  }
  
  server.on("/", handleRoot);
  server.on("/exec", HTTP_POST, handleSubmit);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
