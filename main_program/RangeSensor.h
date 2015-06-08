#ifndef RangeSensor_h
#define RangeSensor_h

#include <Arduino.h>

class RSensor
{
  public:
    RSensor(){} ;
  	void init(unsigned char TRIG, unsigned char _ECHO);
  	void run();
  	long getRange();
  private:
  	unsigned char _TRIG;
  	unsigned char _ECHO;

};

extern RSensor RangeSensor;

#endif

