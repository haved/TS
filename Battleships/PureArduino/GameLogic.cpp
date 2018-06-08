#include "ArduinoInterface.hpp"
#include "GameLogic.hpp"

#include <stdio.h>

ButtonState<bool> buttons = {};
ButtonState<int> framesHeld = {};

bool clicked(int framesHeld) {
	if(framesHeld == 1)
		return true;
	if(framesHeld > 20)
		return (framesHeld-20)%6 == 0;
	return false;
}


int currMode = SPLASH_SCREEN_MODE;
bool changedMode = true;
bool redrawWanted = false;
int frameCount = 0;
int lastFrameInteractedWith = 0;

void instantSwitchModeTo(int mode) {
	currMode = mode;
	changedMode = true;
}

void lightTransitionTo(int mode, int frames) {
	startTransitionAll(frames);
    instantSwitchModeTo(mode);
}

int heavyTransitionTarget = 0;
int heavyTransitionFrames = 0;
void heavyTransitionTo(int targetMode, int frames) {
    lightTransitionTo(HEAVY_TRANSITION_MODE, frames);
	heavyTransitionTarget = targetMode;
	heavyTransitionFrames = frames;
}

void updateHeavyTransitionMode(bool first) {
	if(first)
		fillAllScreens(CRGB::Black);
	if(anyTransitionRunning())
		return;
    lightTransitionTo(heavyTransitionTarget, heavyTransitionFrames);
}

void updateSplashScreenMode(bool redraw); //In SplashScreen.cpp
void updateMenuMode(bool redraw); //In MenuMode.cpp
void updateShipPlaceMode(bool redraw); //In PlaceShipsMode.cpp
void updateBattleshipsMode(bool redraw); //In BattleshipsMode.cpp
void callModeUpdateFunction(int mode, bool redraw) {
	switch(mode) {
	case SPLASH_SCREEN_MODE: updateSplashScreenMode(redraw); break;
    case MENU_MODE: updateMenuMode(redraw); break;
    case HEAVY_TRANSITION_MODE: updateHeavyTransitionMode(redraw); break;
	case SHIP_PLACE_MODE: updateShipPlaceMode(redraw); break;
	case BS_GAME_MODE: updateBattleshipsMode(redraw); break;
    default: break;
	}
}

void loop() {
	getButtonStates(buttons);

	frameCount++;
	for(int i = 0; i < BUTTON_COUNT; i++) {
		auto& fH = framesHeld.raw[i];
		fH = buttons.raw[i] ? fH+1 : 0;
		if(fH)
			lastFrameInteractedWith = frameCount;
	}

	do {
		bool redrawWasWanted = redrawWanted;
		redrawWanted = false;
		if(changedMode) {
			frameCount = 0;
			for(int i = 0; i < BUTTON_COUNT; i++)
				framesHeld.raw[i] = 0;
			lastFrameInteractedWith = 0;
			changedMode = false;
			callModeUpdateFunction(currMode, true);
		} else
			callModeUpdateFunction(currMode, redrawWasWanted);
	} while(changedMode);

	updateScreens();
}

void drawP2AIText(CRGB color) {
	//A
	fillRect(PLAYER2+DEF, 6, 3, 2, 1, color);
	fillRect(PLAYER2+DEF, 5, 4, 1, 4, color);
	fillRect(PLAYER2+DEF, 8, 4, 1, 4, color);
	fillRect(PLAYER2+DEF, 6, 5, 2, 1, color);

	//I
    fillRect(PLAYER2+DEF, 1, 3, 3, 1, color);
	fillRect(PLAYER2+DEF, 1, 7, 3, 1, color);
	fillRect(PLAYER2+DEF, 2, 4, 1, 3, color);
}

void askForRedraw() {
	redrawWanted = true;
}
