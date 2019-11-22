
void setup() {
  // put your setup code here, to run once:

  delay(2000); // for debug

  int EnglishScore = 90, MathScore = 80 ;

  byte value;
  
  Serial.begin(9600);
  
  value = 2;  
  Serial.print("value = ");
  Serial.println(value, BIN);
  value = value << 1;
  Serial.print("value = value << 1 is ");
  Serial.println(value, BIN);  
  value<<=1;  
  Serial.print("value<<=1 is  ");
  Serial.println(value, BIN);      
  Serial.print("value AND 0001 is ");
  Serial.println(value & 1, BIN);
  Serial.print("value OR  0001 is ");
  Serial.println(value | 1, BIN);
  Serial.print("value XOR 1100 is  ");
  Serial.println(value ^ 12, BIN);
  Serial.print("value NOT is  ");
  Serial.println((byte) ~value , BIN);    

}

void loop() {
  // put your main code here, to run repeatedly:

}
