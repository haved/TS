#include "BattleshipsGame.hpp"
#include "AudioSystem.hpp"

#define INGAME_MENU_BORDER CRGB(255, 100, 0)
void InGameMenu::onFocus() {
	pauseMusic();
    auto drawBorder = [&](Player player, Screen screen) {
		setRect(player, screen, 0, 0, WIDTH-1, 1, INGAME_MENU_BORDER);
		setRect(player, screen, WIDTH-1, 0, 1, HEIGHT-1, INGAME_MENU_BORDER);
		setRect(player, screen, 1, HEIGHT-1, WIDTH-1, 1, INGAME_MENU_BORDER);
		setRect(player, screen, 0, 1, 1, HEIGHT-1, INGAME_MENU_BORDER);
	};
	drawBorder(Player::ONE, Screen::DEFENSE);
    drawBorder(Player::TWO, Screen::DEFENSE);
	drawBorder(Player::ONE, Screen::ATTACK);
	drawBorder(Player::TWO, Screen::ATTACK);
    commitUpdate();
}

void InGameMenu::update(ModeStack& modes) {
    auto ownerHeld = &framesHeld.raw[player == Player::ONE ? 0 : PLAYER_2_BUTTONS_OFFSET];
	if(clicked(ownerHeld[BUTTON_A])) {
		playSound("res/Sounds/resume.wav");
		modes.pop_back();
	}
	if(ownerHeld[BUTTON_MENU] > 30) {
		playSound("res/Sounds/option_selected.wav");
	    modes.clear();
		modes.push_back(ModeUniquePtr(new MenuMode));
		modes.push_back(ModeUniquePtr(new TransitionMode(30, true)));
	}
}

void drawArrowDown(Player player, int y, CRGB color, CRGB bgColor) {
    for(int i = 0; i < 3; i++) {
		if(y+i < 0 || y+i >= HEIGHT)
			continue;
		auto AAColor = interpolate(bgColor, color, i/6.);
		setSingleTile(player, Screen::ATTACK, (WIDTH-1)/2-i, y+i, AAColor);
		setSingleTile(player, Screen::ATTACK, WIDTH/2+i, y+i, AAColor);
	}
}

void drawButtonAnimation(Player player, Screen screen, int frame) {
	int armDown = frame % 4;
	if(armDown == 3) armDown = 1;
	int buttonDown = frame % 4 == 2;
	setRect(player, screen, 1, 3, 8, 1, CRGB(217, 167, 95));
	setRect(player, screen, 3, 7-armDown, 1, HEIGHT-7+armDown, CRGB(232, 169, 144));
	setRect(player, screen, 4, 9-armDown, 3, HEIGHT-9+armDown, CRGB(232, 169, 144));
	setRect(player, screen, 2, 4, 6, 2-buttonDown, CRGB(241, 11, 11));
}

#define PLACE_SHIP_PLACING 0
#define PLACE_SHIP_PLACED 1
#define PLACE_SHIP_READY 2
#define SCREEN_ANIMATION_DELAY 15
#define ARROW_COLOR CRGB(255, 100, 0)
void drawIndicatorScreens(int frame, int p1State, int p2State) {
	if(frame % SCREEN_ANIMATION_DELAY == 0) {
		setAllScreens(GAME_BG); //TODO: Only attack screens

		auto drawCorrectIndicator = [&](Player p, int state, int frame) {
			if(state == PLACE_SHIP_PLACING) {
				int shift = frame % 4;
				for(int i = 0; i < 4; i++)
					drawArrowDown(p, i*4-shift, ARROW_COLOR, GAME_BG);
			} else if(state == PLACE_SHIP_PLACED) {
				drawButtonAnimation(p, Screen::ATTACK, frame);
			}
		};

		int frame_internal = frame/SCREEN_ANIMATION_DELAY;
		drawCorrectIndicator(Player::ONE, p1State, frame_internal);
		drawCorrectIndicator(Player::TWO, p2State, frame_internal);

		commitUpdate();
	}
}

void PlaceShipsMode::onFocus() {
	//m_p1State = PLACE_SHIP_PLACING;
	//m_p2State = PLACE_SHIP_PLACING;
	m_p2State = PLACE_SHIP_PLACED;
	m_p1State = PLACE_SHIP_READY;

	drawIndicatorScreens(0, m_p1State, m_p2State);
}

void PlaceShipsMode::update(ModeStack& modes) {
	if(clicked(framesHeld.one()[BUTTON_A])) {
		playSound("res/Sounds/option_selected.wav");
		modes.emplace_back(ModeUniquePtr(new GameMode)); //TODO: Pass ship data
		modes.emplace_back(ModeUniquePtr(new TransitionMode(20, false)));
		return;
	}

	drawIndicatorScreens(frameCount, m_p1State, m_p2State);
}

GameMode::GameMode() {
	stopMusic();
}

void GameMode::onFocus() {
	setAllScreens(GAME_BG);
    commitUpdate();
    loopMusic("res/Music/battle_music.mp3");
}

void GameMode::update(ModeStack& modes) {
	if(clicked(framesHeld.one()[BUTTON_MENU])) {
		playSound("res/Sounds/pause.wav");
		modes.emplace_back(ModeUniquePtr(new InGameMenu(Player::ONE)));
	}
	if(clicked(framesHeld.two()[BUTTON_MENU])) {
		playSound("res/Sounds/pause.wav");
		modes.emplace_back(ModeUniquePtr(new InGameMenu(Player::TWO)));
	}
}
