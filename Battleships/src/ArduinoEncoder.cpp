#include "ArduinoEncoder.hpp"
#include <atomic>
#include <mutex>

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

ButtonState<bool> threaded_buttonState;
std::mutex buttonStateMutex;

void updateButtonState(SerialIO& io, ButtonState<bool>& state) {
	std::lock_guard<std::mutex> stateLock(buttonStateMutex);
	state = threaded_buttonState;
}

void threadedListeningFunc(SerialIO* io) {
	while(io->isOpenForReading()) {
		if(io->waitForByte() == '>') {
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
