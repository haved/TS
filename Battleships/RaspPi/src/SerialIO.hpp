#pragma once

class SerialIO {
private:
	int in;
	int out;
public:
	SerialIO();
	~SerialIO();

	char waitForByte();

	void print(const char* text);
	void write(char byt);
	void flush();
};
