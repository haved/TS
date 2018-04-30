#pragma once
#include <cstdint>

using channel = uint8_t;
class CRGB {
public:
	channel r, g, b;
	CRGB(channel r, channel g, channel b) : r(r), g(g), b(b) {}
};

extern CRGB WHITE;
extern CRGB RED;
extern CRGB GREEN;
extern CRGB BLUE;
extern CRGB BLACK;

CRGB interpolate(CRGB a, CRGB b, double x);
