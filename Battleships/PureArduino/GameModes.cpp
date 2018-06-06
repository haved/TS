#include "GameModes.hpp"
#include "ArduinoInterface.hpp"
#include "GameLogic.hpp"

int currMode = MENU_MODE;
int prevMode = -1;
int frameCount = 0;

void lightTransitionTo(int mode, int frames) {
	for(int i = 0; i < 4; i++)
		startTransition(i, frames);
	currMode = mode;
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
