#include "Game.hpp"
#include "ArduinoEncoder.hpp"
#include <cassert>
#include <cmath>

#define TAU M_PI*2

bool clicked(int framesHeld) {
	if(framesHeld == 1)
		return true;
	if(framesHeld > 30)
		return (framesHeld-30)%10 == 0;
	return false;
}

void ModeDeleter::operator ()(Mode* mode) {
	delete mode;
}

void Mode::update(ModeStack& modeStack) {
	if(!m_initialized) {
		init();
		m_initialized = true;
		m_frameCount = 0;
	}
	m_frameCount++;
	prevButtons = buttons;
	updateButtonState(m_serial, buttons);
    for(int i = 0; i < 2; i++) {
	    auto incOrNone = [&](int& val, bool inc) { val = inc ? val+1 : 0; };
		incOrNone(framesHeld.player[i].up, buttons.player[i].up);
		incOrNone(framesHeld.player[i].down, buttons.player[i].down);
		incOrNone(framesHeld.player[i].left, buttons.player[i].left);
		incOrNone(framesHeld.player[i].right, buttons.player[i].right);
		incOrNone(framesHeld.player[i].action, buttons.player[i].action);
		incOrNone(framesHeld.player[i].start, buttons.player[i].start);
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
	setAllScreens(m_serial, MENU_BG);
	m_currentChoice = 0;
    for(int i = 0; i < CHOICE_COUNT; i++)
		setPlayer1OptionColor(m_serial, i, OPTION_BG);
}

void MenuMode::update_mode(ModeStack& modes) {
	int prevChoice = m_currentChoice;
    if(clicked(framesHeld.one().up))
		m_currentChoice--;
	if(clicked(framesHeld.one().down))
		m_currentChoice++;

	if(m_currentChoice < 0 || m_currentChoice >= CHOICE_COUNT) {
		//TODO: Play error beep
		m_currentChoice = prevChoice; //Revert
	}
	else if(m_currentChoice != prevChoice) {
		//TODO: Play sound effect
	    setPlayer1OptionColor(m_serial, prevChoice, OPTION_BG);
	}

	double interp = (sin(getFrameCount()/30.0*TAU)+1)/2;
	setPlayer1OptionColor(m_serial, m_currentChoice, interpolate(MENU_CHOICE_COLOR_1, MENU_CHOICE_COLOR_2, interp));
}
