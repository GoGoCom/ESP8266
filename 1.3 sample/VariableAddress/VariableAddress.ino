
long   weight = 10000;


//char   tname[5] = "nest";
char   tname[5] = { 'n','e','s','t' };
 static char   day   = 'a';
 
void test(int ar1, char ar2 )
{

  day = 'b';
  
Serial.print("char day ");
Serial.println( (unsigned long ) &day, HEX);
Serial.print("int   ar1 ");
Serial.println( (unsigned long ) &ar1, HEX);
Serial.print("char  ar2 ");
Serial.println( (unsigned long ) &ar2, HEX);

}

void setup() {
const double  pi = 3.14;
unsigned long long tt;
  
int    score  = 80;

delay(2000);

Serial.begin(9600);


Serial.println( sizeof(tt) );


test( 1, 'b' );


Serial.println((unsigned long ) &score, HEX);
Serial.println( sizeof(score));
Serial.print("long weight ");
Serial.println((unsigned long ) &weight, HEX);
Serial.println( sizeof(weight));
Serial.print("float pi ");
Serial.println((unsigned long ) &pi, HEX);
Serial.println( sizeof(pi));
Serial.print("tname ");
Serial.println((unsigned long ) &tname, HEX);
Serial.println( sizeof(tname));

}

void loop() {
  // put your main code here, to run repeatedly:

}
