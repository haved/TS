#include <iostream>
#include "SerialIO.hpp"
#include "Game.hpp"
#include <memory>
#include <chrono>
#include <thread>

int main() {
	std::cout << "Battleships running!" << std::endl;

	SerialIO serialIO;

	ModeStack modes;
	ModeUniquePtr startMode(new MenuMode(serialIO));
    modes.emplace(std::move(startMode));

	while(modes.size()) {
	    modes.top()->update(modes);
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}

	std::cout << "Battleships ended" << std::endl;
}
