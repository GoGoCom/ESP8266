
#include "ClassRobot.h"

#ifndef classCleanerROBOT
#define classCleanerROBOT

class cleanerROBOT : public ROBOT {

 public:

   cleanerROBOT(int style )  {      
       type = style;
   }   

   void Running( );

   void Thinking( ); 
   void Walking( );
};

#endif