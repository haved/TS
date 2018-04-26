#include "ArduinoEncoder.hpp"

void sendColorChannels(SerialIO& io, CRGB color) {
	io.write(color.r);
	io.write(color.g);
	io.write(color.b);
}

void sendColor(SerialIO& io, Player p, Screen s, int x, int y, CRGB color) {
    io.print("BS+P"); //P for player
	io.write(p == Player::ONE ? '1' : '2');
	io.write(s == Screen::ATTACK ? 'A' : 'D');
	io.write('S'); //Single tile
	io.write('0'+x);
	io.write('0'+y);
	sendColorChannels(io, color);
}

void updateButtonState(SerialIO& io, ButtonState& state) {
	if(io.find("BS+BTN ")) {
		
	}
}
