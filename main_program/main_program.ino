// Libraries
#include <Wire.h>
#include <HMC5883L.h>

// Custom Libraries
#include <MotorController.h>
#include <SteeringController.h>
#include <HallSensor.h>
#include <Magnetometer.h>
#include <RangeSensor.h>
#include <Controller.h>
#include <Program.h>

Ctrl Controller;
Prog Program;

void setup() {
  Controller.init();
  Program.init(Controller);
}

void loop() {
	Program.run();
}