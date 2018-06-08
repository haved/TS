#include "ArduinoInterface.hpp"
#include "GameLogic.hpp"
#include "Battleship.hpp"

#define B21 Boat{2, 1, {100, 250, 120}}
#define B31 Boat{3, 1, {250, 250, 120}}
#define B41 Boat{4, 1, {200, 250, 250}}
#define B22 Boat{2, 2, {250, 10, 100}}
const Boat DEFAULT_BOATS[] = {B21, B21, B31, B41, B22};
#define BOAT_COUNT (int)(sizeof(DEFAULT_BOATS)/sizeof(*DEFAULT_BOATS))

Boat boats[2][BOAT_COUNT];
int boatsPlaced[2] = {0,0};

bool twoPlayer;
void configureShipPlaceMode(bool twoPlayer_p) {
	twoPlayer = twoPlayer_p;
	boatsPlaced[0] = boatsPlaced[1] = 0;
	for(int i = 0; i < BOAT_COUNT; i++)
		boats[0][i] = boats[1][i] = DEFAULT_BOATS[i];
}

#define OCEAN_BG CRGB(50, 50, 180)
#define OCEAN_BG2 CRGB(90, 100, 220)

inline void drawOceanTile(int screen, int x, int y) {
	float brightSpotA = fmod(5+frameCount/(float)10, WIDTH)+WIDTH/2;
	float brightSpotB = brightSpotA-WIDTH;
	float xSub = fmin(fabs(x-brightSpotA), fabs(x-brightSpotB))/2;
	setTile(screen, x, y, interpolate(OCEAN_BG, OCEAN_BG2, (3+y-xSub)/(HEIGHT+2)));
}

inline void drawWholeOcean(int screen) {
	for(int x = 0; x < WIDTH; x++)
		for(int y = 0; y < HEIGHT; y++)
			drawOceanTile(screen, x, y);
}

#define OVERLAP_BOAT_COLOR1 CRGB(255, 0, 0)
#define OVERLAP_BOAT_COLOR2 CRGB(255, 255, 150)
#define HOVER_BOAT_COLOR2 CRGB(200, 240, 100)
bool handlePlayerBoatPlacement(int player_global) {
	assert(player_global == PLAYER1 || player_global == PLAYER2);

	int p = player_global == PLAYER2 ? 1 : 0;

	int& placed = boatsPlaced[p];
    for(int i = 0; i < placed; i++)
		boats[p][i].render(player_global+DEF);

	int* buttons = framesHeld.raw+(p*BTN_OFFSET_P2);
	if(placed > 0 && clicked(buttons[BUTTON_MENU]))
		placed--;

	if(placed == BOAT_COUNT)
		return true;

	Boat& placing = boats[p][placed];
	Boat moving = placing;

	if(clicked(buttons[BUTTON_UP]))
		moving.y--;
	if(clicked(buttons[BUTTON_DOWN]))
		moving.y++;
	if(clicked(buttons[BUTTON_LEFT]))
		moving.x+=LEFT(player_global);
	if(clicked(buttons[BUTTON_RIGHT]))
		moving.x+=RIGHT(player_global);
	if(clicked(buttons[BUTTON_B]))
		moving.rotate();

	if(moving.equals(placing));
	else if(!moving.inBounds()); //TODO: Play sound
	else
		placing = moving;

	bool overlapping = false;
	for(int i = 0; i < placed && !overlapping; i++)
		overlapping |= placing.overrides(boats[p][i]);

    CRGB col1 = overlapping ? OVERLAP_BOAT_COLOR1 : placing.color;
	CRGB col2 = overlapping ? OVERLAP_BOAT_COLOR2 : HOVER_BOAT_COLOR2;

	float flashSlow = overlapping ? 5.f : 7.f;

	boats[p][placed].render(player_global+DEF, interpolate(col1, col2, sin(frameCount/flashSlow)/2.f+.5f));

	if(clicked(buttons[BUTTON_A])) {
		if(overlapping); //TODO Play error sound
		else
			placed++;
	}

	return false;
}

void updateShipPlaceMode(bool first) {
    allScreens(drawWholeOcean(screen));

	bool done = true;

	if(!twoPlayer)
		drawP2AIText(CRGB(140, 200, 100));
	else
		done &= handlePlayerBoatPlacement(PLAYER2);
	done &= handlePlayerBoatPlacement(PLAYER1);

	if(done)
		heavyTransitionTo(MENU_MODE, 100);
}
