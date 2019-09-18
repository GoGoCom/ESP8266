
#include <EEPROM.h>

void setup() {
 int addr , val;
  
 Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  EEPROM.begin(512);

 for(addr = 0; addr<512; addr ++) {
   val = addr * 3;

   EEPROM.write(addr, val);
   delay(100);
 }
 EEPROM.commit();
 Serial.println("Finished!");
  
}

void loop() {


}
