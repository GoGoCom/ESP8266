

void setup() {
  // put your setup code here, to run once:

  delay(2000); // for debug

  int EnglishScore = 90, MathScore = 80 ;
  int TotalScore;
  float AverageScore;

  Serial.begin(9600);
  
  TotalScore =  EnglishScore + MathScore ;   
  AverageScore = TotalScore / 2 ;

  Serial.print("EnglishScore = ");
  Serial.println(EnglishScore);
  Serial.print("MathScore = ");
  Serial.println(MathScore);
  Serial.print("TotalScore = ");
  Serial.println(TotalScore);
  Serial.print("AverageScore = ");
  Serial.println(AverageScore);    

}

void loop() {
  // put your main code here, to run repeatedly:

}
