// Libraries
#include <Wire.h>
#include <math.h>
#include <string.h>
#include "I2Cdev.h"
#include "HMC5883L.h"
#include "MPU6050.h"
#include "QueueArray.h"
#include "Event.h"
#include "Timer.h"
#include "CmdMessenger.h"
#include "Kalman.h"
#include "SFE_BMP180.h"
#include "Controller.h"

Timer t;
CmdMessenger cmdMessenger = CmdMessenger(Serial);

// Commands
enum
{
  kSetLed, // Command to request led to be set in specific state
  kStatus,
  kLog,
  kDrive,
  kCalibrate,
  kTurn,
  kSetConfig
};

// Configs
enum
{
  kLogSpeed,
  kHeadingSampleRate,
  kFrontMotorRamp,
  kRearMotorRamp,
  kTickDistance,
  kSpeedOfSound, 
  kRangeMin, 
  kRangeMax,
  kRangeSampleRate,
  kPressureSampleRate,
  kDegreeCorrection,
  kSteeringLeftMax,
  kSteeringRightMax
};

struct {
    int ival;
    float fval;
    char cval;
} config[2];

// Temporary variables
int j = 0;

unsigned long loop_time; 
unsigned long loop_count = 0;
float distance_traveled = 0.0; 

int timerLog;  
int timerHeadingSample;
int timerRangeSample;
int timerPressureSample;

void setup() {

  // 
  config[kLogSpeed].ival = 50;

  // Begin serial communication at high speed 115200 bit/s
  Serial.begin(115200); 
  
  // Adds newline to every command
  cmdMessenger.printLfCr();   

  // Attach my application's user-defined callback methods
  attachCommandCallbacks();

  // Let the serial listener know that the Arduino has started
  cmdMessenger.sendCmd(kStatus,"Arduino has started!");

  // Send logs to Raspberry every Xms
  timerLog = t.every(config[kLogSpeed].ival, sendLog);

  // Get heading values every Xms
  timerHeadingSample = t.every(50, getHeadingSample);

  // Get range values every Xms
  timerRangeSample = t.every(100, getRangeSample);

  // Get pressure values every Xms
  timerPressureSample = t.every(2000, getPressureSample);

  // Start the loop timer
  loop_time = micros();

  Controller.init();
}

void loop() {

  // Check for new serial commands
  cmdMessenger.feedinSerialData();

  // Trigger any timed events
  t.update();

  // Run controller procedures
	Controller.run();
  
  // Iterate loop counter
  loop_count++;

  //Messenger.run();


}

// Callbacks define on which received commands we take action 
void attachCommandCallbacks()
{
  cmdMessenger.attach(OnUnknownCommand);
  cmdMessenger.attach(kDrive, OnDrive);
  cmdMessenger.attach(kCalibrate, OnCalibrate);
  cmdMessenger.attach(kTurn, OnTurn);
  cmdMessenger.attach(kSetConfig, OnSetConfig);
}

// Called when a received command has no attached function
void OnUnknownCommand()
{
  cmdMessenger.sendCmd(kStatus,"Command without attached callback");
}

// Callback function for remote drive command
void OnDrive()
{
  // Read arguments
  unsigned char motor = cmdMessenger.readInt16Arg();
  unsigned char direction = cmdMessenger.readInt16Arg();
  float speed = cmdMessenger.readFloatArg ();

  String output = "Drive command received: ";
  output.concat(motor);
  output.concat(":");
  output.concat(direction);
  output.concat(":");
  output.concat(speed);
  cmdMessenger.sendCmd(kStatus, output);
  
  Controller.Drive(motor, direction, speed);
}

// Callback function for remote calibrate command
void OnCalibrate()
{
  cmdMessenger.sendCmd(kStatus, "Arduino Feedback: Calibrating...");
  Controller.CalibrateSteering();
}

// Callback function for remote turn command
void OnTurn()
{
  // Read arguments
  unsigned char direction = cmdMessenger.readInt16Arg();
  double speed = cmdMessenger.readDoubleArg();
  double amount = cmdMessenger.readDoubleArg();

  String output = "Arduino Feedback: Turn command received: ";
  output.concat(direction);
  output.concat(":");
  output.concat(speed);
  output.concat(":");
  output.concat(amount);
  cmdMessenger.sendCmd(kStatus, output);
  Controller._turningDirection = direction;
  Controller._turningSpeed = round(speed * 255);

  if(direction == MOTOR_DIRECTION_CCW) {
    Controller._turnLeftGoal = Controller._potCenter + (amount*(Controller._POT_LEFT_MAX - Controller._potCenter));
  } else if (direction == MOTOR_DIRECTION_CW) {
    Controller._turnRightGoal = Controller._potCenter - (amount*(Controller._potCenter - Controller._POT_RIGHT_MAX));
  }
  
  //Controller.Turn(direction, speed, amount);
}

void OnSetConfig() {
  unsigned char key = cmdMessenger.readInt16Arg();
  if(key == kLogSpeed) {
    int value = cmdMessenger.readInt16Arg();
    t.stop(timerLog);
    timerLog = t.every(value, sendLog);
    cmdMessenger.sendCmd(kStatus, "Arduino Feedback: Set Log Speed to " + String(value) + "ms");
  } else if(key == kHeadingSampleRate) {
    int value = cmdMessenger.readInt16Arg();
    t.stop(timerHeadingSample);
    timerHeadingSample = t.every(value, getHeadingSample);
    cmdMessenger.sendCmd(kStatus, "Arduino Feedback: Set Heading Sample Speed to " + String(value) + "ms");
  } else if(key == kFrontMotorRamp) {
    int value = cmdMessenger.readInt16Arg();
    Controller.__motorRamp[MOTOR_FRONT] = value;
    cmdMessenger.sendCmd(kStatus, "Arduino Feedback: Set Front Motor Ramp to " + String(value) + "ms");
  } else if(key == kRearMotorRamp) {
    int value = cmdMessenger.readInt16Arg();
    Controller.__motorRamp[MOTOR_REAR] = value;
    cmdMessenger.sendCmd(kStatus, "Arduino Feedback: Set Rear Motor Ramp to " + String(value) + 'ms');
  } else if(key == kTickDistance) {
    int value = cmdMessenger.readInt32Arg();
    Controller.__tickDistance = value/10000.0;
    cmdMessenger.sendCmd(kStatus, "Arduino Feedback: Set Distance per Tick to " + String(value/10000.0) + 'cm');
  } else if(key == kSpeedOfSound) {
    long value = cmdMessenger.readInt32Arg();
    Controller.__speedOfSound = value;
    Controller.calcRangeTimeout();
    cmdMessenger.sendCmd(kStatus, "Arduino Feedback: Set Speed of Sound to " + String(value) + 'cm/s');
  } else if(key == kRangeMin) {
    int value = cmdMessenger.readInt16Arg();
    Controller.__rangeMin = value;
    cmdMessenger.sendCmd(kStatus, "Arduino Feedback: Set Minimum Detecting Range to " + String(value) + 'cm');
  } else if(key == kRangeMax) {
    int value = cmdMessenger.readInt16Arg();
    Controller.__rangeMax = value;
    Controller.calcRangeTimeout();
    cmdMessenger.sendCmd(kStatus, "Arduino Feedback: Set Maximum Detecting Range to " + String(value) + 'cm');
  } else if(key == kRangeSampleRate) {
    int value = cmdMessenger.readInt16Arg();
    t.stop(timerRangeSample);
    timerRangeSample = t.every(value, getRangeSample);
    cmdMessenger.sendCmd(kStatus, "Arduino Feedback: Set Range Sample Speed to " + String(value) + "ms");
  } else if(key == kPressureSampleRate) {
    int value = cmdMessenger.readInt16Arg();
    t.stop(timerPressureSample);
    timerPressureSample = t.every(value, getPressureSample);
    cmdMessenger.sendCmd(kStatus, "Arduino Feedback: Set Pressure Sample Speed to " + String(value) + "ms");
  } else if(key == kDegreeCorrection) {
    int value = cmdMessenger.readInt16Arg();
    Controller.__degreeCorrection = value;
    cmdMessenger.sendCmd(kStatus, "Arduino Feedback: Heading corrected by " + String(value) + " degrees");
  } else if(key == kSteeringLeftMax) {
    int value = cmdMessenger.readInt16Arg();
    Controller._POT_LEFT_MAX = value;
    cmdMessenger.sendCmd(kStatus, "Arduino Feedback: Setting Potentiometer LEFT max to " + String(value));
  } else if(key == kSteeringRightMax) {
    int value = cmdMessenger.readInt16Arg();
    Controller._POT_RIGHT_MAX = value;
    cmdMessenger.sendCmd(kStatus, "Arduino Feedback: Setting Potentiometer RIGHT max to " + String(value));
  } else {
    cmdMessenger.sendCmd(kStatus, "Arduino Feedback: Unknow Config Setting Received!");
  }
}

void sendLog() 
{  
  // Capture program frequency)
  unsigned long capture_time = micros();
  float capture_distance = Controller.distanceTraveled();
  int freq = round(loop_count/((capture_time-loop_time)/1000000.0));
  Controller.readAccelGyro();
  

  String output = String(round(capture_time/1000.0));
  output.concat(":");
  output.concat(freq);
  output.concat(":");
  output.concat(Controller._potValue);
  output.concat(":");
  output.concat(Controller._ticks);
  output.concat(":"); 
  output.concat(Controller.__motorGoalSpeed[MOTOR_FRONT]/255.0);
  output.concat(":"); 
  output.concat(Controller._frontCurrent);
  output.concat(":");  
  output.concat(Controller.__motorGoalSpeed[MOTOR_REAR]/255.0);
  output.concat(":");
  output.concat(Controller._rearCurrent);
  output.concat(":");
  output.concat((capture_distance - distance_traveled)/((capture_time-loop_time)/1000000.0));  
  output.concat(":");
  output.concat(capture_distance);  
  output.concat(":");
  output.concat(":");
  output.concat(":"); 
  output.concat(Controller._frontFault);
  output.concat(":");   
  output.concat(Controller._rearFault);
  output.concat(":"); 
  output.concat(digitalRead(PIN_RF_A));
  output.concat(":");   
  output.concat(digitalRead(PIN_RF_B));  
  output.concat(":");   
  output.concat(Controller._POT_LEFT_MAX);   
  output.concat(":");   
  output.concat(Controller._POT_RIGHT_MAX);   
  output.concat(":");   
  output.concat(Controller._potCenter); 
  output.concat(":");   
  output.concat(Controller._rangeDistance);         
  output.concat(":");   
  output.concat(Controller._rangeDuration); 
  output.concat(":");   
  output.concat(Controller._heading); 
  output.concat(":");   
  output.concat(Controller.__motorCurrentSpeed[MOTOR_FRONT]/255.0);  
  output.concat(":");   
  output.concat(Controller.__motorCurrentSpeed[MOTOR_REAR]/255.0);
  output.concat(":"); 
  output.concat(digitalRead(PIN_RF_C));
  output.concat(":");   
  output.concat(digitalRead(PIN_RF_D));  
  output.concat(":");   
  output.concat(Controller._headingX); 
  output.concat(":");   
  output.concat(Controller._headingY); 
  output.concat(":");   
  output.concat(Controller._headingZ);  
  output.concat(":");   
  output.concat(Controller.__rangeTimeout); 
  output.concat(":");   
  output.concat(Controller._ax); 
  output.concat(":");   
  output.concat(Controller._ay); 
  output.concat(":");   
  output.concat(Controller._az);
  output.concat(":");   
  output.concat(Controller._gx); 
  output.concat(":");   
  output.concat(Controller._gy); 
  output.concat(":");   
  output.concat(Controller._gz);  

  output.concat(":");   
  output.concat(Controller._altitudeDiff); 
  output.concat(":");   
  output.concat(Controller._baselinePressure); 
  output.concat(":");   
  output.concat(Controller._temperature); 
  output.concat(":");   
  output.concat(Controller._temperatureF); 
  output.concat(":");   
  output.concat(Controller._pressure); 
  output.concat(":");   
  output.concat(Controller._pressureHG); 
  output.concat(":");   
  output.concat(Controller._pressure0); 
  output.concat(":");   
  output.concat(Controller._pressure0HG);   
  output.concat(":");   
  output.concat(Controller._turnLeftGoal);  
  output.concat(":");   
  output.concat(Controller._turnRightGoal); 
  output.concat(":");   
  output.concat(Controller._turningStatus);        
        

  cmdMessenger.sendCmd(kLog, output);

  loop_count = 0;
  loop_time = capture_time; 
  distance_traveled = capture_distance;
}

void getHeadingSample() {
  Controller.getHeading();
}

void getRangeSample() {
  Controller.getRange();
}

void getPressureSample() {
  Controller.getPressure();
}

