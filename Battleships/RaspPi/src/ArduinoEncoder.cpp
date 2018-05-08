#include "ArduinoEncoder.hpp"
#include <atomic>
#include <mutex>
#include <cassert>
#include <iostream>
#include <condition_variable>

#define ERROR(s) do { std::cerr << "ArduinoEncoder: " << __LINE__ << ": " << s << std::endl; return; } while(false)

SerialIO* global_serial_ptr;
std::mutex inputMutex;

#define scoped_lock lock_guard<std::mutex>

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
std::condition_variable repaintCV;

void screenUpdate() {
	std::unique_lock<std::mutex> inputLock(inputMutex);
	doneWithRepaint = false;
	serial.write('U');
	serial.flush();
	repaintCV.wait(inputLock, [&]{return doneWithRepaint;});
}

bool transitionsRunning;

void startTransition(Player p, Screen s, int frames) {
	serial.write('T');
    sendPlayerAndScreen(p, s);
	assert(frames > 0 && frames < 256);
	serial.write(frames);

	std::scoped_lock stateLock(inputMutex);
	transitionsRunning = true;
}

void startTransitionAll(int frames) {
	startTransition(Player::ONE, Screen::ATTACK,  frames);
	startTransition(Player::ONE, Screen::DEFENSE, frames);
	startTransition(Player::TWO, Screen::ATTACK,  frames);
	startTransition(Player::TWO, Screen::DEFENSE, frames);
}

bool anyTransitionsRunning() {
	std::scoped_lock stateLock(inputMutex);
	return transitionsRunning;
}

ButtonState<bool> threaded_buttonState;

void updateButtonState(ButtonState<bool>& state) {
	std::scoped_lock stateLock(inputMutex);
	state = threaded_buttonState;
}

void threadedListeningFunc(SerialIO* io) {
	while(io->isOpenForReading()) {
		if(io->waitForByte() == '>') {
			char c = io->waitForByte();
			if(c == 'T') {
				std::scoped_lock stateLock(inputMutex);
				transitionsRunning = false;
			} else if(c == '>') {
				std::scoped_lock stateLock(inputMutex);
			    doneWithRepaint = true;
				repaintCV.notify_one();
			} else {
				bool down = c == 'D';
				char byt = io->waitForByte();
				std::scoped_lock stateLock(inputMutex);
				threaded_buttonState.raw[byt-'A']=down;
			}
		}
	}
}

std::thread startListeningThread(SerialIO* io) {
    return std::thread(threadedListeningFunc, io);
}
