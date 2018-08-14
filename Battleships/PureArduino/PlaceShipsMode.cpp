#include "ArduinoInterface.hpp"
#include "GameLogic.hpp"
#include "Battleship.hpp"
#include "LotsOfHeader.hpp"

#define B21 Boat{2, 1, {200, 160, 20}}
#define C21 Boat{2, 1, {250, 180, 20}}
#define D21 Boat{2, 1, {200, 200, 20}}
#define B31 Boat{3, 1, {200, 160, 230}}
#define C31 Boat{3, 1, {250, 200, 120}}
#define D31 Boat{3, 1, {160, 240, 120}}
#define B41 Boat{4, 1, {230, 100, 150}}
#define B51 Boat{5, 1, {250, 150, 150}}
const Boat DEFAULT_BOATS[] = {B21, C21, D21, B31, C31, D31, B41, B51};
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

#define OVERLAP_BOAT_COLOR1 CRGB(255, 0, 0)
#define OVERLAP_BOAT_COLOR2 CRGB(255, 255, 150)
#define HOVER_BOAT_COLOR1 CRGB(255, 100, 100)
#define HOVER_BOAT_COLOR2 CRGB(250, 250, 100)
bool handlePlayerBoatPlacement(int player_global, bool* redraw, bool* overlapping) {
	assert(player_global == PLAYER1 || player_global == PLAYER2);

	int p = player_global == PLAYER2 ? 1 : 0;

	int& placed = boatsPlaced[p];

	int* buttons = framesHeld.raw+(p*BTN_OFFSET_P2);
	if(placed > 0 && clicked(buttons[BUTTON_MENU])) {
		placed--;
		playSoundEffect(SOUND_ILLEGAL_ACTION);
		*redraw = true;
	}

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
	else if(!moving.inBounds())
		playSoundEffect(SOUND_ILLEGAL_MOVE);
	else {
		placing = moving; //TODO Play move sound
		*redraw = true;
	}

	*overlapping = false;
	for(int i = 0; i < placed && !*overlapping; i++)
		*overlapping |= placing.overrides(boats[p][i]);

	if(clicked(buttons[BUTTON_A])) {
		if(*overlapping) {
			playSoundEffect(SOUND_ILLEGAL_ACTION);
		}
		else {
			placed++;
			playSoundEffect(SOUND_ACTION);
			*redraw = true;
		}
	}

	return false;
}

void drawBoats(int player_global) {
	int p = player_global == PLAYER2 ? 1 : 0;

	int& placed = boatsPlaced[p];
    for(int i = 0; i < placed; i++)
		boats[p][i].render(player_global+DEF);
}

void drawPlayerBoatPlacement(int player_global, bool overlapping) {
	assert(player_global == PLAYER1 || player_global == PLAYER2);
	int p = player_global == PLAYER2 ? 1 : 0;

	int& placed = boatsPlaced[p];
	if(placed == BOAT_COUNT)
		return;
	Boat& placing = boats[p][placed];

	CRGB col1 = overlapping ? OVERLAP_BOAT_COLOR1 : HOVER_BOAT_COLOR1;
	CRGB col2 = overlapping ? OVERLAP_BOAT_COLOR2 : HOVER_BOAT_COLOR2;

	float flashSlow = overlapping ? 5.f : 7.f;

    placing.render(player_global+DEF, interpolate(col1, col2, sin(frameCount/flashSlow)/2.f+.5f));
}

void updateShipPlaceMode(bool redraw) {
	bool done = true;
	bool p1Overlap, p2Overlap;

	done &= !twoPlayer || handlePlayerBoatPlacement(PLAYER2, &redraw, &p2Overlap);
	done &= handlePlayerBoatPlacement(PLAYER1, &redraw, &p1Overlap);

	if(redraw) {
		allScreens(drawWholeOcean(screen, 0));
		if(!twoPlayer)
			drawP2AIText(CRGB::Green);
		drawBoats(PLAYER1);
		drawBoats(PLAYER2);

		bothPlayers(clearLCD(player));
		bothPlayers(printLCDText(player, "Plasser skip:"));
		bothPlayers(setLCDPosition(player, 2, 1));
		printLCDNumber(PLAYER1, boatsPlaced[0]);
		printLCDNumber(PLAYER2, boatsPlaced[1]);
		bothPlayers(printLCDText(player, "/"));
		bothPlayers(printLCDNumber(player, BOAT_COUNT));
	}

	drawPlayerBoatPlacement(PLAYER1, p1Overlap);
	drawPlayerBoatPlacement(PLAYER2, p2Overlap);

	if(done) {
		configureBattleshipsMode(boats[0], BOAT_COUNT, boats[1], BOAT_COUNT, !twoPlayer);
		heavyTransitionTo(BS_GAME_MODE, 40);
		playSoundEffect(SOUND_DONE);
	}

	handleHoldEscToMenu();
}
