#pragma once
#include "ArduinoInterface.hpp"

extern ButtonState<bool> buttons;
extern ButtonState<int> framesHeld;
extern int frameCount;
extern int lastFrameInteractedWith;
bool pressed(int framesHeld);
bool clicked(int framesHeld);

enum GameModes {
				SPLASH_SCREEN_MODE,
				MENU_MODE,
				HEAVY_TRANSITION_MODE,
				SHIP_PLACE_MODE,
				BS_GAME_MODE,
				BS_PAUSE_MODE,
				TETRIS_GAME_MODE,
				COMET_MODE,
				PONG_MODE
};

void instantSwitchModeTo(int mode);
void lightTransitionTo(int mode, int frames);
void heavyTransitionTo(int targetMode, int frames);

void drawP2AIText(CRGB color);
void askForRedraw();

//Allows both 1 and 2 to mean player 2, while 0 is player 1
#define LEFT(p) (p==PLAYER1?-1:1)
#define RIGHT(p) (-LEFT(p))
