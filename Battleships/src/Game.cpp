#include "Game.hpp"
#include "ArduinoEncoder.hpp"
#include <cassert>
#include <cmath>

#define TAU M_PI*2

ButtonState<bool> buttons = {};
ButtonState<bool> prevButtons = {};
ButtonState<int> framesHeld = {};
SerialIO* global_serial_ptr;
#define serial (*global_serial_ptr)

bool clicked(int framesHeld) {
	if(framesHeld == 1)
		return true;
	if(framesHeld > 20)
		return (framesHeld-20)%6 == 0;
	return false;
}

void ModeDeleter::operator ()(Mode* mode) {
	delete mode;
}

Mode::~Mode() {}

void Mode::update(ModeStack& modeStack) {
	if(!m_initialized) {
		init();
		m_initialized = true;
		m_frameCount = 0;
	}
	m_frameCount++;
	prevButtons = buttons;
	updateButtonState(serial, buttons); //TODO: Handle instant pushes
    for(int i = 0; i < BUTTON_COUNT; i++) {
		auto& fH = framesHeld.raw[i];
		fH = buttons.raw[i] ? fH+1 : 0;
	}
	update_mode(modeStack);
}

long long Mode::getFrameCount() {
	return m_frameCount;
}

void setPlayer1OptionColor(SerialIO& io, int optionIndex, CRGB color) {
	assert(optionIndex>=0 && optionIndex+2<HEIGHT);
	sendColor(io, Player::ONE, Screen::DEFENSE, WIDTH-1, optionIndex+2, color);
}

#define MENU_BG CRGB(80, 80, 80)
#define OPTION_BG CRGB(80, 200, 80)
#define MENU_CHOICE_COLOR_1 RED
#define MENU_CHOICE_COLOR_2 CRGB(255, 255, 90)
#define CHOICE_COUNT 6
void MenuMode::init() {
	setAllScreens(serial, MENU_BG);
	m_currentChoice = 0;
    for(int i = 0; i < CHOICE_COUNT; i++)
		setPlayer1OptionColor(serial, i, OPTION_BG);
}

void MenuMode::update_mode(ModeStack& modes) {
	int prevChoice = m_currentChoice;
    if(clicked(framesHeld.one()[BUTTON_UP]))
		m_currentChoice--;
	if(clicked(framesHeld.one()[BUTTON_DOWN]))
		m_currentChoice++;

	if(m_currentChoice < 0 || m_currentChoice >= CHOICE_COUNT) {
		//TODO: Play error beep
		m_currentChoice = prevChoice; //Revert
	}
	else if(m_currentChoice != prevChoice) {
		//TODO: Play sound effect
	    setPlayer1OptionColor(serial, prevChoice, OPTION_BG);
	}

	double interp = (sin(getFrameCount()/30.0*TAU)+1)/2;
	setPlayer1OptionColor(serial, m_currentChoice, interpolate(MENU_CHOICE_COLOR_1, MENU_CHOICE_COLOR_2, interp));
}
