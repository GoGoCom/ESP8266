
#include "ClassRobot.h"

#ifndef classDogROBOT
#define classDogROBOT

class dogROBOT : public ROBOT {

 public:

   dogROBOT(int style )  {      
       type = style;
   }   

   void Running( );

   void Thinking( ); 
   void Walking( );

};

#endif