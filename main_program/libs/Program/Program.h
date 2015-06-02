#ifndef Program_h
#define Program_h

// Libraries
#include <Arduino.h>

// Custom Libraries
#include <Controller.h>
#include <MotorController.h>
#include <SteeringController.h>
#include <HallSensor.h>
#include <Magnetometer.h>
#include <RangeSensor.h>

class Prog
{
  public:
    Prog(){} ;
  	void init(Ctrl Controller); // Set up all inputs/outputs
  	void run();  //Runs the main program
  private:
  	Ctrl _Controller;
};

#endif