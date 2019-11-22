

void Average(int scoreA, int scoreB)
{
  Serial.print("AverageScore = ");
  Serial.println( (scoreA + scoreB ) / 2 );    
}

void setup() {
  // put your setup code here, to run once:

  delay(2000); // for debug

  Serial.begin(9600);
  
  Average( 90, 80);    
  Average( 70, 60);    
  Average(100, 80);    

}

void loop() {
  // put your main code here, to run repeatedly:

}
