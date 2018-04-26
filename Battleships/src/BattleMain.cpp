#include <iostream>
#include "SerialIO.hpp"

int main() {
	std::cout << "Battleships running!" << std::endl;

	SerialIO serialIO;

	while(!serialIO.find("This is a test: "));

	std::cout << "Battleships ended" << std::endl;
}
