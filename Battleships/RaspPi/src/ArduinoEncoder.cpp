#include "ArduinoEncoder.hpp"
#include <atomic>
#include <mutex>
#include <cassert>
#include <iostream>

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

bool started = false;
void startUpdate() {
	if(started)
		ERROR("Already started command");
	started = true;
	serial.write('>');
}

void assureStarted() {
	if(!started)
		startUpdate();
}

void setSingleTile(Player p, Screen s, int x, int y, CRGB color) {
	assureStarted();
    serial.write('S');
	sendPlayerAndScreen(p, s);
    sendXY(x, y);
	sendColorChannels(color);
}

void setRect(Player p, Screen s, int x, int y, int width, int height, CRGB color) {
	if(width == 0 || height == 0)
		return;
	assert(width > 0 && height > 0 && x+width<=WIDTH && y+height<=HEIGHT);
	assureStarted();
	serial.write('R');
	sendPlayerAndScreen(p, s);
	sendXY(x, y);
	sendXY(width, height);
	sendColorChannels(color);
}

void setAllScreens(CRGB color) {
	assureStarted();
	serial.write('F'); //F for fill (every screen)
	sendColorChannels(color);
}

void commitUpdate() {
	if(!started)
		ERROR("Commiting command that was never started");
	serial.write('\n');
	serial.flush();
	started = false;
}

std::mutex inputMutex;
bool transitionsRunning;

void startTransition(Player p, Screen s, int frames) {
	std::lock_guard<std::mutex> stateLock(inputMutex);

	serial.write('T');
    sendPlayerAndScreen(p, s);
	assert(frames > 0 && frames < 256);
	serial.write(frames);

	transitionsRunning = true;
}

ButtonState<bool> threaded_buttonState;

bool anyTransitionsRunning() {
	std::lock_guard<std::mutex> stateLock(inputMutex);
	return transitionsRunning;
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
				transitionsRunning = false;
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
