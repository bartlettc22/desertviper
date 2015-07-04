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

// Custom Libraries
// #include "Messaging.h"

// #include "HallSensor.h"
#include "Controller.h"
//#include "Instruction.h"
//#include "Program.h"

//Msg Messenger = Msg();

//int sr_state = LOW;
//unsigned char turn = TURN_DIRECTION_LEFT;
//bool ledState                   = 0;   // Current state of Led
const int kBlinkLed             = 3;  // Pin of internal Led
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
  kDegreeCorrection  
};

struct {
    int ival;
    float fval;
    char cval;
} config[2];

// Implement varying config types using a "tagged union"


// Temporary variables
int j = 0;

unsigned long loop_time; 
unsigned long loop_count = 0;
float distance_traveled = 0.0; 

int timerLog;  
int timerHeadingSample;
int timerRangeSample;
int timerPressureSample;
//Prog Program = Prog();
//Ctrl Controller = Ctrl();
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

  // set pin for blink LED
  pinMode(kBlinkLed, OUTPUT);  
  //Program.init();
  Controller.init();
  //t.every(2000, getFreq);
  //t.every(5000, steer);
  //t.every(5000, drive);
  //t.every(200, blink2);
  //cmdMessenger.sendCmdStart (kDrive);
  //cmdMessenger.sendCmdArg (DRIVE_DIRECTION_FORWARD);
  //cmdMessenger.sendCmdArg (1);
  //cmdMessenger.sendCmdArg (1.01);
  //cmdMessenger.sendCmdEnd ();
  //t.every(200, turn);
  //Controller.Drive(DRIVE_MOTOR_BOTH, DRIVE_DIRECTION_FORWARD, 0.6);
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

// Callback function that sets led on or off
/*void OnSetLed()
{
  // Read led state argument, interpret string as boolean
  bool ledState = cmdMessenger.readBoolArg();
  // Set led
  digitalWrite(kBlinkLed, ledState?HIGH:LOW);
}*/

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
  Controller.Turn(direction, speed, amount);
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
    int value = cmdMessenger.readInt16Arg();
    Controller.__tickDistance = value/10.0;
    cmdMessenger.sendCmd(kStatus, "Arduino Feedback: Set Distance per Tick to " + String(value/10.0) + 'cm');
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


/*
void steer() {
  if(turn == TURN_DIRECTION_LEFT) {
    turn = TURN_DIRECTION_RIGHT;
    //Controller.turn(turn, 1, 1); // Direction, amount, speed 
  } else {
    turn = TURN_DIRECTION_LEFT;
    //Controller.turn(turn, 1, 1); // Direction, amount, speed 
  }
}*/

/*
void turn() {
   //Serial.println(analogRead(PIN_STEER_POT));
   
  if(j == 0) {
    Controller.Turn(TURN_DIRECTION_LEFT, 1, 1);
    //Controller.Drive(DRIVE_MOTOR_BOTH, DRIVE_DIRECTION_FORWARD, 1);
    Controller.setRegisterPin(0, HIGH); 
    Controller.setRegisterPin(4, LOW);  
    //Serial.println("LEFT");
    j++;
  } else if(j == 1) {
    Controller.Turn(TURN_DIRECTION_BRAKE, 1, 1);
    //Controller.Drive(DRIVE_MOTOR_BOTH, DRIVE_DIRECTION_BRAKE_LOW, 1);
    Controller.setRegisterPin(0, LOW);
    Controller.setRegisterPin(4, HIGH);   
    j++;
  } else if(j == 2) {
    Controller.Turn(TURN_DIRECTION_RIGHT, 1, 1);
    //Controller.Drive(DRIVE_MOTOR_BOTH, DRIVE_DIRECTION_REVERSE, 1);
    Controller.setRegisterPin(0, HIGH); 
    Controller.setRegisterPin(4, LOW);  
    //Serial.println("RIGHT");
    j++;
  } else {
    Controller.Turn(TURN_DIRECTION_BRAKE, 1, 1);
    //Controller.Drive(DRIVE_MOTOR_BOTH, DRIVE_DIRECTION_BRAKE_LOW, 1);
    Controller.setRegisterPin(0, LOW);
    Controller.setRegisterPin(4, HIGH);
    j = 0;
  }
  //Serial.println("writing...");
  //digitalWrite(PIN_SR_RCLK, LOW);
  //shiftOut(PIN_SR_SER, PIN_SR_SRCLK, MSBFIRST, value);
  //digitalWrite(PIN_SR_RCLK, HIGH);
  //Controller.setRegisterPin(0, sr_state);
}*/

