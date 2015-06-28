
//How many of the shift registers - change this
#define number_of_74hc595s 1

//do not touch
#define numOfRegisterPins number_of_74hc595s * 8

// Shift Register (74hc595)
#define PIN_SR_SER      11   // Shift Register Serial Data (DS/SER)
#define PIN_SR_SRCLK    12   // Shift Register Serial Clock (SHCP/SRCLK)
#define PIN_SR_RCLK     13   // Shift Register Register Clock (STCP/Latch)

#define SPIN_STEER_INA1        2  // Steering Motor Direction/Input 1
#define SPIN_STEER_INB1        3  // Steering Motor Direction/Input 2
#define SPIN_STEER_EN1DIAG1    1   // Steering Motor Standby/Sleep

bool registers[numOfRegisterPins]; 
bool shift_register_changed = false; // Flagged true if shift register value changed during cycle, triggers writeRegisters();

void setup() {
	// Shift Register
	pinMode(PIN_SR_SER, OUTPUT);
	pinMode(PIN_SR_RCLK, OUTPUT);
	pinMode(PIN_SR_SRCLK, OUTPUT);
	clearRegisters(); // Set all registers to LOW
	writeRegisters(); // Save changes
}

void run() {
	// If a register was changed during this cycle (by calling setRegisterPin()), save it
	if(shift_register_changed == true) {
		writeRegisters();
		shift_register_changed = false;
	}
}

void turn() {
  // Default Brake
  int _INA = LOW;
  int _INB = LOW;
  int _speed = round(speed * 255);

  // Amount of turn is based on potentiometer reading

  if(direction == TURN_DIRECTION_LEFT) {
    _INA = LOW;
    _INB = HIGH;
  } else if(direction == TURN_DIRECTION_RIGHT) {
    _INA = HIGH;
    _INB = LOW;    
  } else if(direction == TURN_DIRECTION_BRAKE) {
    _INA = LOW;
    _INB = LOW;
    // _INA = HIGH;
    // _INB = HIGH;
  }

  analogWrite(PIN_STEER_PWM, _speed);

  //digitalWrite(SPIN_STEER_INA1, _INA);
  //digitalWrite(SPIN_STEER_INB1, _INB);
  setRegisterPin(SPIN_STEER_INA1, _INA);
  setRegisterPin(SPIN_STEER_INB1, _INB);
}

//set all register pins to LOW
void Ctrl::clearRegisters(){
  for(int i = numOfRegisterPins - 1; i >=  0; i--){
     registers[i] = LOW;
  }
}

// Set and display registers
// Only call AFTER all values are set how you would like (slow otherwise)
void Ctrl::writeRegisters(){

  digitalWrite(PIN_SR_RCLK, LOW);

  for(int i = numOfRegisterPins - 1; i >=  0; i--){
    digitalWrite(PIN_SR_SRCLK, LOW);

    int val = registers[i];

    digitalWrite(PIN_SR_SER, val);
    digitalWrite(PIN_SR_SRCLK, HIGH);

  }
  digitalWrite(PIN_SR_RCLK, HIGH);

}

//set an individual pin HIGH or LOW
void Ctrl::setRegisterPin(int index, int value){
  registers[index] = value;
  shift_register_changed = true;
}