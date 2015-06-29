#ifndef Controller_h
#define Controller_h

// Libraries
#include <Arduino.h>
#include "I2Cdev.h"
#include "HMC5883L.h"
#include "MPU6050.h"
#include <math.h>

// Custom Libraries
#include "HallSensor.h"

// Motor Controller PINs
// Motor 1 = Rear Wheels
// Motor 2 = Front Wheels
#define PIN_MOTOR_INA1		  7 
#define PIN_MOTOR_INB1		  8
#define PIN_MOTOR_EN1DIAG1	A0
#define PIN_MOTOR_PWM1      5
#define PIN_MOTOR_CS1 		  2
#define PIN_MOTOR_INA2 		  4
#define PIN_MOTOR_INB2		  9 
#define PIN_MOTOR_EN2DIAG2 	A1
#define PIN_MOTOR_PWM2 		  6
#define PIN_MOTOR_CS2 		  3

// Hall Effect (Tick) Sensor PINS
#define PIN_HALL     2    // PIN #
#define INRPT_HALL   0    // INTERRUPT # (PIN2 = Interrupt 0, PIN 3 = Interrupt 1)

// RF Receiver
#define PIN_RF        3 // RF Receiver VT - Any button pressed
#define INRPT_RF      1 
#define PIN_RF_A      19 // RF Receiver D2
#define INRPT_RF_A    4 
#define PIN_RF_B      18 // RF Receiver D0
#define INRPT_RF_B    5 
//#define INRPT_RF_C   0 // RF Receiver D3
//#define INRPT_RF_D   0 // RF Receiver D1

// Steering Controller Motor/Potentiometer
#define PIN_STEER_PWM         10  // Steering Motor Speed Control (PWM)
#define PIN_STEER_INA1        22  // Steering Motor Direction/Input 1
#define PIN_STEER_INB1        23  // Steering Motor Direction/Input 2
//#define PIN_STEER_EN1DIAG1    1   // Steering Motor Standby/Sleep
#define PIN_STEER_POT          15   // Steering Motor Position Potentiometer

// Range Sensor
#define PIN_RANGE_TRIG   46
#define PIN_RANGE_ECHO   47

// Steering Motor Globals
#define TURN_DIRECTION_BRAKE    0
#define TURN_DIRECTION_LEFT     1
#define TURN_DIRECTION_RIGHT    2

// Drive Motor Globals
#define DRIVE_MOTOR_FRONT       0
#define DRIVE_MOTOR_REAR        1
#define DRIVE_MOTOR_BOTH        2

#define DRIVE_DIRECTION_FORWARD     0
#define DRIVE_DIRECTION_REVERSE     1
#define DRIVE_DIRECTION_COAST       2
#define DRIVE_DIRECTION_BRAKE_LOW   3
#define DRIVE_DIRECTION_BRAKE_HIGH  4
//#define DRIVE_DIRECTION_DISABLE     5 // Don't use this as it isn't implemented

class Ctrl
{
  public:
  
    /*
    * Empty Constructor
    */
    Ctrl(){} ;

    /*
    * Initialize I/Os, shift register, and set configuration settings
    */    
  	void init(); // Initialize controllers/sensors
  	void run();  // Run controller/sensor step functions

    /*
    * Control primary drive motors
    *
    * @param motor      Specify which motor to control (DRIVE_MOTOR_FRONT, DRIVE_MOTOR_REAR, DRIVE_MOTOR_BOTH)
    * @param direction  Specify which direction to drive (DRIVE_MOTOR_FORWARD, DRIVE_MOTOR_REVERSE, DRIVE_DIRECTION_BRAKE_LOW, DRIVE_DIRECTION_BRAKE_HIGH)
    * @param speed      Specify speed to drive (0 to 1, for example 0.5 drives at half speed)
    */
    void Drive(unsigned char motor, unsigned char direction, double speed);
    
    /*
    * Control steering motors
    *
    * @param direction  Specify which direction to drive (DRIVE_MOTOR_FORWARD, DRIVE_MOTOR_REVERSE, DRIVE_DIRECTION_BRAKE_LOW, DRIVE_DIRECTION_BRAKE_HIGH)
    * @param speed      Specify speed to drive (0 to 1, for example 0.5 drives at half speed)
    * @param amount     Specify how hard to turnt the wheels (0 to 1, for example 1 is full turn, 0.5 is half turn)
    */
    void Turn(unsigned char direction, float speed, float amount);

    void CalibrateSteering();

    void checkSteering();
    void checkDrive();
    void getRange();
    void getHeading();

    int _potValue;
    int _frontCurrent;
    bool _frontFault;
    int _rearCurrent;
    bool _rearFault;

    // Range sensor readings
    unsigned int _rangeDuration;
    double _rangeDistance;

    /*
      Potentiometer Edge Values (Right - Left)
      Base: 0 - 1023
      Casing: ~100 - ~910
      Wheels: ~352 - ~630
      Ideal: 380 - 610
    */
    int _POT_LEFT_MAX = 0; //688
    int _POT_RIGHT_MAX = 0; // 431    
    int _potCenter = 0; // 560
    int _turnAmountPot = 0; // Potentiometer value for 1% turn

    // Motors
    double _speedGoalFront;
    double _speedGoalRear;

    // RF Receiver
    volatile bool _RF_A = false;
    volatile bool _RF_B = false;

    // Compass
    int16_t _headingX;
    int16_t _headingY;
    int16_t _headingZ;
    float _heading;

    HMC5883L compass;
    MPU6050 accelgyro;
  private:

    int _potGoal;
    bool _turn_command_changed;
};

extern Ctrl Controller;

#endif