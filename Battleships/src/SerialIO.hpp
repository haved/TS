#pragma once
#include <fstream>

#define SERIAL_BUFFER_SIZE 128

class SerialIO {
private:
	char m_buffer[SERIAL_BUFFER_SIZE];
	std::fstream m_serial;
public:
	SerialIO();
	~SerialIO();
	void readuntil(char* text);
	char* readline();
	void print(char* text);
};
