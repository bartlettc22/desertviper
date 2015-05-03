#include <Logging.h>

// PIN Assignements
int PIN_STEER_SPEED  = 5;     // Steering Speed Control (PWM)
int PIN_STEER_DIR1   = 9;     // Steering Direction 1
int PIN_STEER_DIR2   = 8;     // Steering Direction 2
int PIN_STEER_STBY   = 10;    // Steering Standby
int PIN_STEER_POT    = 0;     // Steering Position Potentiometer

// Drive Motor (PINs)
int PWM_DRIVE = 6;    //Speed control
int AIN1_DRIVE = 11;  //Direction
int AIN2_DRIVE = 12;  //Direction

/*
  Potentiometer Edge Values (Right - Left)
  Base: 0 - 1023
  Casing: ~100 - ~910
  Wheels: ~352 - ~630
  Ideal: 380 - 610
*/
int static POT_LEFT_MAX = 610;
int static POT_RIGHT_MAX = 380;

int static MOTOR_STEER = 1;
int static MOTOR_DRIVE = 2;

// Steering Motor (Control)
int STEER_SPEED = 150; // 0 - 255

//Steering Position
int steerDir = 0;
int potVal = 0;       // variable to store the value coming from the sensor

// Drive
int driveDir = 0;

// Drive Motor
int DRIVE_SPEED = 255;

void setup() {

  pinMode(PIN_STEER_STBY, OUTPUT);

  // Enable STEERING Controls
  pinMode(PIN_STEER_SPEED, OUTPUT);
  pinMode(PIN_STEER_DIR1, OUTPUT);
  pinMode(PIN_STEER_DIR2, OUTPUT);

  // Enable DRIVE Controls
  pinMode(PWM_DRIVE, OUTPUT);
  pinMode(AIN1_DRIVE, OUTPUT);
  pinMode(AIN2_DRIVE, OUTPUT);

  // Debug
  Serial.begin(9600);      // open the serial port at 9600 bps:
}

void loop() {

  potVal = analogRead(PIN_STEER_POT);    // read the value from the sensor

  if (potVal >= POT_LEFT_MAX || potVal <= POT_RIGHT_MAX) {

    //Serial.print("Stopped!");

    if (potVal >= POT_LEFT_MAX) {
      //steerDir = 1;
      //driveDir = 1;
    } else {
      //steerDir = 0;
      driveDir = 0;
    }
  }

  //move(MOTOR_STEER, STEER_SPEED, steerDir); //motor 1, full speed, left
  move(MOTOR_DRIVE, DRIVE_SPEED, driveDir);
  //Serial.print("Motor: ", DRIVE_SPEED);
  delay(20);  // stop the program for some time
}

void move(int motor, int speed, int direction) {

  //Move specific motor at speed and direction
  //speed: 0 is off, and 255 is full speed
  //direction: 0 clockwise, 1 counter-clockwise

  digitalWrite(PIN_STEER_STBY, HIGH); //disable standby

  boolean inPin1 = LOW;
  boolean inPin2 = HIGH;

  if (direction == 1) {
    inPin1 = HIGH;
    inPin2 = LOW;
  }

  if (motor == MOTOR_STEER) {
    digitalWrite(PIN_STEER_DIR1, inPin1);
    digitalWrite(PIN_STEER_DIR2, inPin2);
    analogWrite(PIN_STEER_SPEED, speed);
  } else if (motor == MOTOR_DRIVE) {
    digitalWrite(AIN1_DRIVE, inPin1);
    digitalWrite(AIN2_DRIVE, inPin2);
    analogWrite(PWM_DRIVE, speed);
  }
}

void stop() {
  //enable standby
  digitalWrite(PIN_STEER_STBY, LOW);
}
