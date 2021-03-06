#include "SerialIO.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>

#include <iostream>
#include <string>
#include <cstring>

#define ERROR(s) do{ \
		std::cerr << "SerialIO:" << __LINE__ << ": error: " << s << std::endl; \
		throw std::runtime_error("Previous error was fatal");			       \
	} while(false)

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
	in = open(target, O_RDONLY);
	if(in == -1)
		ERROR( "Failed to open " << target << ": " << strerror(errno) );

	target = "/dev/stdout";
	out = open(target, O_WRONLY);
	if(out == -1)
		ERROR( "Failed to open " << target << ": " << strerror(errno) );
}
#else
SerialIO::SerialIO() {
	std::string target = "/dev/" + getWantedTTY();
	in = open(target.c_str(), O_RDWR | O_NOCTTY); // | O_NDELAY
	if(in == -1)
		ERROR( "Failed to open " << target << ": " << strerror(errno) );

	out = in;

	struct termios options;
	tcgetattr(in, &options);

	options.c_cflag |= (CLOCAL | CREAD);
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ISIG | IEXTEN );
	options.c_oflag &= ~(OPOST | ONLCR | OCRNL);
	options.c_iflag &= ~(INLCR | IGNCR | ICRNL | IGNBRK);

	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);

	options.c_cflag &= ~(CSIZE | CSTOPB);
	options.c_cflag |= CS8; //bytesize 8

	options.c_iflag &= ~(INPCK | ISTRIP);
	options.c_cflag &= ~(PARENB | PARODD | CMSPAR);

	options.c_iflag &= ~(IXON | IXOFF);

	options.c_cc[VTIME] = 0; //Minimum chars needed to be read

	tcsetattr(in, TCSANOW, &options);

	tcflush(in, TCIOFLUSH); //flush data written and recieved that isn't sent or read
}
#endif

SerialIO::~SerialIO() {
	close(in);
	if(in != out)
		close(out);
}

char SerialIO::waitForByte() {
	char c;
	int result = read(in, &c, 1);
	if(result == 0)
		ERROR( "EOF in Serial in" );
	else if(result == -1)
	    ERROR( "Serial in read failed: " << strerror(errno) );
#ifdef DEBUG_IO
	std::lock_guard<std::mutex> lock(loggingMutex);
	std::cerr << "Input: " << c << std::endl;
#endif
	return c;
}

void SerialIO::print(const char* c) {
	int len = strlen(c);
    int written = ::write(out, c, len);
	if(written == -1)
		ERROR( "Failed to write: " << strerror(errno) );
	if(written != len)
		ERROR( "Is output full? Wrote " << written << "/" << len );
#ifdef DEBUG_IO
	std::lock_guard<std::mutex> lock(loggingMutex);
	std::cerr << "Wrote: " << c << std::endl;
#endif
}

void SerialIO::write(char byt) {
    int written = ::write(out, &byt, 1);
	if(written == -1)
		ERROR( "Failed to write: " << strerror(errno) );
	if(written != 1)
		ERROR( "Is output full? Wrote " << written << "/1 bytes" );

#ifdef DEBUG_IO
	std::lock_guard<std::mutex> lock(loggingMutex);
	std::cerr << "Wrote (byte): " << byt << std::endl;
#endif
}

void SerialIO::flush() {
    //We don't need to flush a file descriptor, seing as it isn't buffered like a FILE*
}
