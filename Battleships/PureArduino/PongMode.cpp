#include "ArduinoInterface.hpp"

bool p_player2;


void configurePongMode(bool player2) {
	p_player2 = player2;
	fadeOutMusic(0);
}

void updatePongMode(bool redraw) {
	
}
