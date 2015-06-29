# desertviper

## Setup


## Controller
The Controller class is responsible for interfacing with the various hardware/sensor on board.  It initializes PIN mappings for each device and calls each driver to update its state with the main program loop.

## Program
The Program class is responsible for sending instructions to the Controller that dictate when and how each device should behave.  This is where the logic for automation lives.

