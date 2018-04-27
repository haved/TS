#pragma once

#include "SerialIO.hpp"
#include "Util.hpp"
#include "Game.hpp"

#include <thread>

enum class Player:int {
	ONE=0,
	TWO=1
};

enum class Screen:int {
	ATTACK=0,
	DEFENSE=1
};

void sendColor(SerialIO& serial, Player p, Screen s, int x, int y, CRGB color);

void setAllScreens(SerialIO& serial, CRGB color);

void updateButtonState(SerialIO& io, ButtonState<bool>& state);
std::thread startListeningThread(SerialIO* io);
