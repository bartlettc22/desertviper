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
  pinMode(_INA1,OUTPUT);
  pinMode(_INB1,OUTPUT);
  pinMode(_PWM1,OUTPUT);
  pinMode(_EN1DIAG1,INPUT);
  #if defined(__AVR_ATmega168__)|| defined(__AVR_ATmega328P__) || defined(__AVR_ATmega32U4__)
  // Timer 1 configuration
  // prescaler: clockI/O / 1
  // outputs enabled
  // phase-correct PWM
  // top of 400
  //
  // PWM frequency calculation
  // 16MHz / 1 (prescaler) / 2 (phase-correct) / 400 (top) = 20kHz
  TCCR1A = 0b10100000;
  TCCR1B = 0b00010001;
  ICR1 = 400;
  #endif
}

void SController::run() {
  
  _pot_value = analogRead(_POT);    // read the value from the sensor

  /*if (input_steer_pot_value >= _POT_LEFT_MAX && state_turn_direction == TURN_LEFT) {
    //stop(MOTOR_STEER);
    move(MOTOR_STEER, 0, state_turn_direction);
    //Log.Info("Maxed Left"CR);
  } else if(input_steer_pot_value <= POT_RIGHT_MAX && state_turn_direction == TURN_RIGHT) {
    //stop(MOTOR_STEER);
    move(MOTOR_STEER, 0, state_turn_direction);
    //Log.Info("Maxed Right"CR);
  } else if(state_turn_direction == TURN_STRAIGHT) {
    //Log.Info("Turning %d"CR, state_turn_direction);
       //move(MOTOR_STEER, STEER_SPEED, state_turn_direction);
       stop(MOTOR_STEER);
  } else {
    move(MOTOR_STEER, STEER_SPEED, state_turn_direction);
  }*/
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
  if (speed > 400)  // Max PWM dutycycle
    speed = 400;
  #if defined(__AVR_ATmega168__)|| defined(__AVR_ATmega328P__) || defined(__AVR_ATmega32U4__)
  OCR1A = speed;
  #else
  analogWrite(_PWM1,speed * 51 / 80); // default to using analogWrite, mapping 400 to 255
  #endif
  if (speed == 0)
  {
    digitalWrite(_INA1,LOW);   // Make the motor coast no
    digitalWrite(_INB1,LOW);   // matter which direction it is spinning.
  }
  else if (reverse)
  {
    digitalWrite(_INA1,LOW);
    digitalWrite(_INB1,HIGH);
  }
  else
  {
    digitalWrite(_INA1,HIGH);
    digitalWrite(_INB1,LOW);
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
  digitalWrite(_INA1, LOW);
  digitalWrite(_INB1, LOW);
  #if defined(__AVR_ATmega168__)|| defined(__AVR_ATmega328P__) || defined(__AVR_ATmega32U4__)
  OCR1A = brake;
  #else
  analogWrite(_PWM1,brake * 51 / 80); // default to using analogWrite, mapping 400 to 255
  #endif
}

SController SteeringController = SController();