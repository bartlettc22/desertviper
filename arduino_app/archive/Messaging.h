#ifndef Messaging_h
#define Messaging_h

// Libraries
#include "CmdMessenger.h"
#include "Timer.h"

// Commands
enum
{
  kSetLed, // Command to request led to be set in specific state
  kStatus,
  kLog
};

// Attach a new CmdMessenger object to the default Serial port
CmdMessenger cmdMessenger2 = CmdMessenger(Serial);

Timer MsgTimer;

class Msg
{
  public:

    // Loop timer and counter to determine program frequency
    unsigned long loop_time; 
    unsigned long loop_count;     

    Msg() { };
  	void init(); 
  	void run();  //Runs the main program

  private:

    void attachCommandCallbacks();
    static void OnUnknownCommand();
    static void OnSetLed();
    static void sendLog(); 
};

//extern Msg Messenger;

#endif

