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

void updateSplashScreenMode(bool changed); //In SplashScreen.cpp
void updateMenuMode(bool changed); //In MenuMode.cpp
void updateShipPlaceMode(bool changed); //In PlaceShipsMode.cpp
void callModeUpdateFunction(int mode, bool changed) {
	switch(mode) {
	case SPLASH_SCREEN_MODE: updateSplashScreenMode(changed); break;
    case MENU_MODE: updateMenuMode(changed); break;
    case HEAVY_TRANSITION_MODE: updateHeavyTransitionMode(changed); break;
	case SHIP_PLACE_MODE: updateShipPlaceMode(changed); break;
    default: break;
	}
}

void loop() {
	getButtonStates(buttons);

	do {
		frameCount++;

		for(int i = 0; i < BUTTON_COUNT; i++) {
			auto& fH = framesHeld.raw[i];
			fH = buttons.raw[i] ? fH+1 : 0;
			if(fH)
				lastFrameInteractedWith = frameCount;
		}

		if(changedMode) {
			frameCount = 0;
			lastFrameInteractedWith = 0;
			changedMode = false;
			callModeUpdateFunction(currMode, true);
		} else
			callModeUpdateFunction(currMode, false);
	} while(changedMode);

	updateScreens();
}
