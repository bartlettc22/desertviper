#include "Magnetometer.h"

void Magnetometer::init()
{
  while (!_compass.begin())
  {
    Serial.println("Could not find a valid HMC5883L sensor, check wiring!");
    delay(500);
  }

  // Set gain/range to 1.3 Ga; If we cannot get full readings, may need to increase this value; valid values are 0.88 Ga, 1.3 Ga, 1.9 Ga, 2.5 Ga, 4.0 Ga, 4.7 Ga, 5.6 Ga, 8.1 Ga
  _compass.setRange(HMC5883L_RANGE);

  // Set compass to read continuously
  _compass.setMeasurementMode(HMC5883L_MODE);

  // Set data rate
  _compass.setDataRate(HMC5883L_DATA_RATE);

  // Set number of samples averaged
  _compass.setSamples(HMC5883L_SAMPLES);

  // Set calibration offset. See HMC5883L_calibration.ino
  _compass.setOffset(0, 0);  

  initial_heading = getHeading(); 
}

void Magnetometer::run()
{
  
}

float Magnetometer::getHeading() {

  // Normalized data reading (no scale/vector strength, just direction)
  Vector norm = _compass.readNormalize();  

  // Calculate heading (convert x,y coordinates to radians)
  float heading = atan2(norm.YAxis, norm.XAxis); 
  
  // Adjust for Denver's magnetic declination (+ because it is East)
  heading += _declinationAngle;

  // Correct for heading < 0 deg and heading > 360 deg
  if(heading < 0)
     heading += 2*PI; // add 360 degrees if we're negative
  if(heading > 2*PI)
     heading -= 2*PI; // subtract 360 degrees if we're over 360 degrees

  // Convert from radians to degrees: 0 = North; 90 = East; 180 = South; 270 = West 
  float headingDegrees = heading * 180/M_PI; 

  heading_degrees = headingDegrees;

  return headingDegrees;
}

Magnetometer Compass = Magnetometer();

