## Battleships Arcade Machine

![Machine in action](/Battleships/MahcineInAction.jpg?raw=true)
The machine in Action at the 2019 Trondheim Maker Faire
  
This folder contains the source code for a Battleships Arcade Machine.
Four screens, two joysticks and some buttons are connected to an Arduino Mega.
It runs a program with all the games and goodness.
It is powered over USB by a Raspberry Pi, which also
listens for sound IDs to play.
  
For easy debugging there is also a simulator project with a Makefile.
It will link together the Arduino code, but replaces some files.
The output will be a gtk+-3.0 window and the sound will play directly from the program.

### Dependencies
*PureArduino* (runs on the Arduino)
- FastLED

*Sound server* (runs on the Pi)
- SDL2
- SDL2_mixer
  
*PureArduinoSim* (For debugging, includes the two others)
- gtk+3.0
- SDL2
- SDL2_mixer
