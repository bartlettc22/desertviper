#ifndef SteeringController_h
#define SteeringController_h

#include <Arduino.h>
#include <math.h>

#define TURN_DIRECTION_STRAIGHT 0
#define TURN_DIRECTION_LEFT     1
#define TURN_DIRECTION_RIGHT    2
#define TURN_DIRECTION_COAST    3

class SController
{
  public:  
    // CONSTRUCTORS
    SController() {};
    
    // PUBLIC METHODS
    void init(unsigned char INA1, unsigned char INB1, unsigned char EN1DIAG1, unsigned char PWM1, unsigned char POT);
    void run();
    void turn(int Direction, float Amount, float Speed);    
    void setMSpeed(int speed); // Set speed (-400 to 400)
    void setMBrake(int brake); // Brake (-400 to 400)
    int _potValue;
  private:
    unsigned char _INA1;
    unsigned char _INB1;
    unsigned char _PWM1;
    unsigned char _EN1DIAG1;
    unsigned char _POT;
    bool command_changed = false;

    /*
      Potentiometer Edge Values (Right - Left)
      Base: 0 - 1023
      Casing: ~100 - ~910
      Wheels: ~352 - ~630
      Ideal: 380 - 610
    */
    int const _POT_LEFT_MAX = 610; //688
    int const _POT_RIGHT_MAX = 380; // 431
    

    int _potCenter; // Center value - Calculated from right/left max
    int _potTick; // Potentiometer value for 1% turn
    int _stateDirection;
    float _stateAmount;
    float _stateSpeed; // % speed (0 - 1)    
    int _statePotGoal; // Potentiomenter goal (current state)
    //int const _POT_LEFT_MAX = 510;
    //int const _POT_RIGHT_MAX = 480;    
 
};

extern SController SteeringController;

#endif





