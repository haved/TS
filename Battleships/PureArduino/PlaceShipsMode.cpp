#include "ArduinoInterface.hpp"
#include "GameLogic.hpp"

bool twoPlayer;
void configureShipPlaceMode(bool twoPlayer_p) {
	twoPlayer = twoPlayer_p;
}

#define OCEAN_BG CRGB(50, 50, 180)
#define OCEAN_BG2 CRGB(90, 100, 220)

inline void drawOceanTile(int screen, int x, int y) {
	float brightSpotA = fmod(5+frameCount/(float)10, WIDTH)+WIDTH/2;
	float brightSpotB = brightSpotA-WIDTH;
	float xSub = fmin(std::abs(x-brightSpotA), std::abs(x-brightSpotB))/2;
	setTile(screen, x, y, interpolate(OCEAN_BG, OCEAN_BG2, (3+y-xSub)/(HEIGHT+2)));
}

inline void drawWholeOcean(int screen) {
	for(int x = 0; x < WIDTH; x++)
		for(int y = 0; y < HEIGHT; y++)
			drawOceanTile(screen, x, y);
}

void updateShipPlaceMode(bool first) {
    allScreens(drawWholeOcean(screen));
	if(!twoPlayer)
		drawP2AIText(CRGB(140, 200, 100));
}
