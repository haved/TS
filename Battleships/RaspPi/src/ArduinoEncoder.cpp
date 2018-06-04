#include "ArduinoEncoder.hpp"
#include <cassert>
#include <iostream>
#include <cstring>

#include <thread>
#include <condition_variable>
#include <atomic>
#include <mutex>

#define ERROR(s) do{													             \
		std::lock_guard<std::mutex> lock(loggingMutex);					             \
		std::cerr << "ArduinoEncoder:" << __LINE__ << ": error: " << s << std::endl; \
		throw std::runtime_error("Previous error was fatal");			             \
	} while(false)

SerialIO* global_serial_ptr;
std::atomic<bool> atomic_keepThreadRunning;
std::mutex inputMutex;

std::unique_lock<std::mutex> lockInput() {
	if(atomic_keepThreadRunning.load() == false)
		throw std::runtime_error("Locking input mutex while input thread is stopped");
	return std::unique_lock<std::mutex>(inputMutex);
}

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
	serial.write('\0'+(x+y*WIDTH));
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
    auto lock = lockInput();
    threaded_doneWithRepaint = false;
	serial.write('U');
	serial.flush();
	repaintCV.wait(lock, [&]{return threaded_doneWithRepaint;});
}

bool threaded_transitionsRunning;

void startTransition(Player p, Screen s, int frames) {
    auto lock = lockInput();
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
    auto lock = lockInput();
	return threaded_transitionsRunning;
}

ButtonState<bool> threaded_buttonState;

void updateButtonState(ButtonState<bool>& state) {
    auto lock = lockInput();
    state = threaded_buttonState;
}

void waitForCommand() {
	char c = serial.waitForByte();
	if(c != '>') {
		std::lock_guard<std::mutex> lock(loggingMutex);
		std::cerr << "Got trash from serial: " << c << std::endl;
	    return;
	}

	c = serial.waitForByte();

	if(c == 'T') {
		std::unique_lock<std::mutex> lock(inputMutex);
		threaded_transitionsRunning = false;
	} else if(c == '>' || c == '<') { // >> means update done, >< means distress
		std::unique_lock<std::mutex> lock(inputMutex);
		threaded_doneWithRepaint = true;
		repaintCV.notify_one();
	} else if(c == 'D' || c == 'U') {
		bool down = c == 'D';
		int button = serial.waitForByte()-'A';
		std::unique_lock<std::mutex> lock(inputMutex);
		threaded_buttonState.raw[button]=down;
	} else {
		std::lock_guard<std::mutex> lock(loggingMutex);
		std::cerr << "Unrecognized command: >" << c << std::endl;
	}
}

void inputListeningThread() {
	try {
		while(atomic_keepThreadRunning.load())
			waitForCommand();
	} catch(std::runtime_error e) {
		{
			std::lock_guard<std::mutex> lockLog(loggingMutex);
			std::cerr << "Excpetion thrown in listening thread: " << e.what() << std::endl;
		}
		atomic_keepThreadRunning.store(false);
		std::unique_lock<std::mutex> lockInput(inputMutex);
	    if(threaded_doneWithRepaint == false) {
			threaded_doneWithRepaint = true;
			repaintCV.notify_one();
		}
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
