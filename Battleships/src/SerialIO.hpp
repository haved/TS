#pragma once
#include <fstream>
#include <atomic>

#define SERIAL_BUFFER_SIZE 64

class SerialIO {
private:
	char m_circleBuffer[SERIAL_BUFFER_SIZE]={};
	int m_writtenUntil = 0;
	int m_readUntil = 0;
	std::fstream m_serial;
	std::atomic<bool> m_keepReading;
	void doRead(int preserve);
public:
	SerialIO();
	~SerialIO();

	bool find(const char* text);
	char waitForByte();
	bool openForReading();
	void tellToStopReading();

	void print(const char* text);
	void write(char byt);
	void flush();
};

