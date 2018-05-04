#include "Util.hpp"

CRGB WHITE{255, 255, 255};
CRGB RED{255, 0, 0};
CRGB GREEN{0, 255, 0};
CRGB BLUE{0, 0, 255};
CRGB BLACK{0, 0, 0};

CRGB interpolate(CRGB a, CRGB b, double x) {
	double y = 1-x;
	return CRGB(a.r*x+b.r*y, a.g*x+b.g*y, a.b*x+b.b*y);
}
