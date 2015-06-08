#ifndef Magnetometer_h
#define Magnetometer_h

// Libraries
#include <Arduino.h>
#include <Wire.h>
#include "HMC5883L.h"

#define HMC5883L_RANGE      HMC5883L_RANGE_1_3GA
#define HMC5883L_MODE       HMC5883L_CONTINOUS
#define HMC5883L_DATA_RATE  HMC5883L_DATARATE_30HZ
#define HMC5883L_SAMPLES    HMC5883L_SAMPLES_8

class Magnetometer
{
  public:  

    // CONSTRUCTORS
    Magnetometer() {};
    
    // PUBLIC METHODS
    void init();
    void run();
    float getHeading();

    // PUBLIC VARIABLES
    float initial_heading;
    float heading_degrees;
  private:
    /*
      Adjustment for magnetic declination
      Via instructions: http://www.meccanismocomplesso.org/en/arduino-magnetic-magnetic-magnetometer-hmc5883l/
      Declination angle in Denver is 8 degrees 29 arc minutes; This is 45.96 milliradians;
      declinationAngle is converted to in radians: 0.04596 rads
    */
    float const _declinationAngle = 45.96/1000;
    HMC5883L _compass;

};

extern Magnetometer Compass;

#endif

