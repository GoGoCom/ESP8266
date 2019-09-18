/*
   EEPROM Read

   Reads the value of each byte of the EEPROM and prints it
   to the computer.
   This example code is in the public domain.
*/

#include <EEPROM.h>

struct WIFI {
  char  SID[21];
  char  PWD[21];
  char  PIN[21];
  char  IPA[21];
  uint8_t   HRI;
  uint8_t   MNI;
  boolean   SFS;
  char  REV[39];
};

struct HOUR {
  uint8_t  g_hour[24];
};

struct MODE {
  char     LocalName[21];
  char     HostName[21];
};

struct {
  struct WIFI W;
  struct HOUR H;
  struct MODE V;  
} Info;


void ReadAllFromEEPROM()
{
 int i;
byte value;

  // read the value to the appropriate byte of the EEPROM.
  for( i=0; i<sizeof(Info); i ++) {
    value = EEPROM.read(i);
    *(Info.W.SID + i) =  value;
    //delay(100);
  }
  
}

void WriteAllToEEPROM()
{
   int  i;
  byte value;

  // write the value to the appropriate byte of the EEPROM.
  for( i=0; i<sizeof(Info); i ++) {
    value = *(Info.W.SID + i);
    EEPROM.write(i, value);
    delay(100);
  }  

  EEPROM.commit();
}

void WriteToEEPROM(long taddr, int len)
{
  int addr, i;
  byte value;

  addr = taddr - long( &Info );  
  for( i=0; i<len; i ++) {
    value = *(Info.W.SID + addr + i);    
    EEPROM.write(addr + i, value);
    delay(100);
  }

  EEPROM.commit();
}

void setup() {
  delay(2000);
  // initialize serial and wait for port to open:
  Serial.begin(9600);
  // initialize EEPROM :
  EEPROM.begin( sizeof(Info) );

  ReadAllFromEEPROM();

  memset(Info.W.SID, 0, sizeof(Info) );

  Info.W.HRI = 12;
  Info.W.MNI = 0;
  strcpy(Info.W.PIN, "0000");
  strcpy(Info.V.LocalName, "heater");
  strcpy(Info.V.HostName,  "wfshost"); 
  
  WriteAllToEEPROM();

  WriteToEEPROM(long(Info.W.SID), sizeof(Info.W));
  
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
