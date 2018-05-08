#pragma once

#include "SerialIO.hpp"
#include "Util.hpp"

#include <thread>

#define WIDTH 10
#define HEIGHT 10

extern SerialIO* global_serial_ptr;
#define serial (*global_serial_ptr)

enum class Player:int {
	ONE=0,
	TWO=1
};

enum class Screen:int {
	ATTACK=0,
	DEFENSE=1
};

#define PLAYER_2_BUTTONS_OFFSET 7
#define BUTTON_COUNT PLAYER_2_BUTTONS_OFFSET*2
#define BUTTON_LEFT 0
#define BUTTON_RIGHT 1
#define BUTTON_UP 2
#define BUTTON_DOWN 3
#define BUTTON_A 4
#define BUTTON_B 5
#define BUTTON_MENU 6

template<typename T>
struct ButtonState {
	T raw[BUTTON_COUNT];
	auto one() { return &raw[0]; }
	auto two() { return &raw[PLAYER_2_BUTTONS_OFFSET]; }
};

void startUpdate();

void setSingleTile(Player p, Screen s, int x, int y, CRGB color);

void setRect(Player p, Screen s, int x, int y, int width, int height, CRGB color);

void setAllScreens(CRGB color);

void commitUpdate();

void startTransition(Player p, Screen s, int frames);
void startTransitionAll(int frames);
bool anyTransitionsRunning();

void updateButtonState(ButtonState<bool>& state);
std::thread startListeningThread(SerialIO* io);
