#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#define PI M_PI
#include <inttypes.h>
using channel = uint8_t;
class CRGB {
public:
	channel r, g, b;
	CRGB() : r(0), g(0), b(0) {}
	CRGB(channel r, channel g, channel b) : r(r), g(g), b(b) {}

	static CRGB White; //These are defined in Util.cpp in PureArduinoSim
	static CRGB Black;
	static CRGB Red;
	static CRGB Green;
	static CRGB Blue;
};
