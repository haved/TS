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

	auto last = std::chrono::steady_clock::now();
	auto second = std::chrono::seconds(1);
	int framesSinceLast = 0;
	while(modes.size()) {
	    //update(modes);
		//std::this_thread::sleep_for(std::chrono::milliseconds(16));
		screenUpdate();
		std::cerr << "We waiting" << std::endl;
		framesSinceLast++;
		if(std::chrono::steady_clock::now() - last > second) {
			std::cerr << "FPS: " << framesSinceLast << std::endl;
			framesSinceLast = 0;
			last -= second;
		}
	}

	serialIO.tellToStopReading();
	readingThread.join();

	stopAudioSystem();

	std::cout << "Battlearcade ended" << std::endl;
}
