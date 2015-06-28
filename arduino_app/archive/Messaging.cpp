#include "Messaging.h"

/*
  Set up all inputs/outputs
*/
void Msg::init()
{
  //cmdMessenger;

  // Begin serial communication at high speed 115200 bit/s
  Serial.begin(115200); 
  
  // Adds newline to every command
  cmdMessenger2.printLfCr();   

  // Attach my application's user-defined callback methods
  attachCommandCallbacks();

  // Let the serial listener know that the Arduino has started
  cmdMessenger2.sendCmd(kStatus,"Arduino has started!");

  // Send logs to Raspberry every 500ms
  MsgTimer.every(500, Msg::sendLog);

  // Start the loop timer
  loop_time = micros();
}

/*
  Runs the main program
*/
void Msg::run()
{
  // Check for new serial commands
  cmdMessenger2.feedinSerialData();

  // Trigger any timed events
  MsgTimer.update();

  // Iterate loop counter
  loop_count++;
}

// Callbacks define on which received commands we take action 
void Msg::attachCommandCallbacks()
{
  cmdMessenger2.attach(Msg::OnUnknownCommand);
  //cmdMessenger.attach(kSetLed, Msg.OnSetLed);
}

// Called when a received command has no attached function
void Msg::OnUnknownCommand()
{
  cmdMessenger2.sendCmd(kStatus,"Command without attached callback");
}

// Callback function that sets led on or off
void Msg::OnSetLed()
{
  // Read led state argument, interpret string as boolean
  //ledState = cmdMessenger.readBoolArg();
  // Set led
  //digitalWrite(kBlinkLed, ledState?HIGH:LOW);
}

void Msg::sendLog() 
{  
  // Capture program frequency)
  /*int freq = loop_count/(micros()-loop_time)/1000000;
  loop_count = 0;
  loop_time = micros();

  String output = "{["
  //"" + String(freq, DEC) + ","
  "1"
  "]}";
  cmdMessenger.sendCmd(kLog,output);*/
}

//Msg Messenger = Msg();

