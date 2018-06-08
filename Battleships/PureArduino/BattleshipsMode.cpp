#include "Battleship.hpp"
#include "LotsOfHeader.hpp"
#include "GameLogic.hpp"

Boat *p1Boats, *p2Boats;
int p1BoatCount, p2BoatCount;

//p1Attacks means where p1 has attacked
int p1Attacks[WIDTH][HEIGHT] = {};
int p2Attacks[WIDTH][HEIGHT] = {};

int x, y;
bool player2;
bool done;
int winner;

#define UNCHARTED 0
#define MISS 1
#define HIT 2
#define SUNK 3

void switchPlayer() {
	player2^=true;
	x = WIDTH/2;
	y = HEIGHT/2;
}

void configureBattleshipsMode(Boat* p1Boats, int p1BoatCount, Boat* p2Boats, int p2BoatCount) {
	::p1Boats = p1Boats;
	::p1BoatCount = p1BoatCount;
	::p2Boats = p2Boats;
	::p2BoatCount = p2BoatCount;

	done = false;
	player2 = true;
	switchPlayer();

	for(int x = 0; x < WIDTH; x++)
		for(int y = 0; y < HEIGHT; y++)
			p1Attacks[x][y] = p2Attacks[x][y] = UNCHARTED;
}

CRGB getColorOfATK(int hitStatus) {
	switch(hitStatus) {
	case MISS: return CRGB(255, 255, 255);
	case HIT: return CRGB(0, 0, 0);
	case SUNK: return CRGB(255, 0, 0);
	default: return CRGB(0,0,0);
	}
}

CRGB getColorOfDEF(int hitStatus) {
	switch(hitStatus) {
	case MISS: return CRGB(255, 255, 255);
	case HIT: return CRGB(0, 0, 0);
	case SUNK: return CRGB(255, 0, 0);
	default: return CRGB(0,0,0);
	}
}

#define MARKER_COLOR_1 CRGB(255, 0, 0)
bool handlePlayerTurn() {
    int* buttons = framesHeld.raw+(player2*BTN_OFFSET_P2);

	int oldX = x, oldY = y;
	if(clicked(buttons[BUTTON_LEFT]))
		x+=LEFT(player2);
	if(clicked(buttons[BUTTON_RIGHT]))
		x+=RIGHT(player2);
	if(clicked(buttons[BUTTON_UP]))
		y++;
	if(clicked(buttons[BUTTON_DOWN]))
		y--;

	if(x==oldX && y == oldY);
	else if(x<0 || x>=WIDTH || y<0 || y >=HEIGHT) {
		playSoundEffect(SOUND_ILLEGAL_MOVE);
		x = oldX;
		y = oldY;
	} else
		return true; //We must repaint

	return false;
}

#define screenForAtk ((player2?PLAYER2:PLAYER1) + ATK)
void updateBattleshipsMode(bool redraw) {
	static CRGB underMarkerColor;
	if(redraw | (!done && handlePlayerTurn())) {
		allScreens(drawWholeOcean(screen, 0));
		for(int i = 0; i < p1BoatCount; i++)
			p1Boats[i].render(PLAYER1+DEF);
		for(int i = 0; i < p2BoatCount; i++)
			p2Boats[i].render(PLAYER2+DEF);

		for(int x = 0; x < WIDTH; x++) {
			for(int y = 0; y < HEIGHT; y++) {
				int p1A = p1Attacks[x][y];
			    if(p1A != UNCHARTED) {
					setTile(PLAYER1+ATK, x, y, getColorOfATK(p1A));
					setTile(PLAYER2+DEF, x, y, getColorOfDEF(p1A));
				}
				int p2A = p2Attacks[x][y];
			    if(p2A != UNCHARTED) {
					setTile(PLAYER2+ATK, x, y, getColorOfATK(p2A));
					setTile(PLAYER1+DEF, x, y, getColorOfDEF(p2A));
				}
			}
		}

		underMarkerColor = getWrittenColor(screenForAtk, x, y);
	}

	if(done) {
		//TODO: Flash P1/P2 winner until button press
	} else {
		setTile(screenForAtk, x, y, interpolate(MARKER_COLOR_1, underMarkerColor, sin(frameCount/10.f)/2+.5f));
	}

	//TODO: Pause menu
	handleHoldEscToMenu();
}
