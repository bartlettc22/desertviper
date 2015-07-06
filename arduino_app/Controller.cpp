#include "Controller.h"

void RF_interrupt() {
  Controller._RF_A = digitalRead(PIN_RF_A);
  Controller._RF_B = digitalRead(PIN_RF_B);
}

void HSensor_interrupt() {
  Controller._ticks++;
}

void Ctrl::init()
{

  // Steering
  pinMode(PIN_STEER_PWM, OUTPUT);
  pinMode(PIN_STEER_POT, INPUT);

  // Main Motor Controls
  pinMode(PIN_MOTOR_FRONT_INA, OUTPUT);
  pinMode(PIN_MOTOR_FRONT_INB, OUTPUT);
  pinMode(PIN_MOTOR_FRONT_PWM, OUTPUT);
  pinMode(PIN_MOTOR_FRONT_ENDIAG, INPUT);
  pinMode(PIN_MOTOR_FRONT_CS, INPUT);
  pinMode(PIN_MOTOR_REAR_INA, OUTPUT);
  pinMode(PIN_MOTOR_REAR_INB, OUTPUT);
  pinMode(PIN_MOTOR_REAR_PWM, OUTPUT);
  pinMode(PIN_MOTOR_REAR_ENDIAG, INPUT);
  pinMode(PIN_MOTOR_REAR_CS, INPUT);  

  // Initialize Motors Braked
  digitalWrite(PIN_MOTOR_FRONT_INA, LOW);
  digitalWrite(PIN_MOTOR_FRONT_INB, LOW);
  digitalWrite(PIN_MOTOR_REAR_INA, LOW);
  digitalWrite(PIN_MOTOR_REAR_INB, LOW);  

  // Initialize Hall Effect Sensor Interrupt
  attachInterrupt(INRPT_HALL, HSensor_interrupt, CHANGE);
  //HallSensor.init(PIN_HALL, INRPT_HALL);

  // Initialize RF Receiver Interrupts
  //pinMode(PIN_RF_A, INPUT);
  //pinMode(PIN_RF_B, INPUT);
  attachInterrupt(INRPT_RF, RF_interrupt, CHANGE);
  //attachInterrupt(INRPT_RF_A, RF_A_interrupt, CHANGE);
  //attachInterrupt(INRPT_RF_B, RF_B_interrupt, CHANGE);

  // Initialize I2C Communication
  Wire.begin();

  // Initialize Accelerometer
  accelgyro.initialize();
  accelgyro.setI2CBypassEnabled(true);

  // Initialize Compass
  compass.initialize();

  // Pressure Sensor 
  pressure.begin();
  getPressure(); // Get pressure for baseline
  _baselinePressure = _pressure;

  // Initialize Range Finder, sent Trigger to LOW (initial) state
  pinMode(PIN_RANGE_TRIG, OUTPUT);
  pinMode(PIN_RANGE_ECHO, INPUT); 
  digitalWrite(PIN_RANGE_TRIG, LOW);
  calcRangeTimeout(); 
}

void Ctrl::run()
{

  // Check Kill (B+C)
  if(digitalRead(PIN_RF_B) == HIGH && digitalRead(PIN_RF_C) == HIGH) {
    MotorControl(MOTOR_FRONT, MOTOR_DIRECTION_BRAKE_LOW, 255);
    MotorControl(MOTOR_REAR, MOTOR_DIRECTION_BRAKE_LOW, 255);
    Turn(MOTOR_DIRECTION_BRAKE_LOW, 255);
    setKill = true;
  } else if(setKill == false || (digitalRead(PIN_RF_A) && digitalRead(PIN_RF_D))) {
    // Check on Steering
    checkSteering();

    // Check on Motors
    checkDrive();

    setKill = false;
  }
}

void Ctrl::checkSteering() {

  _potValue = analogRead(PIN_STEER_POT);
  //Serial.println(_potValue);
  
  // If we're within x of goal, stop motors to lock wheels in this position
  if (_turningDirection == MOTOR_DIRECTION_CW) { // Right
    if(_potValue < (_turnRightGoal - __potMaintainTolerance)) { 

      // We're past goal, apply brakes
      Turn(MOTOR_DIRECTION_BRAKE_LOW,_turningSpeed);
    } else if (_potValue > (_turnRightGoal + __potMaintainTolerance)) {
      // We haven't made it yet, keep trying
      Turn(MOTOR_DIRECTION_CW, _turningSpeed);
    }
  } else if (_turningDirection == MOTOR_DIRECTION_CCW) { // Left

    if(_potValue > (_turnLeftGoal + __potMaintainTolerance)) { 
      _turningStatus = "Turning Left beyond goal";
      // We're past goal, apply brakes
      Turn(MOTOR_DIRECTION_BRAKE_LOW, _turningSpeed);
    } else if (_potValue < (_turnLeftGoal - __potMaintainTolerance)) {
      // We haven't made it yet, keep trying
      Turn(MOTOR_DIRECTION_CCW, _turningSpeed);
    }
  } else {
    Turn(_turningDirection, _turningSpeed);
  }
    //_potValue >= (_potGoal - 10) && _potValue <= (_potGoal + 10)) {
    //setMBrake(400); // Full Brake to hold turn
    //setSteerBrake();
    //Serial.println("Stopping, within goal");

 // } else if(_potValue < _potGoal) {
    //Serial.println("Stopping, within goal");
  //  //setMSpeed(-1 * (stateSpeed * 400));
  //} else if(_potValue > _potGoal) {
    //Serial.println("Stopping, within goal");
    //setMSpeed(-1 * (stateSpeed * 400));  
  //}
}


void Ctrl::Turn(unsigned char direction, int speed) {
  
  // Default Brake
  int _INA = LOW;
  int _INB = LOW;
  //int _speed = round(speed * 255);

  

  // Amount of turn is based on potentiometer reading

  if(direction == MOTOR_DIRECTION_CCW) { // LEFT
    //_turnLeftGoal = _potCenter + (amount*(_POT_LEFT_MAX - _potCenter));
    _INA = LOW;
    _INB = HIGH;
  } else if(direction == MOTOR_DIRECTION_CW) { // RIGHT
    //_turnRightGoal = _potCenter - (amount*(_potCenter - _POT_RIGHT_MAX));
    // Right
    _INA = HIGH;
    _INB = LOW;    
  } else if(direction == MOTOR_DIRECTION_BRAKE_LOW) {
    _INA = LOW;
    _INB = LOW;
  }

  analogWrite(PIN_STEER_PWM, speed);
  digitalWrite(PIN_STEER_INA1, _INA);
  digitalWrite(PIN_STEER_INB1, _INB);
}

void Ctrl::MotorControl(unsigned char motor_index, unsigned char direction, int speed) {
  
  // Default Brake
  int _INA = LOW;
  int _INB = LOW;

  if(direction == MOTOR_DIRECTION_CW) {
    _INA = HIGH;
    _INB = LOW;
  } else if(direction == MOTOR_DIRECTION_CCW) {
    _INA = LOW;
    _INB = HIGH;
  } else if(direction == MOTOR_DIRECTION_BRAKE_LOW) {
    _INA = LOW;
    _INB = LOW;
  } else if(direction == MOTOR_DIRECTION_BRAKE_HIGH) {
    _INA = HIGH;
    _INB = HIGH;
  }

  analogWrite(__motorPWMPin[motor_index], speed);
  digitalWrite(__motorINAPIN[motor_index], _INA);
  digitalWrite(__motorINBPIN[motor_index], _INB);
}

void Ctrl::Drive(unsigned char motor, unsigned char direction, float speed)
{
  // Convert % into analog value
  int _speed = round(speed * 255);

  // If both, set each one individually
  if(motor == MOTOR_BOTH) {
    Drive(MOTOR_FRONT, direction, speed);
    Drive(MOTOR_REAR, direction, speed);
  } else {
    __motorGoalSpeed[motor] = _speed;
    __motorDirection[motor] = direction;
    __motorAcc[motor] = (__motorRamp[motor] / 255.0)*1000;
    __motorAccTime[motor] = micros();    
  }

  // Calculate new acceleration


  /*
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
    _frontGoalSpeed = _speedGoal;
  } 

  if(motor == DRIVE_MOTOR_REAR || motor == DRIVE_MOTOR_BOTH) {
    analogWrite(PIN_MOTOR_PWM1, _speed);
    digitalWrite(PIN_MOTOR_INA1, _INA);
    digitalWrite(PIN_MOTOR_INB1, _INB);
    _rearGoalSpeed = _speedGoal;
  }*/
}

void Ctrl::checkDrive() {

  // Check Drive front motors to see if speed adjustments are needed
  for (int i = 0; i < 2; i++) {

    if(__motorDirection[i] == MOTOR_DIRECTION_CW || __motorDirection[i] == MOTOR_DIRECTION_CCW) {
      // Accelerating, aim to increase speed
      if(__motorCurrentSpeed[i] < __motorGoalSpeed[i]) {
        if(__motorAcc[i] == 0.0) {
          MotorControl(i, __motorDirection[i], __motorGoalSpeed[i]);
          __motorCurrentSpeed[i] = __motorGoalSpeed[i];
        } else {
          __motorCurrentSpeed[i] = min(255, round((micros() - __motorAccTime[i]) / __motorAcc[i]));
          MotorControl(i, __motorDirection[i], __motorCurrentSpeed[i]);
        }
      }
    } 

    else {
      // Decelerating, aim to decrease speed
      MotorControl(i, __motorDirection[i], __motorGoalSpeed[i]);
      __motorCurrentSpeed[i] = 0.0;
    }


  }
  
  // Record fault status
  _frontFault = !digitalRead(PIN_MOTOR_FRONT_ENDIAG);
  _rearFault = !digitalRead(PIN_MOTOR_REAR_ENDIAG);

  // Record electrical current readings
  _frontCurrent = analogRead(PIN_MOTOR_FRONT_CS)*38;  
  _rearCurrent = analogRead(PIN_MOTOR_REAR_CS)*38;
}


void Ctrl::CalibrateSteering() {
  Turn(MOTOR_DIRECTION_CCW, 1);
  delay(1000);
  _POT_LEFT_MAX = analogRead(PIN_STEER_POT);
  Turn(MOTOR_DIRECTION_BRAKE_LOW, 1);
  delay(1000);
  Turn(MOTOR_DIRECTION_CW, 1); // Right
  delay(1000);
  _POT_RIGHT_MAX = analogRead(PIN_STEER_POT);
  Turn(MOTOR_DIRECTION_BRAKE_LOW, 1);  

  //_potCenter = (_POT_LEFT_MAX + _POT_RIGHT_MAX) / 2;
  //_turnAmountPot = (_POT_LEFT_MAX - _potCenter); // Potentiometer value (to add to center) for 100% turn

}

void Ctrl::getRange() {
  digitalWrite(PIN_RANGE_TRIG, HIGH);
  delayMicroseconds(2000); // Added this line
  digitalWrite(PIN_RANGE_TRIG, LOW);
  _rangeDuration = pulseIn(PIN_RANGE_ECHO, HIGH, __rangeTimeout);

   _rangeDistance = (_rangeDuration/2.0) / (1000000.0/__speedOfSound); // (_rangeDuration/2) / 29.1;
  if(_rangeDuration == 0 || _rangeDistance < __rangeMin || _rangeDistance > __rangeMax) {
    _rangeDistance = -1.0;
  } 
}

void Ctrl::calcRangeTimeout() {
  // Based on speed of sound and max range, calculate timeout
  // Range * 2 to account for travel to and from object
  // Speed of sound divided by 1000000 to convert to microseconds  
  __rangeTimeout = (__rangeMax*2.0)/(__speedOfSound/1000000.0); // microseconds
}

void Ctrl::getHeading() {

  compass.getHeading(&_headingX, &_headingY, &_headingZ);
  
  // To calculate heading in degrees. 0 degree indicates North
  _heading = atan2(_headingY, _headingX);
  if(_heading < 0)
    _heading += 2 * M_PI;
  _heading = (_heading * 180/M_PI) + __degreeCorrection;
}

float Ctrl::distanceTraveled() 
{
  // Get distance travelled (in cm)
  float distance_in_cm = _ticks * __tickDistance;
  return distance_in_cm;
}

void Ctrl::readAccelGyro() {

  // Raw Values
  accelgyro.getMotion6(&_ax, &_ay, &_az, &_gx, &_gy, &_gz);

}

void Ctrl::getPressure() {  

  // Pressure Calcs
  char status;

  // You must first get a temperature measurement to perform a pressure reading.
  
  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.
  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Use '&T' to provide the address of T to the function.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(_temperature);
    if (status != 0)
    {
      _temperatureF = (9.0/5.0)*_temperature+32.0;

      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Use '&P' to provide the address of P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(_pressure,_temperature);
        if (status != 0)
        {
          _pressureHG = _pressure*0.0295333727;

          // The pressure sensor returns abolute pressure, which varies with altitude.
          // To remove the effects of altitude, use the sealevel function and your current altitude.
          // This number is commonly used in weather reports.
          // Parameters: P = absolute pressure in mb, ALTITUDE = current altitude in m.
          // Result: p0 = sea-level compensated pressure in mb

          _pressure0 = pressure.sealevel(_pressure,__altitude); // we're at 1655 meters (Boulder, CO)
          _pressure0HG = _pressure0*0.0295333727;

          // On the other hand, if you want to determine your altitude from the pressure reading,
          // use the altitude function along with a baseline pressure (sea-level or other).
          // Parameters: P = absolute pressure in mb, p0 = baseline pressure in mb.
          // Result: a = altitude in m.

          _altitudeDiff = pressure.altitude(_pressure,_baselinePressure);
        }
        //else Serial.println("error retrieving pressure measurement\n");
      }
     // else Serial.println("error starting pressure measurement\n");
    }
    //else Serial.println("error retrieving temperature measurement\n");
  }
  //else Serial.println("error starting temperature measurement\n");
}


Ctrl Controller = Ctrl();

