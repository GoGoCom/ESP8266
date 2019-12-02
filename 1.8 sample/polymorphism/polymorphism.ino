#include <ClassDogRobot.h>
#include <ClassCleanerRobot.h>

void WalkingTest(ROBOT  *baseRobot)
{
   baseRobot->Thinking( );
   baseRobot->Walking( );
}

void setup() {

  dogROBOT     myDogRobot(1); 
  cleanerROBOT myCleanerRobot(2); 
  
  // put your setup code here, to run once:
  delay(2000);
  
  Serial.begin(9600);
  Serial.println("Polymorphism ");

  myDogRobot.Running( );
  myCleanerRobot.Running( );
  
  WalkingTest(&myDogRobot);
  WalkingTest(&myCleanerRobot);
  
  Serial.println("End"); 

}

void loop() {
  // put your main code here, to run repeatedly:

}
