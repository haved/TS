#include "SerialIO.hpp"
#include <string>
#include <iostream>
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

SerialIO::SerialIO() : m_serial() {
	std::string wantedTTY = "/dev/"+getWantedTTY();
	m_serial.open(wantedTTY, std::ios::out | std::ios::in | std::ios::binary);
	if(!m_serial.is_open()) throw std::runtime_error("Failed to open: " + wantedTTY);
}

SerialIO::~SerialIO() {
	m_serial.close();
}

void SerialIO::doRead(int preserve) {
	int maxReadLength = std::min(SERIAL_BUFFER_SIZE-m_writtenUntil, SERIAL_BUFFER_SIZE-preserve+1);
	int read = m_serial.readsome(&m_circleBuffer[m_writtenUntil], maxReadLength);
	m_writtenUntil = (m_writtenUntil+read)%SERIAL_BUFFER_SIZE;
}

bool SerialIO::find(const char* text) {
	int textLen = strlen(text);
	assert(textLen > 0);
	assert(textLen < SERIAL_BUFFER_SIZE); //We can never read the full buffer

    auto buf = [&](int x) {
		x %= SERIAL_BUFFER_SIZE;
		x += SERIAL_BUFFER_SIZE;
		x %= SERIAL_BUFFER_SIZE;
		return m_circleBuffer[x];
	};

	while(true) {
		int unread = (m_writtenUntil - m_readUntil + SERIAL_BUFFER_SIZE) % SERIAL_BUFFER_SIZE;
	    for(int i = -textLen+1; i <= unread-textLen; i++) {
			for(int j = 0; j < textLen; j++) {
				if(buf(m_readUntil+i+j) != text[j])
					goto outer;
			}
			m_readUntil = (i+textLen)%SERIAL_BUFFER_SIZE; //We now start reading from after what we found
			return true;
		outer:;
		}

		m_readUntil = m_writtenUntil;
		doRead(textLen);

		if(m_readUntil == m_writtenUntil) //No more to look at
			return false;
	}
}

char SerialIO::waitForByte() {
	if(m_readUntil != m_writtenUntil) {
		m_readUntil++;
		m_readUntil %= SERIAL_BUFFER_SIZE;
	}
	char c = m_serial.get();
	return c;
}

void SerialIO::print(const char* c) {
	m_serial << c;
}

void SerialIO::write(char byt) {
	m_serial << byt;
}
