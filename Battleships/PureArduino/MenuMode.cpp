#include "GameLogic.hpp"
#include "ArduinoInterface.hpp"
#include "LotsOfHeader.hpp"

#define MENU_BG CRGB(230, 200, 220)
#define MENU_OPTION_BG CRGB(80, 200, 80)
#define MENU_OPTION_COLOR_1 CRGB(255, 0, 0)
#define MENU_OPTION_COLOR_2 CRGB(255, 255, 90)

#define MENU_OPTIONS_Y_OFFSET 1
#define MENU_CHOICE_COUNT 6

#define P1_SHIRT_COLOR CRGB(190,52,81)
#define P1_EYE_COLOR CRGB(33, 72, 196)
#define P2_SHIRT_COLOR CRGB(126,150,225)
#define P2_EYE_COLOR CRGB(52, 162, 53)

void drawPerson(int screen, int x, int y, CRGB eyeColor, CRGB shirtColor) {
    fillRect(screen, x, y, 3, 4, shirtColor); //Body
	CRGB headColor(209,150,138);
	setTile(screen, x+1, y+4, headColor); //neck
	fillRect(screen, x, y+5, 3, 3, headColor); //Head
	setTile(screen, x, y+6, eyeColor); //Eye
	setTile(screen, x+2, y+6, eyeColor); //Eye
}

void drawComputer(int screen, int x, int y) {
	CRGB tableColor(199, 172, 87);
	fillRect(screen, x, y, 1, 3, tableColor);
	fillRect(screen, x+2, y, 1, 3, tableColor);
	setTile(screen, x+1, y+2, tableColor);

	CRGB screenPlastic(255, 255, 255);
	fillRect(screen, x, y+3, 3, 1, screenPlastic);
	setTile(screen, x+1, y+4, screenPlastic);
	fillRect(screen, x+2, y+5, 1, 2, screenPlastic);

	CRGB screenColor(0,0,0);
	fillRect(screen, x, y+5, 2, 2, screenColor);
}

void drawP2CPU() {
	fillScreen(PLAYER2+DEF, MENU_BG);
	fillScreen(PLAYER2+ATK, MENU_BG);
	fillScreen(PLAYER1+ATK, MENU_BG);

	drawP2AIText(CRGB(0, 100, 255));

	drawPerson(PLAYER1+ATK, 1, 1, P1_EYE_COLOR, P1_SHIRT_COLOR);
	drawComputer(PLAYER1+ATK, 6, 1);

	drawPerson(PLAYER2+ATK, 6, 1, P2_EYE_COLOR, P2_SHIRT_COLOR);
	drawComputer(PLAYER2+ATK, 1, 1);
}

void drawP2Human() {
	fillScreen(PLAYER2+DEF, MENU_BG);
	fillScreen(PLAYER2+ATK, MENU_BG);
	fillScreen(PLAYER1+ATK, MENU_BG);

	drawPerson(PLAYER1+ATK, 1, 1, P1_EYE_COLOR, P1_SHIRT_COLOR);
	drawPerson(PLAYER1+ATK, 6, 1, P2_EYE_COLOR, P2_SHIRT_COLOR);

	drawPerson(PLAYER2+ATK, 1, 1, P1_EYE_COLOR, P1_SHIRT_COLOR);
	drawPerson(PLAYER2+ATK, 6, 1, P2_EYE_COLOR, P2_SHIRT_COLOR);
}

#define IDLE_TIME_BEFORE_SPLASH 1000
void updateMenuMode(bool first) {
	static int menuChoicePos = 0;
	static bool player2 = false;
	static int animate_player2 = 0;

	if(first) {
        fillScreen(PLAYER1+DEF, MENU_BG);
		menuChoicePos = 0;
		fillRect(PLAYER1 + DEF, 0, MENU_OPTIONS_Y_OFFSET, 1, MENU_CHOICE_COUNT, MENU_OPTION_BG);
		animate_player2 = 0;
	    if(player2) //We keep p2 from previous
			drawP2Human();
		else
			drawP2CPU();
	}

	int prevChoice = menuChoicePos;
	if(clicked(framesHeld.one()[BUTTON_DOWN]))
		menuChoicePos++;
	if(clicked(framesHeld.one()[BUTTON_UP]))
		menuChoicePos--;
	if(menuChoicePos == prevChoice);
	else if(menuChoicePos < 0 || menuChoicePos >= MENU_CHOICE_COUNT) {
		playSoundEffect(SOUND_ILLEGAL_MOVE);
		menuChoicePos = prevChoice;
	}
	else {
		//playSoundEffect(SOUND_MOVE_ACTION);
		setTile(PLAYER1+DEF, 0, MENU_OPTIONS_Y_OFFSET + prevChoice, MENU_OPTION_BG);
	}

	CRGB color = interpolate(MENU_OPTION_COLOR_1, MENU_OPTION_COLOR_2, (sin(frameCount/5.)+1)/2);
	setTile(PLAYER1+DEF, 0, MENU_OPTIONS_Y_OFFSET + menuChoicePos, color);

	if(clicked(framesHeld.one()[BUTTON_A])) {
		playSoundEffect(SOUND_DONE);
		switch(menuChoicePos) {
		case 0:
			configureShipPlaceMode(player2);
			heavyTransitionTo(SHIP_PLACE_MODE, 10); break;
		default: heavyTransitionTo(MENU_MODE, 20); break;
		}
	}

	if(clicked(framesHeld.two()[BUTTON_A]) && !player2) {
		player2 = true;
		animate_player2 = 20;
	}

	if(clicked(framesHeld.two()[BUTTON_MENU]) && player2) {
		player2 = false;
	    animate_player2 = 20;
	}

	if(animate_player2) {
		animate_player2--;
		bool drawP2 = player2 == animate_player2%8<4;
		if(drawP2)
			drawP2Human();
		else
			drawP2CPU();
	}

	if(frameCount - lastFrameInteractedWith > IDLE_TIME_BEFORE_SPLASH)
	    instantSwitchModeTo(SPLASH_SCREEN_MODE);
}

