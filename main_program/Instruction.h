#ifndef Instruction_h
#define Instruction_h

// Libraries
#include <Arduino.h>
#include "QueueArray.h"
#include "Event.h"
#include "Timer.h"

// Custom Libraries
#include "Controller.h"
#include "MotorController.h"
//#include "SteeringController.h"
#include "HallSensor.h"
#include "Magnetometer.h"
#include "RangeSensor.h"

class Instruction
{
  public:

  	// Type of instruction
  	int static const TYPE_DRIVE =	0;
  	int static const TYPE_STOP  =	2;
  	int static const TYPE_COAST =	3;
    int static const TYPE_TURN_LEFT = 4;
    int static const TYPE_TURN_RIGHT = 5;
    int static const TYPE_TURN_HEAD = 6;

    Instruction () {};
    Instruction(Ctrl &Controller, int type, int duration);
  	void init();
    void run();
  	bool IsDone(); 

  private:
    bool isDone = false;
    unsigned long startTime;
    Ctrl _Controller;
  	int _type;
    unsigned long _duration = 0;

};

#endif




