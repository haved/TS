#pragma once
#include <fstream>

#define SERIAL_BUFFER_SIZE 128

class SerialIO {
private:
	char m_circleBuffer[SERIAL_BUFFER_SIZE]={};
	int m_currentPos = 0;
	std::fstream m_serial;
public:
	SerialIO();
	~SerialIO();
	bool find(char* text);
	char* readline();
	void print(char* text);
};
