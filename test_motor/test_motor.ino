#include <Logging.h>

#define LOGLEVEL LOG_LEVEL_DEBUG

int static MOTOR_STEER 		= 1;
int static MOTOR_DRIVE 		= 2;
int static MOTOR_CW			= 0;
int static MOTOR_CCW		= 1;
int static TURN_LEFT 		= MOTOR_CW;
int static TURN_RIGHT 		= MOTOR_CCW;
int static TURN_STRAIGHT 	= 3;
int static TURN_STOP		= 4;
int static DRIVE_FORWARD 	= MOTOR_CW;
int static DRIVE_REVERSE 	= MOTOR_CCW;
int static DRIVE_STOP 		= 3;

/*
  Potentiometer Edge Values (Right - Left)
  Base: 0 - 1023
  Casing: ~100 - ~910
  Wheels: ~352 - ~630
  Ideal: 380 - 610
*/
int static POT_LEFT_MAX = 610;
int static POT_RIGHT_MAX = 380;

// PIN Assignements
int PIN_STEER_SPEED  = 5;     // Steering Motor Speed Control (PWM)
int PIN_STEER_DIR1   = 9;     // Steering Motor Direction/Input 1
int PIN_STEER_DIR2   = 8;     // Steering Motor Direction/Input 2
int PIN_STEER_STBY   = 10;    // Steering Motor Standby/Sleep
int PIN_STEER_POT    = 0;     // Steering Motor Position Potentiometer
int PIN_DRIVE_SPEED  = 6;     // Drive Motor Speed Control (PWM)
int PIN_DRIVE_DIR1   = 11;    // Drive Motor Direction/Input 1
int PIN_DRIVE_DIR2   = 12;    // Drive Motor Direction/Input 2
int PIN_DRIVE_STBY   = -1;    // Drive Motor Standby/Sleep
int PIN_DRIVE_FB     = 5;     // Drive Motor Current Feedback

// Static (for now) Motor Speeds (0-255)
int STEER_SPEED = 255;
int DRIVE_SPEED = 255;

// Sensor values
int input_steer_pot_value = 0;

// State variables
int state_turn_direction;
int state_drive_direction;

void setup() {

	// Initialize Loggins
  	Log.Init(LOGLEVEL, 38400L);

	// Enable STEERING Controls
	pinMode(PIN_STEER_STBY, OUTPUT);
	pinMode(PIN_STEER_SPEED, OUTPUT);
	pinMode(PIN_STEER_DIR1, OUTPUT);
	pinMode(PIN_STEER_DIR2, OUTPUT);

	// Enable DRIVE Controls
	pinMode(PIN_DRIVE_STBY, OUTPUT);
	pinMode(PIN_DRIVE_SPEED, OUTPUT);
	pinMode(PIN_DRIVE_DIR1, OUTPUT);
	pinMode(PIN_DRIVE_DIR2, OUTPUT);

	// Open Serial port for loggins (38400 baud)
	Serial.begin(38400);
}

void loop() {

  	// Execute current drive/turn commands
  	program();
	drive();
	turn(); 

}

/* 
	Based on time since start (millis()), set state configuration 
*/
void program() {

	// For 5 seconds turn left, for 5 seconds turn right
	if((millis() % 10000) < 5000) {
		state_turn_direction = TURN_LEFT;
	} else {
		state_turn_direction = TURN_RIGHT;
	}

	// For the first two seconds of every 5, drive forward
	if((millis() % 5000) < 2000) {
		state_drive_direction = DRIVE_FORWARD;
	} else {
		state_drive_direction = DRIVE_STOP;
	}

}

void drive() {

	if(state_drive_direction == DRIVE_STOP) {
		stop(MOTOR_DRIVE);
  	} else {
    	move(MOTOR_DRIVE, DRIVE_SPEED, state_drive_direction);
  	}

}

void turn() {
   
  input_steer_pot_value = analogRead(PIN_STEER_POT);    // read the value from the sensor
  
  Log.Info("Wanting to move %d, %d"CR, state_turn_direction, input_steer_pot_value);
  if (potVal >= POT_LEFT_MAX && state_turn_direction == TURN_LEFT) {
    stop(MOTOR_STEER);
    Log.Info("Maxed Left"CR);
  } else if(potVal <= POT_RIGHT_MAX && state_turn_direction == TURN_RIGHT) {
    stop(MOTOR_STEER);
    Log.Info("Maxed Right"CR);
  } else {
     Log.Info("Turning %d"CR, state_turn_direction);
     move(MOTOR_STEER, STEER_SPEED, state_turn_direction);
  }

}

/*
	Motor (Motor ID)
	Speed (0 - 255)
	Direction: (0 = Forward/Left; 1 = Reverse/Right)
*/
void move(int motor, int speed, int direction) {

  boolean inPin1 = LOW;
  boolean inPin2 = HIGH;

  if (direction == MOTOR_CCW) {
    inPin1 = HIGH;
    inPin2 = LOW;
  }

  if (motor == MOTOR_STEER) {
  	digitalWrite(PIN_STEER_STBY, HIGH); // Disable Standby/Sleep
    digitalWrite(PIN_STEER_DIR1, inPin1);
    digitalWrite(PIN_STEER_DIR2, inPin2);
    analogWrite(PIN_STEER_SPEED, speed);
  } else if (motor == MOTOR_DRIVE) {
  	digitalWrite(PIN_DRIVE_STBY, HIGH); // Disable Standby/Sleep
    digitalWrite(PIN_DRIVE_DIR1, inPin1);
    digitalWrite(PIN_DRIVE_DIR2, inPin2);
    analogWrite(PIN_DRIVE_SPEED, speed);
  }
}

/*
	Stop a motor by enabling the Standby/Sleep funciton
*/
void stop(int motor) {
	if (motor == MOTOR_STEER) {
		digitalWrite(PIN_STEER_STBY, LOW); // Enable Standby/Sleep
	} else if (motor == MOTOR_DRIVE) {
		digitalWrite(PIN_DRIVE_STBY, LOW); // Enable Standby/Sleep
	}
}
