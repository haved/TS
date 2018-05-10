#include "SerialIO.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>

#define ERROR(s) do{std::cerr << __FILE__ << ":" << __LINE__ << ": error: " << s << std::endl; throw std::runtime_error("Previous error was fatal"); } while(false)

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
SerialIO::SerialIO() {
	const char* target = "/dev/stdin";
	nonBlockingIn = open(target, O_RDONLY | O_NDELAY);
	if(nonBlockingIn == -1)
		ERROR( "Failed to open " << target << ": " << strerror(errno) );

	fcntl(nonBlockingIn, F_SETFL, FNDELAY);

	target = "/dev/stdout";
	out = open(target, O_WRONLY);
	if(out == -1)
		ERROR( "Failed to open " << target << ": " << strerror(errno) );
}
#else
SerialIO::SerialIO() {
	std::string target = getWantedTTY();
	nonBlockingIn = open(target.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
	if(nonBlockingIn == -1)
		ERROR( "Failed to open " << target << ": " << strerror(errno) );

	fcntl(nonBlockingIn, F_SETFL, FNDELAY);
	out = nonBlockingIn;

	const int TRASH_SIZE = 128;
	char trash[TRASH_SIZE];
	read(trash, TRASH_SIZE);
}
#endif

SerialIO::~SerialIO() {
	fcntl(nonBlockingIn, F_SETFL, 0);
	close(nonBlockingIn);
	if(nonBlockingIn != out)
		close(out);
}

int SerialIO::getByteIfReady() {
	char in;
	int result = read(&in, 1);
	if(result == 0)
		return -1;
	return in;
}

int SerialIO::read(char* buf, int maxLen) {
	int result = ::read(nonBlockingIn, buf, maxLen);
	if(result == 0)
		ERROR( "EOF in Serial in" );
	else if(result == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
	    ERROR( "Serial in read failed: " << strerror(errno) );
	/*if(result > 0)
	  std::cerr << "Got something" << std::endl;*/
	return std::max(result, 0);
}

void SerialIO::print(const char* c) {
	int len = strlen(c);
    int written = ::write(out, c, len);
	if(written == -1)
		ERROR( "Failed to write: " << strerror(errno) );
	if(written != len)
		ERROR( "Is output full? Wrote " << written << "/" << len );
}

void SerialIO::write(char byt) {
    int written = ::write(out, &byt, 1);
	if(written == -1)
		ERROR( "Failed to write: " << strerror(errno) );
	if(written != 1)
		ERROR( "Is output full? Wrote " << written << "/1 bytes" );
}

void SerialIO::flush() {
    //We don't need to flush a file descriptor, seing as it isn't buffered like a FILE*
}
