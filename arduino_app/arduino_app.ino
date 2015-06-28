#pragma once

// Libraries
#include <Wire.h>
#include <math.h>
#include <string.h>
#include "HMC5883L.h"
#include "QueueArray.h"
#include "Event.h"
#include "Timer.h"
#include "CmdMessenger.h"

// Custom Libraries
// #include "Messaging.h"
#include "HallSensor.h"
#include "Magnetometer.h"
#include "RangeSensor.h"
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
  kTurn
};

// Temporary variables
int j = 0;

unsigned long loop_time; 
unsigned long loop_count = 0;
float distance_traveled = 0.0;   
//Prog Program = Prog();
//Ctrl Controller = Ctrl();
void setup() {

  //Messenger.init();

  // Begin serial communication at high speed 115200 bit/s
  Serial.begin(115200); 
  
  // Adds newline to every command
  cmdMessenger.printLfCr();   

  // Attach my application's user-defined callback methods
  attachCommandCallbacks();

  // Let the serial listener know that the Arduino has started
  cmdMessenger.sendCmd(kStatus,"Arduino has started!");

  // Send logs to Raspberry every 500ms
  t.every(50, sendLog);

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
  cmdMessenger.sendCmdStart (kDrive);
  cmdMessenger.sendCmdArg (DRIVE_DIRECTION_FORWARD);
  cmdMessenger.sendCmdArg (1);
  cmdMessenger.sendCmdArg (1.01);
  cmdMessenger.sendCmdEnd ();
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
  double speed = cmdMessenger.readDoubleArg ();

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

void sendLog() 
{  
  // Capture program frequency)
  unsigned long capture_time = micros();
  float capture_distance = HallSensor.distanceTraveled();
  int freq = round(loop_count/((capture_time-loop_time)/1000000.0));
  

  String output = String(round(capture_time/1000.0));
  output.concat(":");
  output.concat(freq);
  output.concat(":");
  output.concat(Controller._potValue);
  output.concat(":");
  output.concat(HallSensor.ticks);
  output.concat(":"); 
  output.concat(Controller._speedGoalFront);
  output.concat(":"); 
  output.concat(Controller._frontCurrent);
  output.concat(":");  
  output.concat(Controller._speedGoalRear);
  output.concat(":");
  output.concat(Controller._rearCurrent);
  output.concat(":");
  output.concat((capture_distance - distance_traveled)/((capture_time-loop_time)/1000000.0));  
  output.concat(":");
  output.concat(capture_distance);  
  output.concat(":");
  //output.concat(turn amount);     
  output.concat(":");
  //output.concat(turn goal); 
  output.concat(":"); 
  output.concat(Controller._frontFault);
  output.concat(":");   
  output.concat(Controller._rearFault);
  output.concat(":"); 
  output.concat(digitalRead(PIN_RF_A));
  output.concat(":");   
  output.concat(Controller._RF_B);  
  output.concat(":");   
  output.concat(Controller._POT_LEFT_MAX);   
  output.concat(":");   
  output.concat(Controller._POT_RIGHT_MAX);   
  output.concat(":");   
  output.concat(Controller._potCenter);       
  //output.concat(Controller._POT_RIGHT_MAX);
  //output.concat(":"); 
  //output.concat(Controller._potCenter);
  //output.concat(":"); 
  //output.concat(Controller._turnAmountPot);
  output.concat(":"); 
 //output.concat(HallSensor.distanceTraveled());
  //output.concat(":");

  cmdMessenger.sendCmd(kLog, output);

  loop_count = 0;
  loop_time = capture_time; 
  distance_traveled = capture_distance;
}

/*
void drive() {

  if(j == 0) {
    Controller.Drive(DRIVE_MOTOR_FRONT, DRIVE_DIRECTION_BRAKE_HIGH, 1);
    Controller.Drive(DRIVE_MOTOR_REAR, DRIVE_DIRECTION_BRAKE_LOW, 1);

  } else if(j == 1) {
    Controller.Drive(DRIVE_MOTOR_FRONT, DRIVE_DIRECTION_FORWARD, 1);   
    Controller.Drive(DRIVE_MOTOR_REAR, DRIVE_DIRECTION_FORWARD, 1);    
  } else {
    Controller.Drive(DRIVE_MOTOR_FRONT, DRIVE_DIRECTION_BRAKE_LOW, 1);
    Controller.Drive(DRIVE_MOTOR_REAR, DRIVE_DIRECTION_BRAKE_LOW, 1);    
  }

  j++;
}*/

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
void blink2() {

  char value;

  if(sr_state == LOW) {
    sr_state = HIGH;
    //value = B01010101;
  } else {
    sr_state = LOW;
    //value = B10101010;
  }
  //Serial.println("writing...");
  //digitalWrite(PIN_SR_RCLK, LOW);
  //shiftOut(PIN_SR_SER, PIN_SR_SRCLK, MSBFIRST, value);
  digitalWrite(kBlinkLed, sr_state);
  //Controller.setRegisterPin(0, sr_state);
}
*/
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

