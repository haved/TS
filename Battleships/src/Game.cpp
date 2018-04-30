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

Mode* lastMode;
void Mode::update(ModeStack& modeStack) {
	if(!m_initialized) {
		init();
		m_initialized = true;
		m_frameCount = 0;
	}

	if(lastMode != this) {
	    onFocus();
		lastMode = this;
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

#define OPTION_LIST_Y_OFFSET 2
void setPlayer1OptionColor(SerialIO& io, int optionIndex, CRGB color) {
	assert(optionIndex>=0 && optionIndex+2<HEIGHT);
	setSingleTile(io, Player::ONE, Screen::DEFENSE, WIDTH-1, optionIndex+OPTION_LIST_Y_OFFSET, color);
}

#define MENU_BG CRGB(80, 80, 80)
#define OPTION_BG CRGB(80, 200, 80)
#define MENU_CHOICE_COLOR_1 RED
#define MENU_CHOICE_COLOR_2 CRGB(255, 255, 90)
#define MENU_CHOICE_COUNT 6
#define NEW_2P_GAME_CHOICE 0
#define NEW_1P_GAME_CHOICE 1

#define GAME_BG CRGB(50, 60, 200)

void MenuMode::init() {
	m_currentChoice = 0;
}

void MenuMode::onFocus() {
	setAllScreens(serial, MENU_BG);
	m_currentChoice = 0;
	setRect(serial, Player::ONE, Screen::DEFENSE, WIDTH-1, OPTION_LIST_Y_OFFSET, 1, MENU_CHOICE_COUNT, OPTION_BG);
}

void MenuMode::update_mode(ModeStack& modes) {
	int prevChoice = m_currentChoice;
    if(clicked(framesHeld.one()[BUTTON_UP]))
		m_currentChoice--;
	if(clicked(framesHeld.one()[BUTTON_DOWN]))
		m_currentChoice++;

	if(m_currentChoice < 0 || m_currentChoice >= MENU_CHOICE_COUNT) {
		//TODO: Play error beep
		m_currentChoice = prevChoice; //Revert
	}
	else if(m_currentChoice != prevChoice) {
		//TODO: Play sound effect
	    setPlayer1OptionColor(serial, prevChoice, OPTION_BG);
	}

	if(clicked(framesHeld.one()[BUTTON_ACTION] || clicked(framesHeld.one()[BUTTON_START]))) {
		if(m_currentChoice == NEW_2P_GAME_CHOICE) {
		    modes.emplace(ModeUniquePtr(new PlaceShipsMode));
			modes.emplace(ModeUniquePtr(new TransitionMode(MENU_BG, GAME_BG, 50)));
		} else
			; //TODO Add the other options
	}

	double interp = (sin(getFrameCount()/30.0*TAU)+1)/2;
	setPlayer1OptionColor(serial, m_currentChoice, interpolate(MENU_CHOICE_COLOR_1, MENU_CHOICE_COLOR_2, interp));
}

void drawArrowDown(Player player, CRGB color) {
	setRect(serial, player, Screen::ATTACK, WIDTH/2, 2, 1+(1+WIDTH)%2, HEIGHT-4, color);
	setRect(serial, player, Screen::ATTACK, WIDTH/2-3, 4, 5+(1+WIDTH%2), 2, color);
}

#define ARROW_COLOR CRGB(255, 100, 0)
void PlaceShipsMode::onFocus() {
	setAllScreens(serial, GAME_BG);
	drawArrowDown(Player::ONE, ARROW_COLOR);
	drawArrowDown(Player::TWO, ARROW_COLOR);
}

void PlaceShipsMode::update_mode(ModeStack& modes) {
	if(clicked(framesHeld.one()[BUTTON_START]))
		modes.emplace(ModeUniquePtr(new GameMode)); //TODO: Pass ship data
}

void GameMode::init() {
	
}

void GameMode::onFocus() {
	setAllScreens(serial, GAME_BG);
}

void GameMode::update_mode(ModeStack& modes) {
	if(clicked(framesHeld.one()[BUTTON_START]))
		modes.emplace(ModeUniquePtr(new InGameMenu(Player::ONE)));
	if(clicked(framesHeld.two()[BUTTON_START]))
		modes.emplace(ModeUniquePtr(new InGameMenu(Player::TWO)));
}

TransitionMode::TransitionMode(CRGB from, CRGB to, int frames) : from(from), to(to), frames(frames) {}

void TransitionMode::update_mode(ModeStack& modes) {
	setAllScreens(serial, interpolate(to, from, (float)getFrameCount()/frames));
	if(getFrameCount() >= frames)
		modes.pop();
}

#define INGAME_MENU_BORDER CRGB(255, 100, 0)
void InGameMenu::onFocus() {
    auto drawBorder = [&](Player player, Screen screen) {
		setRect(serial, player, screen, 0, 0, WIDTH-1, 1, INGAME_MENU_BORDER);
		setRect(serial, player, screen, WIDTH-1, 0, 1, HEIGHT-1, INGAME_MENU_BORDER);
		setRect(serial, player, screen, 1, HEIGHT-1, WIDTH-1, 1, INGAME_MENU_BORDER);
		setRect(serial, player, screen, 0, 1, 1, HEIGHT-1, INGAME_MENU_BORDER);
	};
	drawBorder(Player::ONE, Screen::DEFENSE);
    drawBorder(Player::TWO, Screen::DEFENSE);
	drawBorder(Player::ONE, Screen::ATTACK);
	drawBorder(Player::TWO, Screen::ATTACK);
}

void InGameMenu::update_mode(ModeStack& modes) {
	int button = BUTTON_START + (player == Player::TWO ? PLAYER_2_BUTTONS_OFFSET : 0);
	if(clicked(framesHeld.raw[button]))
		modes.pop();
}
