
void setup() {
  // put your setup code here, to run once:

  delay(2000); // for debug

  float temperature;

  Serial.begin(9600);
  
   // tytpe 1
   temperature = 70.0 ;   
   if (temperature >= 70) {        // temperature >= 70 
       Serial.println("Danger! Shut down the system. ");
   } else if (temperature >= 60) { // 60 <= temperature < 70 
       Serial.println("Warning! User attention required."); 
   } else {                        // temperature < 60 
       Serial.println("Safe! Continue usual tasks.");
   }   
   // tytpe 2
   temperature = 65.0 ;   
   if (temperature >= 70)         // temperature >= 70 
       Serial.println("Danger! Shut down the system. ");      
   if ((temperature >= 60) && (temperature < 70) ) 
       Serial.println("Warning! User attention required.");    
   if (temperature < 60)         // temperature < 60 
       Serial.println("Safe! Continue usual tasks.");
   
}

void loop() {
  // put your main code here, to run repeatedly:

}
