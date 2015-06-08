#include "Instruction.h"

Instruction::Instruction(Ctrl &Controller, int type, int duration)
{
  _Controller = Controller;
  _type = type;
  _duration = duration;
}

void Instruction::init() {
	switch(_type) {
		case TYPE_TURN_LEFT:
			_Controller.turn(TURN_DIRECTION_LEFT, 1, 1);
			break;

		case TYPE_TURN_RIGHT:
			_Controller.turn(TURN_DIRECTION_RIGHT, 1, 1);
			break;
	}	

	if(_duration > 0) {
		startTime = millis();
	}
}

void Instruction::run() {
	if(millis() - startTime >= _duration) {
		isDone = true;
	}
}

bool Instruction::IsDone()
{
  return isDone;
}