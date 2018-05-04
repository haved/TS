#pragma once
#include <fstream>
#include <atomic>

class SerialIO {
private:
	std::fstream m_serial;
	std::atomic<bool> m_keepReading;
	void doRead(int preserve);
public:
	SerialIO();
	~SerialIO();

	char waitForByte();
	bool isOpenForReading();
	void tellToStopReading();

	void print(const char* text);
	void write(char byt);
	void flush();
};

