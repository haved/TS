#include "GameModes.hpp"
#include "GameLogic.hpp"

#define MENU_BG CRGB(80, 80, 80)
#define MENU_OPTION_BG CRGB(80, 200, 80)
#define MENU_OPTION_COLOR_1 CRGB(255, 0, 0)
#define MENU_OPTION_COLOR_2 CRGB(255, 255, 90)

#define MENU_OPTIONS_Y_OFFSET 1
#define MENU_CHOICE_COUNT 6
void updateMenuMode(bool first) {
	static int menuChoicePos = 0;

	if(first) {
		menuChoicePos = 0;
		fillAllScreens(MENU_BG);
		fillRect(PLAYER1 + DEF, 0, MENU_OPTIONS_Y_OFFSET, 1, MENU_OPTIONS_Y_OFFSET + MENU_CHOICE_COUNT, MENU_OPTION_BG);
	}

	int prevChoice = menuChoicePos;
	if(clicked(framesHeld.one()[BUTTON_DOWN]))
		menuChoicePos++;
	if(clicked(framesHeld.one()[BUTTON_UP]))
		menuChoicePos--;
	if(menuChoicePos == prevChoice);
	else if(menuChoicePos < 0 || menuChoicePos >= MENU_CHOICE_COUNT)
		menuChoicePos = prevChoice;
	else
		setTile(PLAYER1+DEF, 0, MENU_OPTIONS_Y_OFFSET + prevChoice, MENU_OPTION_BG);

	CRGB color = interpolate(MENU_OPTION_COLOR_1, MENU_OPTION_COLOR_2, (sin(frameCount/5.)+1)/2);
	setTile(PLAYER1+DEF, 0, MENU_OPTIONS_Y_OFFSET + menuChoicePos, color);

	if(clicked(framesHeld.one()[BUTTON_A]))
		heavyTransitionTo(MENU_MODE, 20);
}
