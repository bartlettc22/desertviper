#include "Program.h"

/*
  Set up all inputs/outputs
*/
void Prog::init()
{

  _Controller = Ctrl();

  //_stack.push(Instruction(Instruction::TYPE_DRIVE));
  // stack[0].set(Instruction::TYPE_DRIVE);
  _stack.push(Instruction(_Controller, Instruction::TYPE_STOP, 2000));
  _stack.push(Instruction(_Controller, Instruction::TYPE_TURN_LEFT, 2000));
  _stack.push(Instruction(_Controller, Instruction::TYPE_TURN_RIGHT, 2000));
  _stack.push(Instruction(_Controller, Instruction::TYPE_TURN_LEFT, 2000));
  _stack.push(Instruction(_Controller, Instruction::TYPE_TURN_RIGHT, 2000));
  _stack.push(Instruction(_Controller, Instruction::TYPE_TURN_LEFT, 2000));

  _currentInstruction = _stack.pop();
}

/*
  Runs the main program
*/
void Prog::run()
{

  /*
    Check to see if the current instruction is complete.  If so, advance to next instruction.
  */
  if(_currentInstruction.IsDone() == true) {
    if(_stack.isEmpty() == false) {
      _currentInstruction = _stack.pop();
      _currentInstruction.init();
    } else {
      isDone = true;
    }
  }

  /* 
    Commands:

    drive; input speed (%), keeps heading
    turn; input rate (%); input direction
    stop
    coast
  */

  /*
    Command termination conditions:

    time, time in seconds
    distance, distance in cm
    heading_diff, heading difference in degrees,
    sensor condition; sensor; value
  */

  // Set controller commands to be run

  //float heading = Compass.getHeading();  
  /*
  float heading_error = INITIAL_HEADING - heading;    

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

  if(input_distance_value <= 60) {
    state_drive_direction = DRIVE_FORWARD;
  } else {
    state_drive_direction = DRIVE_STOP;
  }*/

  // Execute any I/O commands
  if(isDone == false) {
    _currentInstruction.run();
    _Controller.run();
  } else {
    //_Controller.finish();
  }

}








