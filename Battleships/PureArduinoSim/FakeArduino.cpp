#include "FakeArduino.hpp"

CRGB CRGB::White = {255, 255, 255};
CRGB CRGB::Black = {0, 0, 0};
CRGB CRGB::Red = {255, 0, 0};
CRGB CRGB::Green = {0, 255, 0};
CRGB CRGB::Blue = {0, 0, 255};

int min(int a, int b) {
	return a<b?a:b;
}

int max(int a, int b) {
	return a>b?a:b;
}

