#ifndef SteeringController_h
#define SteeringController_h

#include <Arduino.h>

class SController
{
  public:  
    // CONSTRUCTORS
    SController() {};
    
    // PUBLIC METHODS
    void init(unsigned char INA1, unsigned char INB1, unsigned char EN1DIAG1, unsigned char PWM1, unsigned char POT);
    void run();
    void turn(int speed, float amount);    
    void setMSpeed(int speed); // Set speed (-400 to 400)
    void setMBrake(int brake); // Brake (-400 to 400)
    
  private:
    unsigned char _INA1;
    unsigned char _INB1;
    unsigned char _PWM1;
    unsigned char _EN1DIAG1;
    unsigned char _POT;

    /*
      Potentiometer Edge Values (Right - Left)
      Base: 0 - 1023
      Casing: ~100 - ~910
      Wheels: ~352 - ~630
      Ideal: 380 - 610
    */
    int const _POT_LEFT_MAX = 610;
    int const _POT_RIGHT_MAX = 380;
    int _pot_value;
    //int const _POT_LEFT_MAX = 510;
    //int const _POT_RIGHT_MAX = 480;    
 
};

extern SController SteeringController;

#endif