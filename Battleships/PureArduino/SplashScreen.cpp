#include "ArduinoInterface.hpp"
#include "GameLogic.hpp"

#define TRANSITION_TIME 100
CRGB colors[] = {{255, 0, 0}, {0, 0, 255}, {100, 100, 200}, {255, 255, 0}, {0, 255, 255}, {0, 255, 0}};
#define SCREEN_COLOR_COUNT (sizeof(colors)/sizeof(*colors))
void updateSplashScreenMode(bool redraw) {

	if(redraw) {
		bothPlayers(clearLCD(player));
		bothPlayers(printLCDText(player, "Velkommen!"));
		bothPlayers(setLCDPosition(player, 1, 1));
		bothPlayers(printLCDText(player, "Trykk A eller B"));
	}

	if(clicked(buttons.one()[BUTTON_A]) || clicked(buttons.one()[BUTTON_B])
	   || clicked(buttons.two()[BUTTON_A]) || clicked(buttons.two()[BUTTON_B])) {
		playSoundEffect(SOUND_ACTION);
		heavyTransitionTo(MENU_MODE, 15);
	}

	static int currentScreen = 0;
	if(redraw)
		currentScreen = -1;
	else if(anyTransitionRunning())
		return;

	currentScreen++;
	currentScreen%=SCREEN_COLOR_COUNT;

	startTransitionAll(TRANSITION_TIME);
	for(int i = 0; i < 4; i++)
		fillScreen(i, colors[(currentScreen+i)%SCREEN_COLOR_COUNT]);
}
