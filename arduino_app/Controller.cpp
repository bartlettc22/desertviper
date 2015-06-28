#include "Controller.h"

void RF_interrupt() {
  Controller._RF_A = digitalRead(PIN_RF_A);
  Controller._RF_B = digitalRead(PIN_RF_B);
}

void Ctrl::init()
{
  // Steering
  pinMode(PIN_STEER_PWM, OUTPUT);
  pinMode(PIN_STEER_POT, INPUT);


  // Main Motor Controls
  pinMode(PIN_MOTOR_INA1, OUTPUT);
  pinMode(PIN_MOTOR_INB1, OUTPUT);
  pinMode(PIN_MOTOR_PWM1, OUTPUT);
  pinMode(PIN_MOTOR_EN1DIAG1, INPUT);
  pinMode(PIN_MOTOR_CS1, INPUT);
  pinMode(PIN_MOTOR_INA2, OUTPUT);
  pinMode(PIN_MOTOR_INB2, OUTPUT);
  pinMode(PIN_MOTOR_PWM2, OUTPUT);
  pinMode(PIN_MOTOR_EN2DIAG2, INPUT);
  pinMode(PIN_MOTOR_CS2, INPUT);  

  // Initialize Motors Braked
  digitalWrite(PIN_MOTOR_INA1, LOW);
  digitalWrite(PIN_MOTOR_INB1, LOW);
  digitalWrite(PIN_MOTOR_INA2, LOW);
  digitalWrite(PIN_MOTOR_INB2, LOW);  

  // Initialize Hall Effect Sensor
  HallSensor.init(PIN_HALL, INRPT_HALL);

  // Initialize RF Receiver Interrupts
  pinMode(PIN_RF_A, INPUT);
  pinMode(PIN_RF_B, INPUT);
  attachInterrupt(INRPT_RF, RF_interrupt, CHANGE);
  //attachInterrupt(INRPT_RF_A, RF_A_interrupt, CHANGE);
  //attachInterrupt(INRPT_RF_B, RF_B_interrupt, CHANGE);

  //Compass.init();
  //RangeSensor.init(PIN_RANGE_TRIG, PIN_RANGE_ECHO);
  //CalibrateSteering();
}

void Ctrl::run()
{
  checkSteering();
  checkDrive();
}

void Ctrl::checkSteering() {

  _potValue = analogRead(PIN_STEER_POT);
  //Serial.println(_potValue);
  // If we're within x of goal, stop motors to lock wheels in this position
  if (_potValue >= (_potGoal - 10) && _potValue <= (_potGoal + 10)) {
    //setMBrake(400); // Full Brake to hold turn
    //setSteerBrake();
    //Serial.println("Stopping, within goal");

  } else if(_potValue < _potGoal) {
    //Serial.println("Stopping, within goal");
    //setMSpeed(-1 * (stateSpeed * 400));
  } else if(_potValue > _potGoal) {
    //Serial.println("Stopping, within goal");
    //setMSpeed(-1 * (stateSpeed * 400));  
  }
}


void Ctrl::Turn(unsigned char direction, float speed, float amount) {
  
  // Default Brake
  int _INA = LOW;
  int _INB = LOW;
  int _speed = round(speed * 255);

  // Amount of turn is based on potentiometer reading

  if(direction == TURN_DIRECTION_LEFT) {
    _INA = LOW;
    _INB = HIGH;
  } else if(direction == TURN_DIRECTION_RIGHT) {
    _INA = HIGH;
    _INB = LOW;    
  } else if(direction == TURN_DIRECTION_BRAKE) {
    _INA = LOW;
    _INB = LOW;
    // _INA = HIGH;
    // _INB = HIGH;
  }

  analogWrite(PIN_STEER_PWM, _speed);
  digitalWrite(PIN_STEER_INA1, _INA);
  digitalWrite(PIN_STEER_INB1, _INB);
  //setRegisterPin(SPIN_STEER_INA1, _INA);
  //setRegisterPin(SPIN_STEER_INB1, _INB);
}


void Ctrl::Drive(unsigned char motor, unsigned char direction, double speed)
{
  // Default Brake
  int _INA = LOW;
  int _INB = LOW;
  int _speed = round(speed * 255);
  int _speedGoal = speed;

  if(direction == DRIVE_DIRECTION_FORWARD) {
    _INA = HIGH;
    _INB = LOW;
  } else if(direction == DRIVE_DIRECTION_REVERSE) {
    _INA = LOW;
    _INB = HIGH;
    _speedGoal = -1 * _speedGoal;
  } else if(direction == DRIVE_DIRECTION_BRAKE_LOW) {
    _INA = LOW;
    _INB = LOW;
    _speedGoal = 0;
  } else if(direction == DRIVE_DIRECTION_BRAKE_HIGH) {
    _INA = HIGH;
    _INB = HIGH;
    _speedGoal = 0;
  }

  if(motor == DRIVE_MOTOR_FRONT || motor == DRIVE_MOTOR_BOTH) {
    analogWrite(PIN_MOTOR_PWM2, _speed);
    digitalWrite(PIN_MOTOR_INA2, _INA);
    digitalWrite(PIN_MOTOR_INB2, _INB);
    _speedGoalFront = _speedGoal;
  } 

  if(motor == DRIVE_MOTOR_REAR || motor == DRIVE_MOTOR_BOTH) {
    analogWrite(PIN_MOTOR_PWM1, _speed);
    digitalWrite(PIN_MOTOR_INA1, _INA);
    digitalWrite(PIN_MOTOR_INB1, _INB);
    _speedGoalRear = _speedGoal;
  }
}

void Ctrl::checkDrive() {
  // 5V / 1024 ADC counts / 144 mV per A = 34 mA per count
  // Could be more like 37/38.. we'll go with 38 to be safe
  //return analogRead(PIN_MOTOR_CS2) * 34;
  //Serial.print(digitalRead(PIN_MOTOR_EN2DIAG2));
  //Serial.print(":");
  _frontFault = !digitalRead(PIN_MOTOR_EN2DIAG2);
  _frontCurrent = analogRead(PIN_MOTOR_CS2)*38;
 // Serial.print();
 // Serial.print("mA;"); 

//  Serial.print(digitalRead(PIN_MOTOR_EN1DIAG1));
 // Serial.print(":");
 // Serial.print(analogRead(PIN_MOTOR_CS1)*38);
  _rearFault = !digitalRead(PIN_MOTOR_EN1DIAG1);
  _rearCurrent = analogRead(PIN_MOTOR_CS1)*38;
 // Serial.print("mA;");
 
  //Serial.println("");
}


void Ctrl::CalibrateSteering() {
  Turn(TURN_DIRECTION_LEFT, 1, 1);
  delay(1000);
  _POT_LEFT_MAX = analogRead(PIN_STEER_POT);
  Turn(TURN_DIRECTION_BRAKE, 1, 1);
  delay(1000);
  Turn(TURN_DIRECTION_RIGHT, 1, 1);
  delay(1000);
  _POT_RIGHT_MAX = analogRead(PIN_STEER_POT);
  Turn(TURN_DIRECTION_BRAKE, 1, 1);  

  _potCenter = (_POT_LEFT_MAX + _POT_RIGHT_MAX) / 2;
  _turnAmountPot = (_POT_LEFT_MAX - _potCenter); // Potentiometer value (to add to center) for 100% turn

}

Ctrl Controller = Ctrl();




