#include "ArduinoInterface.hpp"

#include <stdio.h>

ButtonState<bool> buttons = {};
ButtonState<int> framesHeld = {};

void resetFramesHeld() {
	framesHeld = {};
}

bool clicked(int framesHeld) {
	if(framesHeld == 1)
		return true;
	if(framesHeld > 20)
		return (framesHeld-20)%6 == 0;
	return false;
}

#define MENU_MODE 0
int currMode = MENU_MODE;
int prevMode = -1;
int frameCount = 0;
void callModeUpdateFunction(int mode, bool changed);

void lightTransitionTo(int mode, int frames) {
	for(int i = 0; i < 4; i++)
		startTransition(i, frames);
	currMode = mode;
}

#define HEAVY_TRANSITION_MODE 1
int heavyTransitionTarget = 0;
int heavyTransitionFrames = 0;
void heavyTransitionTo(int targetMode, int frames) {
    lightTransitionTo(HEAVY_TRANSITION_MODE, frames);
	heavyTransitionTarget = targetMode;
	heavyTransitionFrames = frames;
}


#define MENU_BG CRGB(80, 80, 80)
#define MENU_OPTION_BG CRGB(80, 200, 80)
#define MENU_OPTION_COLOR_1 CRGB(255, 0, 0)
#define MENU_OPTION_COLOR_2 CRGB(255, 255, 90)

#define MENU_OPTIONS_Y_OFFSET 1
#define MENU_CHOICE_COUNT 6
void updateMenuMode(bool first) {
	static int menuChoicePos = 0;

	if(first) {
		menuChoicePos = 0;
		fillAllScreens(MENU_BG);
		fillRect(PLAYER1 + DEF, 0, MENU_OPTIONS_Y_OFFSET, 1, MENU_OPTIONS_Y_OFFSET + MENU_CHOICE_COUNT, MENU_OPTION_BG);
	}

	int prevChoice = menuChoicePos;
	if(clicked(framesHeld.one()[BUTTON_DOWN]))
		menuChoicePos++;
	if(clicked(framesHeld.one()[BUTTON_UP]))
		menuChoicePos--;
	if(menuChoicePos == prevChoice);
	else if(menuChoicePos < 0 || menuChoicePos >= MENU_CHOICE_COUNT)
		menuChoicePos = prevChoice;
	else
		setTile(PLAYER1+DEF, 0, MENU_OPTIONS_Y_OFFSET + prevChoice, MENU_OPTION_BG);

	CRGB color = interpolate(MENU_OPTION_COLOR_1, MENU_OPTION_COLOR_2, (sin(frameCount/5.)+1)/2);
	setTile(PLAYER1+DEF, 0, MENU_OPTIONS_Y_OFFSET + menuChoicePos, color);

	if(clicked(framesHeld.one()[BUTTON_A]))
		heavyTransitionTo(MENU_MODE, 20);
}

void updateHeavyTransitionMode(bool first) {
	if(first)
		fillAllScreens(CRGB::Black);
	if(anyTransitionRunning())
		return;
    lightTransitionTo(heavyTransitionTarget, heavyTransitionFrames);
}

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

		bool changed = prevMode!=currMode;
		if(changed)
			frameCount = 0;
		prevMode = currMode;
		callModeUpdateFunction(currMode, changed);
	} while(prevMode != currMode);

	updateScreens();

	frameCount++;
}
