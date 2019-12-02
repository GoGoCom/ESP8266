
#include <Arduino.h>

#ifndef classROBOT
#define classROBOT

class ROBOT {
 private:
   int height;
   int weight;
   int legs;

 public:
   int type = 0;

 public:

   void setInfo( int, int, int );
   void getInfo( );

   void Talking( );
   void Thinking( );   

   virtual void Walking( );
};

#endif