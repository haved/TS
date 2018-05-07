#include "Game.hpp"
#include "ArduinoEncoder.hpp"
#include "AudioSystem.hpp"
#include <cassert>
#include <cmath>

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

Mode::~Mode() {}

Mode* lastMode;
void Mode::update(ModeStack& modeStack) {
	if(!m_initialized) {
		init();
		m_initialized = true;
	}

	if(lastMode != this) {
		m_frameCount = 0;
	    onFocus();
		lastMode = this;
	} else
		m_frameCount++;

	prevButtons = buttons;
	updateButtonState(buttons); //TODO: Handle instant pushes
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
		playSound("res/Sounds/scroll_wall.wav");
		m_currentChoice = prevChoice; //Revert
	}
	else if(m_currentChoice != prevChoice) {
	    playSound("res/Sounds/hover_option.wav");
	    setPlayer1OptionColor(serial, prevChoice, OPTION_BG);
	}

	if(clicked(framesHeld.one()[BUTTON_ACTION] || clicked(framesHeld.one()[BUTTON_START]))) {
		if(m_currentChoice == NEW_2P_GAME_CHOICE) {
			playSound("res/Sounds/option_selected.wav");
		    modes.emplace_back(ModeUniquePtr(new PlaceShipsMode));
			modes.emplace_back(ModeUniquePtr(new TransitionMode(20, true)));
		} else
			; //TODO Add the other options
	}

	double interp = (sin(getFrameCount()/30.0*TAU)+1)/2;
	setPlayer1OptionColor(serial, m_currentChoice, interpolate(MENU_CHOICE_COLOR_1, MENU_CHOICE_COLOR_2, interp));
	displayScreens(serial);
}

void drawArrowDown(Player player, int y, CRGB color, CRGB bgColor) {
    for(int i = 0; i < 3; i++) {
		if(y+i < 0 || y+i >= HEIGHT)
			continue;
		auto AAColor = interpolate(bgColor, color, i/6.);
		setSingleTile(serial, player, Screen::ATTACK, (WIDTH-1)/2-i, y+i, AAColor);
		setSingleTile(serial, player, Screen::ATTACK, WIDTH/2+i, y+i, AAColor);
	}
}

#define PLACE_SHIP_PLACING 0
#define PLACE_SHIP_PLACED 1
#define PLACE_SHIP_READY 2
#define SCREEN_ANIMATION_DELAY 15
#define ARROW_COLOR CRGB(255, 100, 0)
void drawIndicatorScreens(int frame, int p1State, int p2State) {
	if(frame % SCREEN_ANIMATION_DELAY == 0) {
		setAllScreens(serial, GAME_BG); //TODO: Only attack screens

		auto drawCorrectIndicator = [&](Player p, int state, int frame) {
			if(state == PLACE_SHIP_PLACING) {
				int shift = frame % 4;
				for(int i = 0; i < 4; i++)
					drawArrowDown(p, i*4-shift, ARROW_COLOR, GAME_BG);
			} else if(state == PLACE_SHIP_PLACED) {
				int armDown = frame % 4;
				if(armDown == 3) armDown = 1;
				int buttonDown = frame % 4 == 2;
				setRect(serial, p, Screen::ATTACK, 1, 3, 8, 1, CRGB(217, 167, 95));
				setRect(serial, p, Screen::ATTACK, 3, 7-armDown, 1, HEIGHT-7+armDown, CRGB(232, 169, 144));
				setRect(serial, p, Screen::ATTACK, 4, 9-armDown, 3, HEIGHT-9+armDown, CRGB(232, 169, 144));
				setRect(serial, p, Screen::ATTACK, 2, 4, 6, 2-buttonDown, CRGB(241, 11, 11));
			}
		};

		int frame_internal = frame/SCREEN_ANIMATION_DELAY;
		drawCorrectIndicator(Player::ONE, p1State, frame_internal);
		drawCorrectIndicator(Player::TWO, p2State, frame_internal);

		displayScreens(serial);
	}
}

void PlaceShipsMode::onFocus() {
	//m_p1State = PLACE_SHIP_PLACING;
	//m_p2State = PLACE_SHIP_PLACING;
	m_p2State = PLACE_SHIP_PLACED;
	m_p1State = PLACE_SHIP_READY;

	drawIndicatorScreens(0, m_p1State, m_p2State);
}

void PlaceShipsMode::update_mode(ModeStack& modes) {
	if(clicked(framesHeld.one()[BUTTON_START])) {
		playSound("res/Sounds/option_selected.wav");
		modes.emplace_back(ModeUniquePtr(new GameMode)); //TODO: Pass ship data
		modes.emplace_back(ModeUniquePtr(new TransitionMode(20, false)));
		return;
	}

	drawIndicatorScreens(getFrameCount(), m_p1State, m_p2State);
}

void GameMode::init() {
}

void GameMode::onFocus() {
	setAllScreens(serial, GAME_BG);
	displayScreens(serial);
    loopMusic("res/Music/battle_music.mp3");
}

void GameMode::update_mode(ModeStack& modes) {
	if(clicked(framesHeld.one()[BUTTON_START])) {
		playSound("res/Sounds/pause.wav");
		modes.emplace_back(ModeUniquePtr(new InGameMenu(Player::ONE)));
	}
	if(clicked(framesHeld.two()[BUTTON_START])) {
		playSound("res/Sounds/pause.wav");
		modes.emplace_back(ModeUniquePtr(new InGameMenu(Player::TWO)));
	}
}

TransitionMode::TransitionMode(int frames, bool viaBlack) : m_frames(frames), m_viaBlack(viaBlack), m_started(false) {
}

void TransitionMode::onFocus() { } //We do all in update_mode

void TransitionMode::update_mode(ModeStack& modes) {
	auto& next = modes[modes.size()-2];

	if(!m_started) {
		assert(modes.size() > 1);
		chooseDoubleBuffer(serial);
		if(m_viaBlack)
			setAllScreens(serial, BLACK);
		else
			next->onFocus();
		startTransitionToDoubleBuffer(serial, m_frames);
		m_started = true;
	}

    if(recieveTransitionDone()) {
		if(m_viaBlack) {
			chooseDoubleBuffer(serial);
			next->onFocus();
			startTransitionToDoubleBuffer(serial, m_frames);
			m_viaBlack = false;
		} else
			modes.pop_back();
	}
}

#define INGAME_MENU_BORDER CRGB(255, 100, 0)
void InGameMenu::onFocus() {
	pauseMusic();
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
	displayScreens(serial);
}

void InGameMenu::update_mode(ModeStack& modes) {
	int button = BUTTON_START + (player == Player::TWO ? PLAYER_2_BUTTONS_OFFSET : 0);
	if(clicked(framesHeld.raw[button])) {
		playSound("res/Sounds/resume.wav");
		modes.pop_back();
	}
}
