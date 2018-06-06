#include "ArduinoInterface.hpp"
#include "GameLogic.hpp"
#include "GameModes.hpp"

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
