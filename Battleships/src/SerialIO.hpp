#pragma once
#include <fstream>

#define SERIAL_BUFFER_SIZE 128

class SerialIO {
private:
	char m_circleBuffer[SERIAL_BUFFER_SIZE]={};
	int m_writtenUntil = 0;
	int m_readUntil = 0;
	std::fstream m_serial;
	void doRead(int preserve);
public:
	SerialIO();
	~SerialIO();
	bool find(const char* text);
	char waitForByte();
	void print(const char* text);
	void write(char byt);
};
