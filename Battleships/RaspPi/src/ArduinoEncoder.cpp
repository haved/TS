#include "ArduinoEncoder.hpp"
#include <cassert>
#include <iostream>
#include <cstring>

#define ERROR(s) do { std::cerr << "ArduinoEncoder: " << __LINE__ << ": " << s << std::endl; return; } while(false)

SerialIO* global_serial_ptr;

void sendColorChannels(CRGB color) {
	serial.write(color.r);
	serial.write(color.g);
	serial.write(color.b);
}

void sendPlayerAndScreen(Player p, Screen s) {
	char c = 'A';
	if(p == Player::TWO)
		c+=2;
	if(s == Screen::DEFENSE)
		c+=1;
	serial.write(c);
}

void sendXY(int x, int y) {
	assert(x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT);
	serial.write('0'+(x+y*WIDTH));
}

void setSingleTile(Player p, Screen s, int x, int y, CRGB color) {
    serial.write('S');
	sendPlayerAndScreen(p, s);
    sendXY(x, y);
	sendColorChannels(color);
}

void setRect(Player p, Screen s, int x, int y, int width, int height, CRGB color) {
	if(width == 0 || height == 0)
		return;
	assert(width > 0 && height > 0 && x+width<=WIDTH && y+height<=HEIGHT);
	serial.write('R');
	sendPlayerAndScreen(p, s);
	sendXY(x, y);
	sendXY(width, height);
	sendColorChannels(color);
}

void setAllScreens(CRGB color) {
	serial.write('F'); //F for fill (every screen)
	sendColorChannels(color);
}

bool doneWithRepaint;

void screenUpdate() {
    doneWithRepaint = false;
	serial.write('U');
	serial.flush();
}

bool transitionsRunning;

void startTransition(Player p, Screen s, int frames) {
	serial.write('T');
    sendPlayerAndScreen(p, s);
	assert(frames > 0 && frames < 256);
	serial.write(frames);

	transitionsRunning = true;
}

void startTransitionAll(int frames) {
	startTransition(Player::ONE, Screen::ATTACK,  frames);
	startTransition(Player::ONE, Screen::DEFENSE, frames);
	startTransition(Player::TWO, Screen::ATTACK,  frames);
	startTransition(Player::TWO, Screen::DEFENSE, frames);
}

bool anyTransitionsRunning() {
	return transitionsRunning;
}

void updateButtonState(ButtonState<bool>& state) {
	const int BUF_SIZE = 10;
	static int pos = 0;
	static char buffer[BUF_SIZE];

	do {
		int read = serial.read(buffer+pos, BUF_SIZE-pos);
		pos += read;

		auto moveBack = [&](int steps) {
			memcpy(buffer, buffer+steps, pos-steps);
			pos-=steps;
			memset(buffer+pos, 0, steps);
		};

		while(pos > 0 && buffer[0] != '>') {
			std::cerr << "Recieved '" << buffer[0] << "' when waiting for '>'" << std::endl;
			moveBack(1);
		}

		auto emp = [&](int p) { return p >= pos; };

		if(emp(1))
			return;
		char c = buffer[1];
		if(c == 'T') {
			transitionsRunning = false;
			moveBack(2); //   >T
		} else if(c == '>') {
			doneWithRepaint = true;
			moveBack(2); //   >>
		} else if(c == 'D' || c == 'U') {
			bool down = c == 'D';
			if(emp(2))
				return;
			state.raw[buffer[2]-'A']=down;
			moveBack(3); //   >DA
		} else {
			std::cerr << "Unrecognized command: " << c << std::endl;
			moveBack(2); //   >?
		}
	} while(pos);
}
