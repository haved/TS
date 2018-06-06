#pragma once
#include "ArduinoInterface.hpp"

extern ButtonState<bool> buttons;
extern ButtonState<int> framesHeld;
bool clicked(int framesHeld);
