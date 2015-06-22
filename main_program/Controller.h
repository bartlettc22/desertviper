#ifndef Controller_h
#define Controller_h

// Libraries
#include <Arduino.h>
#include "HMC5883L.h"
#include <math.h>

// Custom Libraries
//#include "MotorController.h"
//#include "SteeringController.h"
#include "HallSensor.h"
#include "Magnetometer.h"
#include "RangeSensor.h"

//How many of the shift registers - change this
#define number_of_74hc595s 1

//do not touch
#define numOfRegisterPins number_of_74hc595s * 8

// Shift Register (74hc595)
#define PIN_SR_SER      11   // Shift Register Serial Data (DS/SER)
#define PIN_SR_SRCLK    12   // Shift Register Serial Clock (SHCP/SRCLK)
#define PIN_SR_RCLK     13   // Shift Register Register Clock (STCP/Latch)

// On-board LED
//#define PIN_LED             13
//#define CYCLE_MOD           1000

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

// Steering Controller Motor/Potentiometer
#define PIN_STEER_PWM          10  // Steering Motor Speed Control (PWM)
#define SPIN_STEER_INA1        2  // Steering Motor Direction/Input 1
#define SPIN_STEER_INB1        3  // Steering Motor Direction/Input 2
#define SPIN_STEER_EN1DIAG1    1   // Steering Motor Standby/Sleep
#define PIN_STEER_POT          5   // Steering Motor Position Potentiometer

// Range Sensor
//#define PIN_RANGE_TRIG   3
//#define PIN_RANGE_ECHO   4

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
#define DRIVE_DIRECTION_DISABLE     5 // Don't use this as it isn't implemented

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


    void checkSteering();
    void checkDrive();
    void writeRegisters();
    void clearRegisters();
    void setRegisterPin(int index, int value);
    int _potValue;
  private:




    int cycleCount;
    int ledState = LOW;
    bool registers[numOfRegisterPins]; 
    bool shift_register_changed = false; // Flagged true if shift register value changed during cycle, triggers writeRegisters();
  	// Peripherals
  	/*HSensor HallSensor;*/
  	//Magnetometer Compass;
  	/*RSensor RangeSensor;
    SController SteeringController;
  	MController MotorController;*/
    
    /*
      Potentiometer Edge Values (Right - Left)
      Base: 0 - 1023
      Casing: ~100 - ~910
      Wheels: ~352 - ~630
      Ideal: 380 - 610
    */
    int const static _POT_LEFT_MAX = 670; //688
    int const static _POT_RIGHT_MAX = 431; // 431    
    int _potCenter; // 560
    int _turnAmountPot; // Potentiometer value for 1% turn
    int _potGoal;
    bool _turn_command_changed;
};

extern Ctrl Controller;

#endif





