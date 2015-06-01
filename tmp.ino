#include <Logging.h>
#include <Wire.h>
#include <HMC5883L.h>
//#include <Instructor.h>

#define LOGLEVEL LOG_LEVEL_DEBUG

//Instructor Instructor;
HMC5883L compass;

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
/* 
  Conversion of revolutions/ticks to real distance (in cm)
*/
int static DIST_TICK_TO_REAL = 5;
/*
  Adjustment for magnetic declination
  Via instructions: http://www.meccanismocomplesso.org/en/arduino-magnetic-magnetic-magnetometer-hmc5883l/
  Declination angle in Denver is 8 degrees 29 arc minutes; This is 45.96 milliradians;
  declinationAngle is converted to in radians: 0.04596 rads
*/
float static declinationAngle = 45.96/1000; 

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
int PIN_HALL_OUT     = -1;    // Rotation pulse sensing for distance (interrupt)

// Static (for now) Motor Speeds (0-255)
int STEER_SPEED = 255;
int DRIVE_SPEED = 255;

// Sensor values
int input_steer_pot_value = 0;
int input_distance_value = 0;

// State variables
int state_turn_direction;
int state_drive_direction;

void setup() {

	// Initialize Loggins
  Log.Init(LOGLEVEL, 38400L);

  // Open Serial port for logging (38400 baud)
  Serial.begin(38400);

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

  // Set up interrupts
  attachInterrupt(PIN_HALL_OUT, itrpt_rpm, RISING);

  // Create set of instructions to be executed
  //Instructor.add(INSTRUCT_COMMAND_DRIVE_FORWARD, INSTRUCT_DURATION_UNIT_DIST, 200); // Drive forward 200 cm
  //Instructor.add(INSTRUCT_COMMAND_STEER_RIGHT, INSTRUCT_DURATION_UNIT_DIST, ); // Turn right until compass reads north

  // Initialize compass
  // Compass is set on static address so no port information is needed
  while (!compass.begin())
  {
    Serial.println("Could not find a valid HMC5883L sensor, check wiring!");
    delay(500);
  }

  // Set gain/range to 1.3 Ga; If we cannot get full readings, may need to increase this value; valid values are 0.88 Ga, 1.3 Ga, 1.9 Ga, 2.5 Ga, 4.0 Ga, 4.7 Ga, 5.6 Ga, 8.1 Ga
  compass.setRange(HMC5883L_RANGE_1_3GA);

  // Set compass to read continuously
  compass.setMeasurementMode(HMC5883L_CONTINOUS);

  // Set data rate
  compass.setDataRate(HMC5883L_DATARATE_30HZ);

  // Set number of samples averaged
  compass.setSamples(HMC5883L_SAMPLES_8);

  // Set calibration offset. See HMC5883L_calibration.ino
  compass.setOffset(0, 0);  

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


  // Get distance travelled (in cm)
  float distance_travelled = input_distance_value / DIST_TICK_TO_REAL;
  
  // Get compass heading value (in degrees)
  float heading = getHeading();

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
	if (input_steer_pot_value >= POT_LEFT_MAX && state_turn_direction == TURN_LEFT) {
		stop(MOTOR_STEER);
		Log.Info("Maxed Left"CR);
	} else if(input_steer_pot_value <= POT_RIGHT_MAX && state_turn_direction == TURN_RIGHT) {
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

float getHeading() {

  // Normalized data reading (no scale/vector strength, just direction)
  Vector norm = compass.readNormalize();  

  // Calculate heading (convert x,y coordinates to radians)
  float heading = atan2(norm.YAxis, norm.XAxis); 
  
   // Adjust for Denver's magnetic declination (+ because it is East)
  heading += declinationAngle;

  // Correct for heading < 0 deg and heading > 360 deg
  if(heading < 0)
     heading += 2*PI; // add 360 degrees if we're negative
  if(heading > 2*PI)
     heading -= 2*PI; // subtract 360 degrees if we're over 360 degrees

  // Convert from radians to degrees: 0 = North; 90 = East; 180 = South; 270 = West 
  float headingDegrees = heading * 180/M_PI; 

  return headingDegrees;
}

/* 
  Interrupt to count "ticks" of revolution for calculating distance traveled
*/
void itrpt_rpm() {
  input_distance_value = input_distance_value + 1;
}
