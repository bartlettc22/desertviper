#ifndef Controller_h
#define Controller_h

// Libraries
#include <Arduino.h>
#include "I2Cdev.h"
#include "HMC5883L.h"
#include "MPU6050.h"
#include <math.h>
#include "Kalman.h"
#include "SFE_BMP180.h"

// Motor Controller PINs
// Motor 1 = Rear Wheels
// Motor 2 = Front Wheels
#define PIN_MOTOR_FRONT_INA       4
#define PIN_MOTOR_FRONT_INB       9 
#define PIN_MOTOR_FRONT_ENDIAG   A1
#define PIN_MOTOR_FRONT_PWM       6
#define PIN_MOTOR_FRONT_CS        3
#define PIN_MOTOR_REAR_INA		  7 
#define PIN_MOTOR_REAR_INB		  8
#define PIN_MOTOR_REAR_ENDIAG	  A0
#define PIN_MOTOR_REAR_PWM        5
#define PIN_MOTOR_REAR_CS 		  2


// Hall Effect (Tick) Sensor PINS
#define PIN_HALL     2    // PIN #
#define INRPT_HALL   0    // INTERRUPT # (PIN2 = Interrupt 0, PIN 3 = Interrupt 1)

// RF Receiver
#define PIN_RF        3 // RF Receiver VT - Any button pressed
#define INRPT_RF      1 
#define PIN_RF_A      24 // RF Receiver D2
#define INRPT_RF_A    4 
#define PIN_RF_B      25 // RF Receiver D0
#define INRPT_RF_B    5 
#define PIN_RF_C   27 // RF Receiver D3
#define PIN_RF_D   26 // RF Receiver D1

// Steering Controller Motor/Potentiometer
#define PIN_STEER_PWM         10  // Steering Motor Speed Control (PWM)
#define PIN_STEER_INA1        22  // Steering Motor Direction/Input 1
#define PIN_STEER_INB1        23  // Steering Motor Direction/Input 2
//#define PIN_STEER_EN1DIAG1    1   // Steering Motor Standby/Sleep
#define PIN_STEER_POT          15   // Steering Motor Position Potentiometer

// Range Sensor
#define PIN_RANGE_TRIG   46
#define PIN_RANGE_ECHO   47

#define MOTOR_FRONT                 0
#define MOTOR_REAR                  1
#define MOTOR_BOTH                  2
#define MOTOR_STEER                 3

#define MOTOR_DIRECTION_CW          0 // Forward/Right
#define MOTOR_DIRECTION_CCW         1 // Reverse/Left
#define MOTOR_DIRECTION_COAST       2
#define MOTOR_DIRECTION_BRAKE_LOW   3
#define MOTOR_DIRECTION_BRAKE_HIGH  4
#define MOTOR_DIRECTION_DISABLE     5 

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

    void MotorControl(unsigned char motor_index, unsigned char direction, int speed);

    /*
    * Control primary drive motors
    *
    * @param motor      Specify which motor to control (DRIVE_MOTOR_FRONT, DRIVE_MOTOR_REAR, DRIVE_MOTOR_BOTH)
    * @param direction  Specify which direction to drive (DRIVE_MOTOR_FORWARD, DRIVE_MOTOR_REVERSE, DRIVE_DIRECTION_BRAKE_LOW, DRIVE_DIRECTION_BRAKE_HIGH)
    * @param speed      Specify speed to drive (0 to 1, for example 0.5 drives at half speed)
    */
    void Drive(unsigned char motor, unsigned char direction, float speed);
    
    /*
    * Control steering motors
    *
    * @param direction  Specify which direction to drive (DRIVE_MOTOR_FORWARD, DRIVE_MOTOR_REVERSE, DRIVE_DIRECTION_BRAKE_LOW, DRIVE_DIRECTION_BRAKE_HIGH)
    * @param speed      Specify speed to drive (0 to 1, for example 0.5 drives at half speed)
    * @param amount     Specify how hard to turnt the wheels (0 to 1, for example 1 is full turn, 0.5 is half turn)
    */
    void Turn(unsigned char direction, int speed);

    void CalibrateSteering();

    void checkSteering();
    void checkDrive();
    void getRange();
    void getHeading();

    float distanceTraveled();
    void calcRangeTimeout();
    void readAccelGyro();
    void getPressure();

    int __motorINAPIN[3]            = {PIN_MOTOR_FRONT_INA, PIN_MOTOR_REAR_INA, 0};
    int __motorINBPIN[3]            = {PIN_MOTOR_FRONT_INB, PIN_MOTOR_REAR_INB, 0};
    int __motorPWMPin[3]            = {PIN_MOTOR_FRONT_PWM, PIN_MOTOR_REAR_PWM, 0};
    int __motorRamp[3]              = {3000, 3000, 3000};
    int __motorCurrentSpeed[3]    = {0, 0, 0};
    int __motorGoalSpeed[3]       = {0, 0, 0};
    int __motorDirection[3]         = {MOTOR_DIRECTION_BRAKE_LOW, MOTOR_DIRECTION_BRAKE_LOW, MOTOR_DIRECTION_BRAKE_LOW};
    float __motorAcc[3]             = {0.0, 0.0, 0.0};
    unsigned long __motorAccTime[3]           = {0, 0, 0};

    // Front/Rear/Steering motor ramp-up (acceleration) speed (milliseconds to get from 0-100%)
    

    int __frontMotorRamp = 3000;
    int __rearMotorRamp = 3000;
    int __steerMotorRamp = 3000;

    // Front/Rear/Steering motor current speed (in %)
    float _frontCurrentSpeed = 0.0;
    float _rearCurrentSpeed = 0.0;
    float _steerCurrentSpeed = 0.0;

    // Front/Rear/Steering motor goal speed (in %)    
    float _frontGoalSpeed = 0.0;
    float _rearGoalSpeed = 0.0;
    float _steerGoalSpeed = 0.0;

    // Front/Rear/Steering motor current direction (in %)    
    float _frontDirection = 0.0;
    float _rearDirection = 0.0;
    float _steerDirection = 0.0;    

    // Potentiometer Tolerance for maintaining position (in analog)
    int __potMaintainTolerance = 10;

    int _potValue;
    int _frontCurrent;
    bool _frontFault;
    int _rearCurrent;
    bool _rearFault;

    // Hall Effect Sensor config/readings
    unsigned long _ticks = 0;
    int __tickDistance = 9;

    // Range sensor config/readings
    unsigned int __speedOfSound = 33350; // cm/s
    unsigned int __rangeMin = 2; // cm
    unsigned int __rangeMax = 400; // cm
    unsigned long __rangeTimeout = 1000000;
    unsigned int _rangeDuration; // us
    float _rangeDistance; // cm

    /*
      Potentiometer Edge Values (Right - Left)
      Base: 0 - 1023
      Casing: ~100 - ~910
      Wheels: ~352 - ~630
      Ideal: 380 - 610
    */
    int _POT_LEFT_MAX = 800; //688
    int _POT_RIGHT_MAX = 200; // 431    
    int _potCenter = 472; // 560
    int _turnAmountPot = 0; // Potentiometer value for 1% turn
    int _turnLeftGoal = _POT_LEFT_MAX;
    int _turnRightGoal = _POT_RIGHT_MAX;
    int _turningDirection = MOTOR_DIRECTION_BRAKE_LOW;
    int _turningSpeed = 255;
    String _turningStatus = "";

    // RF Receiver
    volatile bool _RF_A = false;
    volatile bool _RF_B = false;

    // Compass
    int16_t _headingX;
    int16_t _headingY;
    int16_t _headingZ;
    float _heading;
    int __degreeCorrection = 90;

    // Accelerometer/Gyro Values
    int16_t _ax, _ay, _az;
    int16_t _gx, _gy, _gz;

    // Pressure Sensor
    float __altitude = 1655.0; // meters
    double _altitudeDiff; // Calculated altitude difference
    double _baselinePressure;
    double _temperature;
    double _temperatureF;
    double _pressure; // Absolute pressure reading in mb
    double _pressureHG; // Absolute pressure reading in inches of HG
    double _pressure0; // Altitude adjusted pressure reading in mb
    double _pressure0HG; // Altitude adjusted pressure reading in inches of HG    

    HMC5883L compass;
    MPU6050 accelgyro;
    SFE_BMP180 pressure;
  private:

    int _potGoal;
    bool _turn_command_changed;
};

extern Ctrl Controller;

#endif