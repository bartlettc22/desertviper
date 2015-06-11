# desertviper

## Setup
### Running from Raspberry Pi
On Raspbian, run command:
> sudo apt-get install arduino-mk
Run this command to determine the tag for the board

Before running the main program (main_program), you need to add the libraries contained in the /libraries directory into your own libraries folder (usually located at ~/Audrino/libraries).  You can also do this via the Arduino IDE from Sketch -> Include Library -> Add .ZIP Library -> [Select each library folder one at a time]

The main program is very simple.  It instantiates two objects: Controller and Program which are responsible for the two main functions of the program.

## Controller
The Controller class is responsible for interfacing with the various hardware/sensor on board.  It initializes PIN mappings for each device and calls each driver to update its state with the main program loop.

## Program
The Program class is responsible for sending instructions to the Controller that dictate when and how each device should behave.  This is where the logic for automation lives.

