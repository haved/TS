#include "ArduinoEncoder.hpp"
#include <atomic>
#include <mutex>
#include <cassert>

void sendColorChannels(SerialIO& io, CRGB color) {
	io.write(color.r);
	io.write(color.g);
	io.write(color.b);
}

void sendPlayerAndScreen(SerialIO& io, Player p, Screen s) {
	char c = 'A';
	if(p == Player::TWO)
		c+=2;
	if(s == Screen::DEFENSE)
		c+=1;
	io.write(c);
}

void sendXY(SerialIO& io, int x, int y) {
	assert(x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT);
	io.write('0'+(x+y*WIDTH));
}

void setSingleTile(SerialIO& io, Player p, Screen s, int x, int y, CRGB color) {
    io.print(">S");
	sendPlayerAndScreen(io, p, s);
    sendXY(io, x, y);
	sendColorChannels(io, color);
	io.flush();
}

void setRect(SerialIO& io, Player p, Screen s, int x, int y, int width, int height, CRGB color) {
	if(width == 0 || height == 0)
		return;
	assert(width > 0 && height > 0 && x+width<=WIDTH && y+height<=HEIGHT);
	io.print(">R");
	sendPlayerAndScreen(io, p, s);
	sendXY(io, x, y);
	sendXY(io, width, height);
	sendColorChannels(io, color);
	io.flush();
}

void setAllScreens(SerialIO& io, CRGB color) {
	io.print(">F"); //F for fill (every screen)
	sendColorChannels(io, color);
	io.flush();
}

void displayScreens(SerialIO& io) {
	io.print(">D");
	io.flush();
}

void chooseDoubleBuffer(SerialIO& serial) {
	serial.print(">B");
	serial.flush();
}

bool transitionDone=false;

void startTransitionToDoubleBuffer(SerialIO& serial, int frames) {
	serial.print(">T");
	serial.write(frames);
	serial.flush();
}

ButtonState<bool> threaded_buttonState;
std::mutex inputMutex;

bool recieveTransitionDone() {
	std::lock_guard<std::mutex> stateLock(inputMutex);
	if(transitionDone) {
		transitionDone = false;
		return true;
	}
	return false;
}

void updateButtonState(ButtonState<bool>& state) {
	std::lock_guard<std::mutex> stateLock(inputMutex);
	state = threaded_buttonState;
}

void threadedListeningFunc(SerialIO* io) {
	while(io->isOpenForReading()) {
		if(io->waitForByte() == '>') {
			char c = io->waitForByte();
			if(c == 'T') {
				std::lock_guard<std::mutex> stateLock(inputMutex);
				transitionDone = true;
			} else {
				bool down = c == 'D';
				char byt = io->waitForByte();
				std::lock_guard<std::mutex> stateLock(inputMutex);
				threaded_buttonState.raw[byt-'A']=down;
			}
		}
	}
}

std::thread startListeningThread(SerialIO* io) {
    return std::thread(threadedListeningFunc, io);
}
