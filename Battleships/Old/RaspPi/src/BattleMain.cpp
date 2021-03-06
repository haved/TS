#include <iostream>
#include "SerialIO.hpp"
#include "Game.hpp"
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>
#include "ArduinoEncoder.hpp"
#include "AudioSystem.hpp"

void countFPS() {
	static auto lastFPS = std::chrono::steady_clock::now();
	static const auto second = std::chrono::seconds(1);
	static int framesSinceLastFPS = 0;
	framesSinceLastFPS++;
	if(std::chrono::steady_clock::now() - lastFPS > second) {
		std::lock_guard<std::mutex> lock(loggingMutex);
		std::cerr << "FPS: " << framesSinceLastFPS << std::endl;
		framesSinceLastFPS = 0;
		lastFPS += second;
	}
}

void serialIOSyncerThroughSpam() {
	char first = 0;
	while(true) {
		if(first != '>') {
			first = serial.waitForByte();
			continue;
		}
		first = serial.waitForByte();
		if(first == '<')
			return;
	}
}

void gameLoop() {
	serialIOSyncerThroughSpam();

	auto inputThread = startInputListeningThread();
	screenUpdate();

	ModeStack modes;
	ModeUniquePtr startMode(new MenuMode());
	modes.emplace_back(std::move(startMode));

	try {
		auto lastFrame = std::chrono::steady_clock::now();
		auto sleepTime = std::chrono::milliseconds(1000/50);
		while(modes.size()) {
			update(modes);
			screenUpdate();

			auto now = std::chrono::steady_clock::now();
			std::this_thread::sleep_for(sleepTime-(now-lastFrame));
n			lastFrame = now;

			countFPS();
		}
	}
	catch(std::runtime_error e) {
		std::lock_guard<std::mutex> lock(loggingMutex);
		std::cerr << "Excpetion thrown in main thread: " << e.what() << std::endl;
	}

	stopInputListeningThread();
	inputThread.join();
}

int main() {
	std::cout << "Battlearcade running!" << std::endl;

	startAudioSystem();

	try {
		SerialIO serialIO;
		global_serial_ptr = &serialIO;
		gameLoop();
	}
	catch(std::runtime_error e) {
		std::cerr << "Excpetion thrown in sole thread: " << e.what() << std::endl;
		goto cleanup;
	}

 cleanup:
	stopAudioSystem();

	std::cout << "Battlearcade ended" << std::endl;
}
