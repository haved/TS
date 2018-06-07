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

int currMode = MENU_MODE;
bool changedMode = true;
int frameCount = 0;

void instantSwitchModeTo(int mode) {
	currMode = mode;
	changedMode = true;
}

void lightTransitionTo(int mode, int frames) {
	for(int i = 0; i < 4; i++)
		startTransition(i, frames);
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

void updateMenuMode(bool changed); //In MenuMode.cpp
void callModeUpdateFunction(int mode, bool changed) {
	switch(mode) {
    case MENU_MODE: updateMenuMode(changed); break;
    case HEAVY_TRANSITION_MODE: updateHeavyTransitionMode(changed); break;
    default: break;
	}
}

void loop() {
	getButtonStates(buttons);

	do {
		for(int i = 0; i < BUTTON_COUNT; i++) {
			auto& fH = framesHeld.raw[i];
			fH = buttons.raw[i] ? fH+1 : 0;
		}

		if(changedMode) {
			frameCount = 0;
			changedMode = false;
			callModeUpdateFunction(currMode, true);
		} else
			callModeUpdateFunction(currMode, false);
	} while(changedMode);

	updateScreens();

	frameCount++;
}
