#include "SerialIO.hpp"
#include <string>
#include <iostream>
#include <stdexcept>
#include <cstring>

#define CMD_BUFFER_SIZE 256

std::string getWantedTTY() {
	const char* cmd = "dmesg | grep tty | tail -n 1";

	FILE* pipe = popen(cmd, "r");
	if(!pipe) throw std::runtime_error("popen() failed ;(");

	char buffer[CMD_BUFFER_SIZE];
	if(fgets(buffer, CMD_BUFFER_SIZE, pipe) == nullptr)
		throw std::runtime_error("Couldn't read anything from the dmesg pipe");

	char* at = strstr(buffer, "tty");
	if(!at) throw std::runtime_error("Didn't find any \"tty\" in dmesg");
	char* end = strstr(at, ":");
	if(!end) throw std::runtime_error("No ':' after the tty name in dmesg");

	*end = '\0';
	std::string result(at);
	pclose(pipe);
	return result;
}

SerialIO::SerialIO() : m_buffer{}, m_serial() {
	std::string wantedTTY = "/dev/"+getWantedTTY();
	m_serial.open(wantedTTY, std::ios::out | std::ios::in | std::ios::binary);
	if(!m_serial.is_open()) throw std::runtime_error("Failed to open: " + wantedTTY);
}

SerialIO::~SerialIO() {
	m_serial.close();
}
