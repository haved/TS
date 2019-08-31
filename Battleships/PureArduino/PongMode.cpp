#include "ArduinoInterface.hpp"
#include "GameLogic.hpp"
#include "LotsOfHeader.hpp"

bool p_player2;

float p_x, p_y;
float xSpeed, ySpeed;
int p_countdown;
#define COUNTDOWN_STEPS 6
int p_countdown_sub;
#define COUNTDOWN_SUB_TIME 20

void serve(bool againstP2) {
	p_x = p_y = 0;
	xSpeed = 1;
	ySpeed = 1;
	p_countdown = COUNTDOWN_STEPS;
	p_countdown_sub = COUNTDOWN_SUB_TIME;
}

void configurePongMode(bool player2) {
	p_player2 = player2;
	fadeOutMusic(0);
	serve(true);
}

void updatePongMode(bool redraw) {
	if(redraw) {
	    fillAllScreens(0x000000);
	}

	

	if(p_countdown) { //6 is nothing, 5 is one red, 4 is two, 3 is three, 2 is four, 1 they are green
		p_countdown_sub -= 1;
		if(p_countdown_sub <= 0) {
			p_countdown--;
			p_countdown_sub = COUNTDOWN_SUB_TIME;

			if(p_countdown > 1)
				playSoundEffect(SOUND_ACTION);
			else if(p_countdown == 1) {
				playSoundEffect(SOUND_DONE);
				loopMusic(MUSIC_FIGHT);
			}


		}
		for(int i = PLAYER1; i <= PLAYER2; i += PLAYER2-PLAYER1) {
			CRGB color = 0xFF0000;
			switch(p_countdown) {
			case 6:
				break;
			case 1:
				color = 0x00FF00;
			case 2:
				setTile(i+ATK, 7, 1, color, NORMAL_COORDS);
			case 3:
				setTile(i+ATK, 5, 1, color, NORMAL_COORDS);
			case 4:
				setTile(i+ATK, 3, 1, color, NORMAL_COORDS);
			case 5:
				setTile(i+ATK, 1, 1, color, NORMAL_COORDS);
			}
		}
	}



	handleHoldEscToMenu();
}
