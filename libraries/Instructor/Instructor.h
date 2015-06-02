#ifndef INSTRUCTOR_H
#define INSTRUCTOR_H
 
#include <WProgram.h>
 
const int dummy = 0;
 
#define INSTRUCT_COMMAND_DRIVE_FORWARD 0
#define INSTRUCT_COMMAND_DRIVE_REVERSE 1
#define INSTRUCT_COMMAND_DRIVE_STOP 2
#define INSTRUCT_COMMAND_STEER_LEFT 3
#define INSTRUCT_COMMAND_STEER_RIGHT 4
#define INSTRUCT_COMMAND_STEER_STRAIGHT 5

#define INSTRUCT_DURATION_UNIT_DIST 0
#define INSTRUCT_DURATION_UNIT_TIME 1


class Instructor {

public:
        Instructor();
        ~Instructor();
        void add();
        //void off();
        //void blink(int time);
};
 
#endif
