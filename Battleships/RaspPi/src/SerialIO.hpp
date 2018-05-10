#pragma once

class SerialIO {
private:
	int nonBlockingIn;
	int out;
public:
	SerialIO();
	~SerialIO();

	int getByteIfReady();
	int read(char* buf, int maxLen);

	void print(const char* text);
	void write(char byt);
	void flush();
};
