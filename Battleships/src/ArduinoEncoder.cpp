#include "ArduinoEncoder.hpp"
#include <atomic>
#include <mutex>

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
	io.write('\n');
	io.flush();
}

void setAllScreens(SerialIO& io, CRGB color) {
	io.print("BS+F"); //F for fill (every screen)
	sendColorChannels(io, color);
	io.write('\n');
	io.flush();
}


ButtonState<bool> threaded_buttonState;
std::mutex buttonStateMutex;

void updateButtonState(SerialIO& io, ButtonState<bool>& state) {
	std::lock_guard<std::mutex> stateLock(buttonStateMutex);
	state = threaded_buttonState;
}

void threadedListeningFunc(SerialIO* io) {
	while(io->openForReading()) {
		if(io->find("BS+")) {
			bool down = io->waitForByte()=='D';
			char byt = io->waitForByte();
			std::lock_guard<std::mutex> stateLock(buttonStateMutex);
			threaded_buttonState.raw[byt-'A']=down;
		}
	}
}

std::thread startListeningThread(SerialIO* io) {
    return std::thread(threadedListeningFunc, io);
}
