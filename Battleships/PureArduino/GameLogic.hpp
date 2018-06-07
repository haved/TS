#pragma once
#include "ArduinoInterface.hpp"

extern ButtonState<bool> buttons;
extern ButtonState<int> framesHeld;
extern int frameCount;
extern int lastFrameInteractedWith;
bool clicked(int framesHeld);

#define SPLASH_SCREEN_MODE 0
#define MENU_MODE 1
#define HEAVY_TRANSITION_MODE 2
#define SHIP_PLACE_MODE 3
#define BS_GAME_MODE 4
#define BS_PAUSE_MODE 5

void instantSwitchModeTo(int mode);
void lightTransitionTo(int mode, int frames);
void heavyTransitionTo(int targetMode, int frames);
