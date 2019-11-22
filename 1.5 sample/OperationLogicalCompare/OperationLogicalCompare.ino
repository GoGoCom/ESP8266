
void setup() {
  // put your setup code here, to run once:

  delay(2000); // for debug

  int EnglishScore = 90, MathScore = 80 ;

  Serial.begin(9600);

  Serial.println(  (EnglishScore >= 90) && (MathScore >= 90 ) ? "True" : "False");
  Serial.println(  (EnglishScore >= 90) || (MathScore >= 90 ) ? "True" : "False");
  Serial.println( !(EnglishScore <  90) ? "True" : "False");

}

void loop() {
  // put your main code here, to run repeatedly:

}
