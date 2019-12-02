
#include "ClassRobot.h"

void ROBOT::setInfo(int ht, int wt, int lg ) {
  height =  ht;
  weight =  wt;
  legs   =  lg;
  Serial.println("Set robot information");  
}

void ROBOT::getInfo( ) {
  Serial.print("Get Info - Height :");    
  Serial.print(height);    
  Serial.print(", Weight  : ");    
  Serial.print(weight);    
  Serial.print(", Legs : ");    
  Serial.print(legs);    
  Serial.print(", Type : ");    
  Serial.println(type);    
}

void ROBOT::Talking( ) {
   Serial.println("Base Robot is talking.");  
}

void ROBOT::Thinking( ) {
   Serial.println("Base Robot is thinking.");  
}

void ROBOT::Walking( ) {
   Serial.println("Base Robot is walking.");
}




