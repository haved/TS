#include "GameLogic.hpp"
#include "ArduinoInterface.hpp"

#define OCEAN_BG CRGB(30, 30, 130)
#define OCEAN_BG2 CRGB(60, 70, 170)

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
