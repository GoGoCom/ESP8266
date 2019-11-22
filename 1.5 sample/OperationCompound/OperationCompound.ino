

void setup() {
  // put your setup code here, to run once:

  delay(2000); // for debug

  int count ;


  Serial.begin(9600);
  
  count = 100;  
  Serial.print("count = ");
  Serial.println(count);
  count = count + 1;
  Serial.print("count = count + 1 is ");
  Serial.println(count);  
  count+=1;  
  Serial.print("count+=1 is  ");
  Serial.println(count++);    
  Serial.print("count++ is ");
  Serial.println(count);  
  Serial.print("++count is ");
  Serial.println(++count);   

}

void loop() {
  // put your main code here, to run repeatedly:

}
