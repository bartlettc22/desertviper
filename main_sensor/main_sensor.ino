#include <Wire.h>
#include <HMC5883L.h>

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
int static DRIVE_REVERSE 	= MOTOR_CW;
int static DRIVE_STOP 		= 3;

/*
  Potentiometer Edge Values (Right - Left)
  Base: 0 - 1023
  Casing: ~100 - ~910
  Wheels: ~352 - ~630
  Ideal: 380 - 610
*/
//int static POT_LEFT_MAX = 610;
//int static POT_RIGHT_MAX = 380; // 329  636
int static POT_LEFT_MAX = 510;
int static POT_RIGHT_MAX = 480;
/* 
  Conversion of revolutions/ticks to real distance (in cm)
*/
int static DIST_TICK_TO_REAL = 9; // 533 cm = 60 ticks
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
//int PIN_DRIVE_STBY   = -1;    // Drive Motor Standby/Sleep
int PIN_DRIVE_FB     = 5;     // Drive Motor Current Feedback
int PIN_HALL_OUT     = 0;    // This is the interrupt # not the pin number
int PIN_RANGE_TRIG   = 3;
int PIN_RANGE_ECHO   = 4;

// Static (for now) Motor Speeds (0-255)
int STEER_SPEED = 255;
int DRIVE_SPEED = 255;
float INITIAL_HEADING = 0;
int HEADING_TOLERANCE = 0.25;

// Sensor values
int input_steer_pot_value = 0;
volatile int input_distance_value = 0;

// State variables
int state_turn_direction;
int state_drive_direction;

void setup() {
   // Open Serial port for logging (38400 baud)
  //Serial.begin(9600);

  // Enable STEERING Controls
  pinMode(PIN_STEER_STBY, OUTPUT);
  pinMode(PIN_STEER_SPEED, OUTPUT);
  pinMode(PIN_STEER_DIR1, OUTPUT);
  pinMode(PIN_STEER_DIR2, OUTPUT);

  // Enable DRIVE Controls
  //pinMode(PIN_DRIVE_STBY, OUTPUT);
  pinMode(PIN_DRIVE_SPEED, OUTPUT);
  pinMode(PIN_DRIVE_DIR1, OUTPUT);
  pinMode(PIN_DRIVE_DIR2, OUTPUT);

  // Range sensor
  pinMode(PIN_RANGE_TRIG, OUTPUT);
  pinMode(PIN_RANGE_ECHO, INPUT);

  // Set up interrupts
  attachInterrupt(PIN_HALL_OUT, itrpt_rpm, CHANGE);

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

  INITIAL_HEADING = getHeading(); 
  delay(2000);
  Serial.print("Initial Heading: ");
  Serial.println(INITIAL_HEADING);

  state_drive_direction = DRIVE_STOP;
  input_distance_value = 0;
}

void loop() {

//state_drive_direction = DRIVE_FORWARD;	
  program();
  drive();
  turn();
//move(MOTOR_DRIVE, DRIVE_SPEED, DRIVE_FORWARD);
//delay(5);
//Serial.print(digitalRead(2));
}

void program() {

  // Get distance travelled (in cm)
  float distance_travelled = input_distance_value / DIST_TICK_TO_REAL;

// Get compass heading value (in degrees; 0 = North; 90 = West, etc.)
  float heading = getHeading();  
  
  // Get rangefinder range (in cm)
  //long range = get_range();

  float heading_error = INITIAL_HEADING - heading;
  
  //Serial.print(heading);
  //Serial.print(" : ");
  //Serial.println(heading_error);

  if (heading_error >= 180) {
	heading_error -= 360;
  } else if (heading_error <= -180) {
    heading_error += 360;
  }
     
  if(heading_error < 0 && heading_error < (-1*HEADING_TOLERANCE)) {
  	// Turn left
  	state_turn_direction = TURN_LEFT;
  	//Serial.println("Turning left to correct");
  } else if (heading_error > 0 && heading_error > HEADING_TOLERANCE) {
  	// Turn right
  	state_turn_direction = TURN_RIGHT;
  	//Serial.println("Turning right to correct");
  } else {
  	// Straighten up
  	state_turn_direction = TURN_STRAIGHT;
  	//Serial.println("Running Straight");
  }

  //Serial.print("Range:");
  //Serial.println(range);

  /*if(range < 30) {
    state_drive_direction = DRIVE_STOP;
  } else {
  	state_drive_direction = DRIVE_FORWARD;
  }*/

  if(input_distance_value <= 60) {
  	state_drive_direction = DRIVE_FORWARD;
  } else {
  	state_drive_direction = DRIVE_STOP;
  }

  //Serial.print("Ticks:");
  //Serial.println(input_distance_value);

  // Get compass heading value (in degrees)
  //float heading = getHeading();

	// For 5 seconds turn left, for 5 seconds turn right
	/*if((millis() % 10000) < 5000) {
		state_turn_direction = TURN_LEFT;
	} else {
		state_turn_direction = TURN_RIGHT;
	}

	// For the first two seconds of every 5, drive forward
	if((millis() % 5000) < 2000) {
		state_drive_direction = DRIVE_FORWARD;
	} else {
		state_drive_direction = DRIVE_STOP;
	}*/

}

void turn() {
   
	input_steer_pot_value = analogRead(PIN_STEER_POT);    // read the value from the sensor
//Serial.println(input_steer_pot_value);
	//Log.Info("Wanting to move %d, %d"CR, state_turn_direction, input_steer_pot_value);
	if (input_steer_pot_value >= POT_LEFT_MAX && state_turn_direction == TURN_LEFT) {
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
	}

}

void drive() {

  if(state_drive_direction == DRIVE_STOP) {
    //stop(MOTOR_DRIVE);
    move(MOTOR_DRIVE, 0, state_drive_direction);
  } else {
    move(MOTOR_DRIVE, DRIVE_SPEED, state_drive_direction);
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
		//digitalWrite(PIN_DRIVE_STBY, HIGH); // Disable Standby/Sleep
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
		//digitalWrite(PIN_DRIVE_STBY, LOW); // Enable Standby/Sleep
	}
}

/* 
  Interrupt to count "ticks" of revolution for calculating distance traveled
*/
void itrpt_rpm() {
  input_distance_value = input_distance_value + 1;
  //Serial.println("hey!!!");
}

long get_range() {
  long duration, distance;
  //digitalWrite(PIN_RANGE_TRIG, LOW);  // Added this line
  //delayMicroseconds(2); // Added this line
  digitalWrite(PIN_RANGE_TRIG, HIGH);
  delayMicroseconds(2000); // Added this line
  digitalWrite(PIN_RANGE_TRIG, LOW);
  duration = pulseIn(PIN_RANGE_ECHO, HIGH);
  distance = (duration/2) / 29.1;

  //Serial.println(distance);
//delay(500);
  return distance;
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