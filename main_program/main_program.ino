#pragma once

// Libraries
#include <Wire.h>
#include "HMC5883L.h"
#include "QueueArray.h"
#include "Event.h"
#include "Timer.h"
#include <math.h>

// Custom Libraries
//#include "MotorController.h"
//#include "SteeringController.h"
#include "HallSensor.h"
#include "Magnetometer.h"
#include "RangeSensor.h"
#include "Controller.h"
#include "Instruction.h"
#include "Program.h"

int i; // intra-secound counter
int j = 0;
int prog_freq; // Last sample frequency
Timer t;
int sr_state = LOW;
//unsigned char turn = TURN_DIRECTION_LEFT;

//Prog Program = Prog();
//Ctrl Controller = Ctrl();
void setup() {
  Serial.begin(9600);
  //Program.init();
  Controller.init();
  t.every(2000, getFreq);
  //t.every(5000, steer);
  //t.every(5000, drive);
  //t.every(200, blink2);
  //t.every(500, writeSerial);
  t.every(200, turn);
  Controller.Drive(DRIVE_MOTOR_BOTH, DRIVE_DIRECTION_FORWARD, 0.6);
}

void loop() {
	//Program.run();
	Controller.run();
  
  t.update();
  i++;
}

void getFreq()
{
  prog_freq = i/2;
  i = 0;
}

void writeSerial() 
{  Serial.print(prog_freq);
  Serial.print(";");

  //Controller.checkDrive();

  //Serial.print(HallSensor.distanceTraveled());
  Serial.println("");
}

void drive() {

  if(j == 0) {
    Controller.Drive(DRIVE_MOTOR_FRONT, DRIVE_DIRECTION_FORWARD, 1);   
    Controller.Drive(DRIVE_MOTOR_REAR, DRIVE_DIRECTION_FORWARD, 1);
  } else if(j == 1) {
    //Controller.drive(DRIVE_MOTOR_FRONT, DRIVE_DIRECTION_BRAKE_HIGH, 1);
    //Controller.drive(DRIVE_MOTOR_REAR, DRIVE_DIRECTION_DISABLE, 1);
  } else {
    //Controller.drive(DRIVE_MOTOR_FRONT, DRIVE_MOTOR_BRAKE_LOW, 1);
    //Controller.drive(DRIVE_MOTOR_REAR, DRIVE_MOTOR_BRAKE_LOW, 1);    
  }

  j++;
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
  //digitalWrite(PIN_SR_RCLK, HIGH);
  Controller.setRegisterPin(0, sr_state);
}


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
}





