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
int PIN_DRIVE_FB     = 5; // Current feedback

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

int static TURN_LEFT = 0;
int static TURN_RIGHT = 1;
int static TURN_STRAIGHT = 3;
int static DRIVE_FORWARD = 0;
int static DRIVE_REVERSE = 1;
int static DRIVE_STOP = 2;


// Steering Motor (Control)
int STEER_SPEED = 255; // 0 - 255

//Steering Position
int steerDir = 0;
int potVal = 0;       // variable to store the value coming from the sensor

// Drive
int driveDir = 0;

// Drive Motor
int DRIVE_SPEED = 255;

// State variables
int state_turn_direction;
int state_drive_direction;

#define LOGLEVEL LOG_LEVEL_DEBUG

void setup() {

  Log.Init(LOGLEVEL, 38400L);
  //Log.Info(CR"******************************************"CR);
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
  Serial.begin(38400);      // open the serial port at 9600 bps:
}

void loop() {

  Log.Info("%d", millis() % 10000);
  if((millis() % 10000) < 5000) {
     state_turn_direction = TURN_LEFT;
  } else {
    state_turn_direction = TURN_RIGHT;
  }
  
  if((millis() % 5000) < 2000) {
    state_drive_direction = DRIVE_FORWARD;
  } else {
    state_drive_direction = DRIVE_STOP;
  }
  
  
  //state_turn_direction = TURN_LEFT; 
  drive();
  turn(); 
  
  int fbVal = analogRead(PIN_DRIVE_FB); // read the value of the feedback current
  if(fbVal != 0) {
  //  Serial.print(fbVal);
   
  }

/*  if (potVal >= POT_LEFT_MAX || potVal <= POT_RIGHT_MAX) {

    //Serial.print("Stopped!");

    if (potVal >= POT_LEFT_MAX) {
      steerDir = 1;
      //Serial.print("1");
      //driveDir = 1;
    } else {
      steerDir = 0;
      driveDir = 0;
    }
  }*/
    /*
  drive(MOVE_FORWARD);
  delay(2000);
  state_turn = TURN_LEFT;
  delay(1000);
  turn(TURN_STRAIGHT);
  delay(2000);
  turn(TURN_RIGHT);
  delay(2000);
  turn(TURN_STRAIGHT);
 delay(500);*/
  
  //drive(MOVE_STOP);
  
  
  //attachInterrupt(0, blink, CHANGE);
  
  //move(MOTOR_STEER, STEER_SPEED, steerDir);
  //move(MOTOR_DRIVE, DRIVE_SPEED, driveDir);
  //Serial.print("Motor: ", DRIVE_SPEED);
  delay(200);  // stop the program for some time
}

void drive() {
  if(state_drive_direction == DRIVE_STOP) {
    move(MOTOR_DRIVE, 0, state_drive_direction);
  } else {
    move(MOTOR_DRIVE, DRIVE_SPEED, state_drive_direction);
  }
}


void turn() {
   
  potVal = analogRead(PIN_STEER_POT);    // read the value from the sensor
  
  Log.Info("Wanting to move %d, %d"CR, state_turn_direction, potVal);
  if (potVal >= POT_LEFT_MAX && state_turn_direction == TURN_LEFT) {
    stop();
    Log.Info("Maxed Left"CR);
  } else if(potVal <= POT_RIGHT_MAX && state_turn_direction == TURN_RIGHT) {
    stop();
    Log.Info("Maxed Right"CR);
  } else {
     Log.Info("Turning %d"CR, state_turn_direction);
     move(MOTOR_STEER, STEER_SPEED, state_turn_direction);
  }
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
