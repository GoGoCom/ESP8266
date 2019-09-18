/*
   EEPROM Read

   Reads the value of each byte of the EEPROM and prints it
   to the computer.
   This example code is in the public domain.
*/

#include <EEPROM.h>

void setup() {
  delay(2000);
  // initialize serial and wait for port to open:
  Serial.begin(9600);
  // initialize EEPROM :
  EEPROM.begin(100);
}

void loop() {
  byte value;
  int addr;

  Serial.println("Writing started!");  
  for(addr = 0; addr<10; addr++)
  {
   int val = addr * 3;
  
    // write the value to the appropriate byte of the EEPROM.
    EEPROM.write(addr, val);
  }
  EEPROM.commit();
  Serial.println("Writing finished!");
  delay(1000);
  Serial.println("Reading finished!");  
  for(addr = 0; addr<10; addr++)
  {
    // read a byte from the current address of the EEPROM
    value = EEPROM.read(addr);
  
    Serial.print(addr);
    Serial.print("\t");
    Serial.print(value, DEC);
    Serial.println();
  }
  Serial.println("Reading finished!");  
  delay(1000);
  
}
