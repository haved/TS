#pragma once
#include "ArduinoInterface.hpp"

extern ButtonState<bool> buttons;
extern ButtonState<int> framesHeld;
extern int frameCount;
bool clicked(int framesHeld);

#define MENU_MODE 0
#define HEAVY_TRANSITION_MODE 1
#define SHIP_PLACE_MODE 2
#define BS_GAME_MODE 3
#define BS_PAUSE_MODE 4

void instantSwitchModeTo(int mode);
void lightTransitionTo(int mode, int frames);
void heavyTransitionTo(int targetMode, int frames);
