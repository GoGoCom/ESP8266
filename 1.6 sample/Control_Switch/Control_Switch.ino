
void setup() {
  // put your setup code here, to run once:

  delay(2000); // for debug

  int index;

  Serial.begin(9600);

  index = 8;
  switch( index ) {
    case  0 :
    case  1 :
    case  2 :
    case  3 :
    case  4 :
    case  5 : 
       Serial.println(" 0< index <= 5 ");
       break;
    case  6 :
    case  7 :
    case  8 :
    case  9 :
    case  10 :
       Serial.println(" 5< index <= 10 ");
       break;
    default :
       Serial.println(" 10 < index ");
       break;    
  }  

}

void loop() {
  // put your main code here, to run repeatedly:

}
