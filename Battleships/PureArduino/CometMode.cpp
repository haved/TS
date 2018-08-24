#include "ArduinoInterface.hpp"
#include "GameLogic.hpp"
#include "LotsOfHeader.hpp"

bool c_player2;

void configureCometMode(bool player2) {
	c_player2 = player2;
}

void udpateCometMode(bool redraw) {
	if(redraw) {
		//loopMusic(MUSIC_COMET);
	}
}
