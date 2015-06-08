// Libraries
#include <Wire.h>
#include "HMC5883L.h"
#include "QueueArray.h"
#include "Event.h"
#include "Timer.h"
#include <math.h>

// Custom Libraries
#include "MotorController.h"
#include "SteeringController.h"
#include "HallSensor.h"
#include "Magnetometer.h"
#include "RangeSensor.h"
#include "Controller.h"
#include "Instruction.h"
#include "Program.h"

Prog Program = Prog();

void setup() {
  Program.init();
}

void loop() {
	Program.run();
}


