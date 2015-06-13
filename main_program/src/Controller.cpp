#include "Controller.h"

void Ctrl::init()
{
  
  MotorController.init(PIN_MOTOR_INA1, PIN_MOTOR_INB1, PIN_MOTOR_EN1DIAG1, PIN_MOTOR_PWM1, PIN_MOTOR_CS1, PIN_MOTOR_INA2, PIN_MOTOR_INB2, PIN_MOTOR_EN2DIAG2, PIN_MOTOR_PWM2, PIN_MOTOR_CS2);
  SteeringController.init(PIN_STEER_INA1, PIN_STEER_INB1, PIN_STEER_EN1DIAG1, PIN_STEER_PWM, PIN_STEER_POT);
  HallSensor.init(PIN_HALL, INRPT_HALL);
  Compass.init();
  RangeSensor.init(PIN_RANGE_TRIG, PIN_RANGE_ECHO);
  pinMode(PIN_LED, OUTPUT);

}

void Ctrl::run()
{

  // Pin cycle indicator
  if(cycleCount % CYCLE_MOD == 0) {
    if(ledState == LOW) {
      ledState = HIGH;
    } else{
      ledState = LOW;
    }
    digitalWrite(PIN_LED, ledState);
  }

  HallSensor.run();
  Compass.run();
  RangeSensor.run();
  SteeringController.run();
  MotorController.run();
  
}

void Ctrl::turn(int Direction, float Amount, float Speed)
{
  SteeringController.turn(Direction, Amount, Speed);
}

void Ctrl::finish()
{
  // Turn off LED
  digitalWrite(PIN_LED, LOW);
}

