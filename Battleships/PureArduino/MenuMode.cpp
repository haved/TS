#include "GameLogic.hpp"
#include "ArduinoInterface.hpp"
#include "LotsOfHeader.hpp"

#define MENU_BG CRGB(40, 60, 60)
#define MENU_OPTION_BG CRGB(80, 255, 80)
#define MENU_OPTION_COLOR_1 CRGB(255, 0, 0)
#define MENU_OPTION_COLOR_2 CRGB(255, 90, 255)

#define MENU_OPTIONS_Y_OFFSET 0
#define MENU_CHOICE_COUNT 6

#define P1_SHIRT_COLOR CRGB(250,70,100)
#define P1_EYE_COLOR CRGB(0, 0, 255)
#define P2_SHIRT_COLOR CRGB(126,150,225)
#define P2_EYE_COLOR CRGB(0, 255, 0)

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

enum MenuChoice {
				 BATTLESHIPS_CHOICE=0,
				 TETRIS_CHOICE,
				 COMET_CHOICE=4
};

const char* get_game_name(int code) {
	switch(code) {
	case BATTLESHIPS_CHOICE: return "Battleships";
	case TETRIS_CHOICE: return "Tetris";
	default: return "N/A";
	}
}

#define IDLE_TIME_BEFORE_SPLASH 1000
void updateMenuMode(bool redraw) {
	static int menuChoicePos = 0;
	static bool player2 = false;
	static int animate_player2 = 0;
	bool redraw_lcd = false;

	if(redraw) {
		fadeOutMusic(20);
        fillScreen(PLAYER1+DEF, MENU_BG);
		fillRect(PLAYER1 + DEF, 0, MENU_OPTIONS_Y_OFFSET, 1, MENU_CHOICE_COUNT, MENU_OPTION_BG);
		animate_player2 = 0;
		redraw_lcd = true;
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
		//TODO: playSoundEffect(SOUND_MOVE_ACTION);
		setTile(PLAYER1+DEF, 0, MENU_OPTIONS_Y_OFFSET + prevChoice, MENU_OPTION_BG);
		redraw_lcd = true;
	}

	CRGB color = interpolate(MENU_OPTION_COLOR_1, MENU_OPTION_COLOR_2, (sin(frameCount/5.)+1)/2);
	setTile(PLAYER1+DEF, 0, MENU_OPTIONS_Y_OFFSET + menuChoicePos, color);

	if(pressed(framesHeld.one()[BUTTON_A])) {
		playSoundEffect(SOUND_DONE);
		switch(menuChoicePos) {
		case BATTLESHIPS_CHOICE:
			configureShipPlaceMode(player2);
			heavyTransitionTo(SHIP_PLACE_MODE, 10); break;
		case TETRIS_CHOICE:
		    configureTetrisMode(player2);
			heavyTransitionTo(TETRIS_GAME_MODE, 10); break;
		case COMET_CHOICE:
		    configureCometMode(player2);
			heavyTransitionTo(COMET_MODE, 10); break;
		default: heavyTransitionTo(MENU_MODE, 20); break;
		}
	}

	if(pressed(framesHeld.two()[BUTTON_A]) && !player2) {
		playSoundEffect(SOUND_PLAYER_2_JOIN);
		player2 = true;
		animate_player2 = 20;
		redraw_lcd = true;
	}

	if(pressed(framesHeld.two()[BUTTON_MENU]) && player2) {
		playSoundEffect(SOUND_PLAYER_2_LEAVE);
		player2 = false;
	    animate_player2 = 20;
		redraw_lcd = true;
	}

	if(animate_player2) {
		animate_player2--;
		bool drawP2 = player2 == animate_player2%8<4;
		if(drawP2)
			drawP2Human();
		else
			drawP2CPU();
	}

	if(redraw_lcd) {
		fadeOutMusic(20);

		bothPlayers(clearLCD(player));
		if(player2)
			printLCDText(PLAYER1, "Spill sammen:");
		else
			printLCDText(PLAYER1, "Spill alene:");
		setLCDPosition(PLAYER1, 2, 1);
		printLCDText(PLAYER1, get_game_name(menuChoicePos));

		if(player2) {
			printLCDText(PLAYER2, "Du er med!");
			setLCDPosition(PLAYER2, 2, 1);
			printLCDText(PLAYER2, "(MENU) - Forlat");
		}
		else {
			printLCDText(PLAYER2, "Du er ikke med!");
			setLCDPosition(PLAYER2, 2, 1);
			printLCDText(PLAYER2, "(A) - Bli med");
		}
	}

	if(frameCount - lastFrameInteractedWith > IDLE_TIME_BEFORE_SPLASH)
	    instantSwitchModeTo(SPLASH_SCREEN_MODE);
}

#define HOLD_FRAMES 100
#define HOLD_WAIT_UNTIL_SHOW 10
#define HOLD_COLOR CRGB(70, 20, 20)
void handleHoldEscToMenu() {
	int held = framesHeld.one()[BUTTON_MENU] + framesHeld.two()[BUTTON_MENU];
    int heldPixels = (held-HOLD_WAIT_UNTIL_SHOW)*(WIDTH*HEIGHT/2)/(HOLD_FRAMES-HOLD_WAIT_UNTIL_SHOW);

	if(heldPixels > 0) {
		int fullRows = heldPixels / WIDTH;
		int rest = heldPixels % WIDTH;

		if(fullRows > 0) {
			allScreens(fillRect(screen, 0, 0, WIDTH, fullRows, HOLD_COLOR));
			allScreens(fillRect(screen, 0, HEIGHT-fullRows, WIDTH, fullRows, HOLD_COLOR));
		}
		if(rest > 0) {
			allScreens(fillRect(screen, 0, fullRows, rest, 1, HOLD_COLOR));
			allScreens(fillRect(screen, WIDTH-rest, HEIGHT-1-fullRows, rest, 1, HOLD_COLOR));
		}

		askForRedraw();
	}

	if(held >= HOLD_FRAMES) {
		fadeOutMusic(100);
		lightTransitionTo(MENU_MODE, 20);
		playSoundEffect(SOUND_DONE);
	}
}
