#include "RangeSensor.h"

void RSensor::init(unsigned char TRIG, unsigned char ECHO)
{
  _TRIG = TRIG;
  _ECHO = ECHO;

  pinMode(_TRIG, OUTPUT);
  pinMode(_ECHO, INPUT);  
}

void RSensor::run() 
{

}

long RSensor::getRange() {
  
  //long duration, distance;
  //digitalWrite(PIN_RANGE_TRIG, LOW);  // Added this line
  //delayMicroseconds(2); // Added this line
  digitalWrite(_TRIG, HIGH);
  delayMicroseconds(2000); // Added this line
  digitalWrite(_TRIG, LOW);
  _duration = pulseIn(_ECHO, HIGH);
  _distance = (_duration/2) / 29.1;
}

RSensor RangeSensor = RSensor();






