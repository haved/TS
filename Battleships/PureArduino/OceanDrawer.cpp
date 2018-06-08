#include "GameLogic.hpp"
#include "ArduinoInterface.hpp"

#define OCEAN_BG CRGB(50, 50, 180)
#define OCEAN_BG2 CRGB(90, 100, 220)

void drawOceanTile(int screen, int x, int y, int frame) {
	float brightSpotA = fmod(5+frame/(float)10, WIDTH)+WIDTH/2;
	float brightSpotB = brightSpotA-WIDTH;
	float xSub = fmin(fabs(x-brightSpotA), fabs(x-brightSpotB))/2;
	setTile(screen, x, y, interpolate(OCEAN_BG, OCEAN_BG2, (3+y-xSub)/(HEIGHT+2)));
}

void drawWholeOcean(int screen, int frame) {
	for(int x = 0; x < WIDTH; x++)
		for(int y = 0; y < HEIGHT; y++)
			drawOceanTile(screen, x, y, frame);
}
