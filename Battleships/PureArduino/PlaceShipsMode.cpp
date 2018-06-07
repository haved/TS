#include "ArduinoInterface.hpp"
#include "GameLogic.hpp"

#define OCEAN_BG CRGB(80, 80, 255)

void drawOceanTile(int screen, int x, int y) {
	setTile(screen, x, y, OCEAN_BG);
}

void drawWholeOcean(int screen) {
	for(int x = 0; x < WIDTH; x++)
		for(int y = 0; y < HEIGHT; y++)
			drawOceanTile(screen, x, y);
}

void updateShipPlaceMode(bool first) {
	if(first) {
	    allScreens(drawWholeOcean(screen));
	}
}
