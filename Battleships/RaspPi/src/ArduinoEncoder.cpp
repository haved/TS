#include "ArduinoEncoder.hpp"
#include <cassert>
#include <iostream>
#include <cstring>

#include <thread>
#include <condition_variable>
#include <atomic>
#include <mutex>

#define scoped_lock unique_lock<std::mutex>

#define ERROR(s) do { std::cerr << "ArduinoEncoder: " << __LINE__ << ": " << s << std::endl; return; } while(false)

SerialIO* global_serial_ptr;
std::atomic<bool> atomic_keepThreadRunning;
std::mutex inputMutex;

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

bool threaded_doneWithRepaint;
std::condition_variable repaintCV;

void screenUpdate() {
	std::scoped_lock lock(inputMutex);
    threaded_doneWithRepaint = false;
	serial.write('U');
	serial.flush();
	repaintCV.wait(lock, [&]{return threaded_doneWithRepaint;});
}

bool threaded_transitionsRunning;

void startTransition(Player p, Screen s, int frames) {
	std::scoped_lock lock(inputMutex);
	serial.write('T');
    sendPlayerAndScreen(p, s);
	assert(frames > 0 && frames < 256);
	serial.write(frames);

	threaded_transitionsRunning = true;
}

void startTransitionAll(int frames) {
	startTransition(Player::ONE, Screen::ATTACK,  frames);
	startTransition(Player::ONE, Screen::DEFENSE, frames);
	startTransition(Player::TWO, Screen::ATTACK,  frames);
	startTransition(Player::TWO, Screen::DEFENSE, frames);
}

bool anyTransitionsRunning() {
	std::scoped_lock lock(inputMutex);
	return threaded_transitionsRunning;
}

ButtonState<bool> threaded_buttonState;

void updateButtonState(ButtonState<bool>& state) {
    std::scoped_lock lock(inputMutex);
    state = threaded_buttonState;
}

void inputListeningThread() {
	while(atomic_keepThreadRunning.load()) {
		char c = serial.waitForByte();
		if(c != '>') {
			std::cerr << "Got trash from serial: " << c << std::endl;
			continue;
		}

		c = serial.waitForByte();

		if(c == 'T') {
			std::scoped_lock lock(inputMutex);
			threaded_transitionsRunning = false;
		} else if(c == '>') {
			std::scoped_lock lock(inputMutex);
			threaded_doneWithRepaint = true;
			repaintCV.notify_one();
		} else if(c == 'D' || c == 'U') {
			bool down = c == 'D';
			int button = serial.waitForByte()-'A';
			std::scoped_lock lock(inputMutex);
			threaded_buttonState.raw[button]=down;
		} else
			std::cerr << "Unrecognized command: >" << c << std::endl;
	}
}

std::thread startInputListeningThread() {
	assert(global_serial_ptr);
	atomic_keepThreadRunning.store(true);
	return std::thread(inputListeningThread);
}

void stopInputListeningThread() {
	atomic_keepThreadRunning.store(false);
}
