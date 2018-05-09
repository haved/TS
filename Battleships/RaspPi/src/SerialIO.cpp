#include "SerialIO.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstring>
#include <cassert>

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

#ifdef USE_STDIO
#define INPUT std::cin
#define OUTPUT std::cout
#else
#define INPUT m_serial
#define OUTPUT m_serial
#endif


SerialIO::SerialIO() : m_serial(), m_keepReading(true) {
#ifndef USE_STDIO
	std::string wantedTTY = "/dev/"+getWantedTTY();
	m_serial.open(wantedTTY, std::ios::out | std::ios::in | std::ios::binary | std::ios::app);
	if(!m_serial.is_open()) throw std::runtime_error("Failed to open: " + wantedTTY);
#endif
}

SerialIO::~SerialIO() {
	m_serial.close();
}

char SerialIO::waitForByte() {
	char c = INPUT.get();
	std::cerr << "In:" << c << std::endl;
	if(!INPUT)
		throw std::runtime_error("Serial in was closed");
	return c;
}

bool SerialIO::isOpenForReading() {
	return ((bool) INPUT) && m_keepReading.load();
}

void SerialIO::tellToStopReading() {
	m_keepReading.store(false);
}

void SerialIO::print(const char* c) {
	std::cerr << "Out:" << c << std::endl;
	if(!OUTPUT.good())
		throw std::runtime_error("Serial out is borked");
    OUTPUT << c;
}

void SerialIO::write(char byt) {
	std::cerr << "Out:" << byt << std::endl;
	if(!OUTPUT.good())
		throw std::runtime_error("Serial out is borked");
    OUTPUT << byt;
}

void SerialIO::flush() {
	std::cerr << "Flushed" << std::endl;
	OUTPUT.flush();
}
