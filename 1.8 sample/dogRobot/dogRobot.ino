#include <ClassDogRobot.h>

void setup() {
  dogROBOT myRobot(1); 
  
  // put your setup code here, to run once:
  delay(2000);
  
  Serial.begin(9600);

  Serial.println("Robot ");
  myRobot.setInfo(170, 50, 4); 
  myRobot.getInfo( );

  myRobot.Talking( );
  myRobot.Thinking( );  
  myRobot.Walking( );
    
  myRobot.Running( );
    
  Serial.print("Robot type : "); 
  Serial.println(myRobot.type); 
  
  Serial.println("End"); 

}

void loop() {
  // put your main code here, to run repeatedly:

}
