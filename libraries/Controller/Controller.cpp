#include "Controller.h"

void Ctrl::init()
{
  
  MotorController.init(PIN_MOTOR_INA1, PIN_MOTOR_INB1, PIN_MOTOR_EN1DIAG1, PIN_MOTOR_PWM1, PIN_MOTOR_CS1, PIN_MOTOR_INA2, PIN_MOTOR_INB2, PIN_MOTOR_EN2DIAG2, PIN_MOTOR_PWM2, PIN_MOTOR_CS2);
  SteeringController.init(PIN_STEER_INA1, PIN_STEER_INB1, PIN_STEER_EN1DIAG1, PIN_STEER_PWM, PIN_STEER_POT);
  HallSensor.init(PIN_HALL, INRPT_HALL);
  Compass.init();
  RangeSensor.init(PIN_RANGE_TRIG, PIN_RANGE_ECHO);

}

void Ctrl::run()
{

  HallSensor.run();
  Compass.run();
  RangeSensor.run();
  SteeringController.run();
  MotorController.run();
  
}