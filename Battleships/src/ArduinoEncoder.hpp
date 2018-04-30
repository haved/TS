#pragma once

#include "SerialIO.hpp"
#include "Util.hpp"

#include <thread>

#define WIDTH 10
#define HEIGHT 10

enum class Player:int {
	ONE=0,
	TWO=1
};

enum class Screen:int {
	ATTACK=0,
	DEFENSE=1
};

#define BUTTON_COUNT 12
#define BUTTON_LEFT 0
#define BUTTON_RIGHT 1
#define BUTTON_UP 2
#define BUTTON_DOWN 3
#define BUTTON_ACTION 4
#define BUTTON_START 5
#define PLAYER_2_BUTTONS_OFFSET 6


template<typename T>
struct ButtonState {
	T raw[BUTTON_COUNT];
	auto one() { return &raw[0]; }
	auto two() { return &raw[PLAYER_2_BUTTONS_OFFSET]; }
};


void setSingleTile(SerialIO& serial, Player p, Screen s, int x, int y, CRGB color);

void setRect(SerialIO& io, Player p, Screen s, int x, int y, int width, int height, CRGB color);

void setAllScreens(SerialIO& serial, CRGB color);

void updateButtonState(SerialIO& io, ButtonState<bool>& state);
std::thread startListeningThread(SerialIO* io);
