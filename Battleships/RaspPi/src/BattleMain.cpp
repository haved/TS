#include <iostream>
#include "SerialIO.hpp"
#include "Game.hpp"
#include <memory>
#include <chrono>
#include <thread>
#include "ArduinoEncoder.hpp"
#include "AudioSystem.hpp"

int main() {
	std::cout << "Battlearcade running!" << std::endl;

	startAudioSystem();

	SerialIO serialIO;
	global_serial_ptr = &serialIO;

	std::thread readingThread = startListeningThread(&serialIO);

	ModeStack modes;
	ModeUniquePtr startMode(new MenuMode());
    modes.emplace_back(std::move(startMode));

	while(modes.size()) {
	    update(modes);
		std::this_thread::sleep_for(std::chrono::milliseconds(16)); //TODO: Sleep for less if we taking too long
		screenUpdate();
	}

	serialIO.tellToStopReading();
	readingThread.join();

	stopAudioSystem();

	std::cout << "Battlearcade ended" << std::endl;
}
