#include "SteeringController.h"

// Public Methods //////////////////////////////////////////////////////////////
void SController::init(unsigned char INA1, unsigned char INB1, unsigned char EN1DIAG1, unsigned char PWM1, unsigned char POT)
{

  // Set PINs
  _INA1 = INA1;
  _INB1 = INB1;
  _EN1DIAG1 = EN1DIAG1;
  _PWM1 = PWM1;
  _POT = POT;

  // Define pinMode for the pins and set the frequency for timer1.
  //pinMode(_INA1,OUTPUT);
  //pinMode(_INB1,OUTPUT);
  pinMode(_PWM1,OUTPUT);
  //pinMode(_EN1DIAG1,INPUT);
  pinMode(_POT,INPUT);

  _potCenter = (_POT_LEFT_MAX + _POT_RIGHT_MAX) / 2;
}

void SController::run() {
  
  _potValue = analogRead(_POT);    // read the value from the sensor

  // If we're within x of goal, stop motors to lock wheels in this position
  /*if ((_potValue >= (_statePotGoal - 10) && _potValue <= (_statePotGoal + 10)) {
    setMBrake(400); // Full Brake to hold turn
  } else if(_potValue < _statePotGoal) {
    setMSpeed(-1 * (stateSpeed * 400));
  } else if(_potValue > _statePotGoal) {
    setMSpeed(-1 * (stateSpeed * 400));  
  }*/
  if(command_changed == true) {
    
    command_changed = false;
  }
}

void SController::turn(int Direction, float Amount, float Speed) {
  _stateDirection = Direction;
  
  _stateAmount = Amount; // Changes _potGoal
  _stateSpeed =(int) (Speed*255); // 0-1 translates to 0-255

  Serial.print(_stateSpeed);
  Serial.print(";");
  Serial.println("");
  command_changed = true;
  /*if(_stateDirection == TURN_DIRECTION_LEFT) {
    _statePotGoal = ((int) round(Amount * (_POT_LEFT_MAX - _potCenter))) + _potCenter;
  } else if(_stateDirection == TURN_DIRECTION_RIGHT) {
    _statePotGoal = _potCenter - ((int) round(Amount * (_potCenter - _POT_RIGHT_MAX))) + ;
  } */ 
}

// Set speed for motor, speed is a number betwenn -400 and 400
void SController::setMSpeed(int speed)
{
  unsigned char reverse = 0;
  
  if (speed < 0)
  {
    speed = -speed;  // Make speed a positive quantity
    reverse = 1;  // Preserve the direction
  }
  if (speed > 400)
  {  // Max PWM dutycycle
    speed = 400;
  }

  analogWrite(_PWM1,speed * 51 / 80); // default to using analogWrite, mapping 400 to 255
  if (speed == 0) // Coast
  {
    //digitalWrite(_INA1,LOW);   // Make the motor coast no
    Controller.setRegisterPin(_INA1, LOW)
    //digitalWrite(_INB1,LOW);   // matter which direction it is spinning.
    Controller.setRegisterPin(_INB1, LOW);
  }
  else if (reverse)
  {
    //digitalWrite(_INA1,LOW);
    //digitalWrite(_INB1,HIGH);
    Controller.setRegisterPin(_INA1, LOW);
    Controller.setRegisterPin(_INB1, HIGH);
  }
  else
  {
    //digitalWrite(_INA1,HIGH);
    //digitalWrite(_INB1,LOW);
    Controller.setRegisterPin(_INA1, HIGH);
    Controller.setRegisterPin(_INB1, LOW);
  }
}

// Brake motor, brake is a number between 0 and 400
void SController::setMBrake(int brake)
{
  // normalize brake
  if (brake < 0)
  {
    brake = -brake;
  }
  if (brake > 400)  // Max brake
    brake = 400;
  //digitalWrite(_INA1, LOW);
  //digitalWrite(_INB1, LOW);
  Controller.setRegisterPin(_INA1, LOW);
  Controller.setRegisterPin(_INB1, LOW);  
  analogWrite(_PWM1,brake * 51 / 80); // default to using analogWrite, mapping 400 to 255
}

SController SteeringController = SController();





