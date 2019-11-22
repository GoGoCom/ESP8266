
void setup() {
  // put your setup code here, to run once:

  delay(2000); // for debug

  char *StudentName[10] = {"gildong","cheolsu","soonhee","yong" };

  Serial.begin(9600);
  
  for(int i=0; i<4; i++)
  {
    Serial.println( StudentName[i] );
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
