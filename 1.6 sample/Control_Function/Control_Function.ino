

int Average(int scoreA, int scoreB)
{
  return (scoreA + scoreB ) / 2 ;
}

void setup() {
  // put your setup code here, to run once:

  delay(2000); // for debug

  Serial.begin(9600);
  
  Serial.print("AverageScore = ");
  Serial.println( Average(90, 80) );    
  Serial.print("AverageScore = ");
  Serial.println( Average(70, 60) );    
  Serial.print("AverageScore = ");
  Serial.println( Average(100, 80) );    

}

void loop() {
  // put your main code here, to run repeatedly:

}
