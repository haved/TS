## Battleships Arcade Machine
This folder contains the source code for a Battleships Arcade Machine.
Four screens, two joysticks and some buttons are connected to an Arduino Mega,
which in turn communicates to a Raspberry Pi Model 3B+ over USB.
The Pi runs the bulk of the program, only using the Arduino as a GPU.
There is also a third folder, `ArcadeSimulator`, containing a python program emulating the Arduino, putting screens and button inputs in a window on the development machine.
If you want to do run the emulated program, use `./debug.sh` in `RaspPi`
If you want to run a Pi, auto-execute `./pullAndRunForever.sh` from the same folder

### Dependencies
For Raspberry Pi:
 - Cmake
 - C++14
 - pthreads
 - sdl2_mixer

For Arduino:
 - Arduino IDE
 - FastLED library
 
For ArcadeSimulator:
 - python3
 - tkinter
