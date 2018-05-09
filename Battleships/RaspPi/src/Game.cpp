#include "Game.hpp"
#include "ArduinoEncoder.hpp"
#include "AudioSystem.hpp"
#include <cassert>
#include <cmath>

#include "BattleshipsGame.hpp"

#define TAU M_PI*2

ButtonState<bool> buttons = {};
ButtonState<bool> prevButtons = {};
ButtonState<int> framesHeld = {};

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

Mode* lastMode;
void update(ModeStack& modeStack) {
	prevButtons = buttons;
	updateButtonState(buttons); //TODO: Handle instant pushes
    for(int i = 0; i < BUTTON_COUNT; i++) {
		auto& fH = framesHeld.raw[i];
		fH = buttons.raw[i] ? fH+1 : 0;
	}

	if(modeStack.empty())
		return;

	Mode* mode = modeStack.back().get();

	if(mode != lastMode) {
		lastMode = mode;
		mode->onFocus();
	}
	mode->frameCount++;
	mode->update(modeStack);

	/*if(modeStack.size() && modeStack.back().get() != mode)
	  update(modeStack); //Recursion, increasing the button held count, among other things*/
}

Mode::~Mode() {}

#define OPTION_LIST_Y_OFFSET 1
void setPlayer1OptionColor(int optionIndex, CRGB color) {
	assert(optionIndex>=0 && optionIndex+2<HEIGHT);
	setSingleTile(Player::ONE, Screen::DEFENSE, 0, optionIndex+OPTION_LIST_Y_OFFSET, color);
}

#define MENU_BG CRGB(80, 80, 80)
#define OPTION_BG CRGB(80, 200, 80)
#define MENU_CHOICE_COLOR_1 RED
#define MENU_CHOICE_COLOR_2 CRGB(255, 255, 90)
#define MENU_CHOICE_COUNT 6
#define NEW_2P_GAME_CHOICE 0
#define NEW_1P_GAME_CHOICE 1

void MenuMode::onFocus() {
	setAllScreens(MENU_BG);
	m_currentChoice = 0;
	setRect(Player::ONE, Screen::DEFENSE, 0, OPTION_LIST_Y_OFFSET, 1, MENU_CHOICE_COUNT, OPTION_BG);
}

void MenuMode::update(ModeStack& modes) {
	int prevChoice = m_currentChoice;
    if(clicked(framesHeld.one()[BUTTON_UP]))
		m_currentChoice--;
	if(clicked(framesHeld.one()[BUTTON_DOWN]))
		m_currentChoice++;

	if(m_currentChoice < 0 || m_currentChoice >= MENU_CHOICE_COUNT) {
		playSound("res/Sounds/scroll_wall.wav");
		m_currentChoice = prevChoice; //Revert
	}
	else if(m_currentChoice != prevChoice) {
	    playSound("res/Sounds/hover_option.wav");
	    setPlayer1OptionColor(prevChoice, OPTION_BG);
	}

	if(clicked(framesHeld.one()[BUTTON_A] || clicked(framesHeld.one()[BUTTON_B]))) {
		playSound("res/Sounds/option_selected.wav");
		if(m_currentChoice == NEW_2P_GAME_CHOICE) {
		    modes.emplace_back(ModeUniquePtr(new PlaceShipsMode));
			modes.emplace_back(ModeUniquePtr(new TransitionMode(20, true)));
		} else
			; //TODO Add the other options
	}

	double interp = (sin(frameCount/30.0*TAU)+1)/2;
	setPlayer1OptionColor(m_currentChoice, interpolate(MENU_CHOICE_COLOR_1, MENU_CHOICE_COLOR_2, interp));
}

TransitionMode::TransitionMode(int frames, bool viaBlack) : m_frames(frames), m_viaBlack(viaBlack), m_started(false) {
}

void TransitionMode::onFocus() { } //We do all in update_mode

void TransitionMode::update(ModeStack& modes) {
	assert(modes.size() > 1);

	if(m_started && anyTransitionsRunning())
		return;

	if(m_viaBlack && !m_started) {
		startTransitionAll(m_frames);
		setAllScreens(BLACK);
		m_started = true;
	} else {
		startTransitionAll(m_frames);
		modes.pop_back();
	}
}
