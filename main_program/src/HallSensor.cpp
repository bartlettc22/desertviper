#include "HallSensor.h"

void HSensor_interrupt() {
  HallSensor.ticks = HallSensor.ticks + 1;
}

/*
  Set up all inputs/outputs
*/
void HSensor::init(unsigned char DATA, unsigned char INTRPT)
{
  _DATA = DATA;
  _INTRPT = INTRPT;

  // Set up interrupts
  attachInterrupt(_INTRPT, HSensor_interrupt, CHANGE);
}

void HSensor::run()
{
	
}

float HSensor::distanceTraveled() 
{
    // Get distance travelled (in cm)
  float distance_in_cm = ticks * TICK_TO_CM;
  return distance_in_cm;
}

HSensor HallSensor = HSensor();

