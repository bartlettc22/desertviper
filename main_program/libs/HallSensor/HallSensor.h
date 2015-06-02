#ifndef HallSensor_h
#define HallSensor_h

#include "Arduino.h"

/* 
  Conversion of revolutions/ticks to real distance (in cm)
*/
#define TICK_TO_CM 9; // 533 cm = 60 ticks

class HSensor
{
  public:
    HSensor(){} ;
  	void init(unsigned char DATA, unsigned char INTRPT);
    void run();
  	float distanceTraveled();
  	int ticks;
  private:
  	//void static _interrupt_process();
    unsigned char _DATA;	
    unsigned char _INTRPT;
};

//extern HSensor HallSensor;

#endif