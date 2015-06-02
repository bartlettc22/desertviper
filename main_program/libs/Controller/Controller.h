#ifndef Controller_h
#define Controller_h

// Libraries
#include <Arduino.h>

// Custom Libraries
#include <MotorController.h>
#include <SteeringController.h>
#include <HallSensor.h>
#include <Magnetometer.h>
#include <RangeSensor.h>

// Motor Controller PINs
#define PIN_MOTOR_INA1		11 
#define PIN_MOTOR_INB1		12
#define PIN_MOTOR_EN1DIAG1	0
#define PIN_MOTOR_PWM1 		6
#define PIN_MOTOR_CS1 		5
#define PIN_MOTOR_INA2 		0
#define PIN_MOTOR_INB2		0 
#define PIN_MOTOR_EN2DIAG2 	0
#define PIN_MOTOR_PWM2 		0
#define PIN_MOTOR_CS2 		0

// Hall Effect (Distance) Sensor PINS
#define PIN_HALL     2    // PIN #
#define INRPT_HALL   0    // INTERRUPT #

// Steering Controller Motor/Potentiometer
#define PIN_STEER_PWM  5     // Steering Motor Speed Control (PWM)
#define PIN_STEER_INA1   9     // Steering Motor Direction/Input 1
#define PIN_STEER_INB1   8    // Steering Motor Direction/Input 2
#define PIN_STEER_EN1DIAG1   10    // Steering Motor Standby/Sleep
#define PIN_STEER_POT    0     // Steering Motor Position Potentiometer

// Range Sensor
#define PIN_RANGE_TRIG   3
#define PIN_RANGE_ECHO   4

class Ctrl
{
  public:
    Ctrl(){} ;
  	void init(); // Initialize controllers/sensors
  	void run();  // Run controller/sensor step functions

  	// Peripherals
  	/*HSensor HallSensor;*/
  	Magnetometer Compass;
  	/*RSensor RangeSensor;
    SController SteeringController;
  	MController MotorController;*/
};

#endif