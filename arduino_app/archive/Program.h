#ifndef Program_h
#define Program_h

// Libraries
#include <Arduino.h>
#include "QueueArray.h"
#include "Event.h"
#include "Timer.h"
#include <math.h>

// Custom Libraries
#include "Controller.h"
#include "MotorController.h"
//#include "SteeringController.h"
#include "HallSensor.h"
#include "Magnetometer.h"
#include "RangeSensor.h"
#include "Instruction.h"

class Prog
{
  public:
    Prog() {};
  	void init(); 
  	void run();  //Runs the main program
  private:
  	Ctrl _Controller;
  	QueueArray<Instruction> _stack; // The instruciton stack
  	int _i; // The instruciton # that we're on
    bool isDone = false;
  	Instruction _currentInstruction;
};

#endif






