
void setup() {
  // put your setup code here, to run once:

  delay(2000); // for debug

  char *StudentName[10] = {"gildong","cheolsu","soonhee","yong" };

  Serial.begin(9600);

  int i = 0;

  while( i < 4 )
  {
    Serial.println( StudentName[i++] );    
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
