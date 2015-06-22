#include "Controller.h"

void Ctrl::init()
{
  // Shift Register
  pinMode(PIN_SR_SER, OUTPUT);
  pinMode(PIN_SR_RCLK, OUTPUT);
  pinMode(PIN_SR_SRCLK, OUTPUT);
  clearRegisters(); // Set all registers to LOW
  writeRegisters(); // Save changes

  // Steering
  pinMode(PIN_STEER_PWM, OUTPUT);
  pinMode(PIN_STEER_POT, INPUT);
  _potCenter = (_POT_LEFT_MAX + _POT_RIGHT_MAX) / 2;
  _turnAmountPot = (_POT_LEFT_MAX - _potCenter); // Potentiometer value (to add to center) for 100% turn

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
  //Compass.init();
  //RangeSensor.init(PIN_RANGE_TRIG, PIN_RANGE_ECHO);

}

void Ctrl::run()
{

  checkSteering();
/*setRegisterPin(0, HIGH);
setRegisterPin(1, HIGH);
setRegisterPin(2, HIGH);
setRegisterPin(3, HIGH);
setRegisterPin(4, HIGH);
setRegisterPin(5, HIGH);
setRegisterPin(7, HIGH);
writeRegisters();*/

  // If a register was changed during this cycle (by calling setRegisterPin()), save it
  if(shift_register_changed == true) {
    writeRegisters();
    shift_register_changed = false;
  }
  
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

  //digitalWrite(SPIN_STEER_INA1, _INA);
  //digitalWrite(SPIN_STEER_INB1, _INB);
  setRegisterPin(SPIN_STEER_INA1, _INA);
  setRegisterPin(SPIN_STEER_INB1, _INB);
}


//set all register pins to LOW
void Ctrl::clearRegisters(){
  for(int i = numOfRegisterPins - 1; i >=  0; i--){
     registers[i] = LOW;
  }
}

// Set and display registers
// Only call AFTER all values are set how you would like (slow otherwise)
void Ctrl::writeRegisters(){

  digitalWrite(PIN_SR_RCLK, LOW);

  for(int i = numOfRegisterPins - 1; i >=  0; i--){
    digitalWrite(PIN_SR_SRCLK, LOW);

    int val = registers[i];

    digitalWrite(PIN_SR_SER, val);
    digitalWrite(PIN_SR_SRCLK, HIGH);

  }
  digitalWrite(PIN_SR_RCLK, HIGH);

}

//set an individual pin HIGH or LOW
void Ctrl::setRegisterPin(int index, int value){
  registers[index] = value;
  shift_register_changed = true;
}

void Ctrl::Drive(unsigned char motor, unsigned char direction, double speed)
{
  // Default Brake
  int _INA = LOW;
  int _INB = LOW;
  int _speed = round(speed * 255);

  if(direction == DRIVE_DIRECTION_FORWARD) {
    _INA = HIGH;
    _INB = LOW;
  } else if(direction == DRIVE_DIRECTION_REVERSE) {
    _INA = LOW;
    _INB = HIGH;
  } else if(direction == DRIVE_DIRECTION_BRAKE_LOW) {
    _INA = LOW;
    _INB = LOW;
  } else if(direction == DRIVE_DIRECTION_BRAKE_HIGH) {
    _INA = HIGH;
    _INB = HIGH;
  }

  if(motor == DRIVE_MOTOR_FRONT || motor == DRIVE_MOTOR_BOTH) {
    analogWrite(PIN_MOTOR_PWM2, _speed);
    digitalWrite(PIN_MOTOR_INA2, _INA);
    digitalWrite(PIN_MOTOR_INB2, _INB);
  } 

  if(motor == DRIVE_MOTOR_REAR || motor == DRIVE_MOTOR_BOTH) {
    analogWrite(PIN_MOTOR_PWM1, _speed);
    digitalWrite(PIN_MOTOR_INA1, _INA);
    digitalWrite(PIN_MOTOR_INB1, _INB);
  }
}

void Ctrl::checkDrive() {
  // 5V / 1024 ADC counts / 144 mV per A = 34 mA per count
  // Could be more like 37/38.. we'll go with 38 to be safe
  //return analogRead(PIN_MOTOR_CS2) * 34;
  Serial.print(digitalRead(PIN_MOTOR_EN2DIAG2));
  Serial.print(":");
  Serial.print(analogRead(PIN_MOTOR_CS2)*38);
  Serial.print("mA;"); 

  Serial.print(digitalRead(PIN_MOTOR_EN1DIAG1));
  Serial.print(":");
  Serial.print(analogRead(PIN_MOTOR_CS1)*38);
  Serial.print("mA;");
 
  //Serial.println("");
}

Ctrl Controller = Ctrl();

