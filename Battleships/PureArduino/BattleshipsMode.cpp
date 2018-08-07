#include "Battleship.hpp"
#include "LotsOfHeader.hpp"
#include "GameLogic.hpp"

Boat *p1Boats, *p2Boats;
int p1BoatCount, p2BoatCount;
int p1BoatsLeft, p2BoatsLeft;
bool p2AI;

//p1Attacks means where p1 has attacked
int p1Attacks[WIDTH][HEIGHT] = {};
int p2Attacks[WIDTH][HEIGHT] = {};

int x, y;
bool player2;

float hitCountdown;
float truceTime;

bool done;
bool player2Winner;

#define UNCHARTED 0
#define MISS 1
#define HIT 2
#define SUNK 3

void switchPlayer() {
	player2^=true;
	x = WIDTH/2;
	y = HEIGHT/2;
	truceTime = 0;
}

void configureBattleshipsMode(Boat* p1Boats, int p1BoatCount, Boat* p2Boats, int p2BoatCount, bool p2AI) {
	::p1Boats = p1Boats;
	::p1BoatCount = p1BoatCount;
	p1BoatsLeft = p1BoatCount;
	::p2Boats = p2Boats;
	::p2BoatCount = p2BoatCount;
	p2BoatsLeft = p2BoatCount;
	::p2AI = p2AI;

	truceTime = 0;
	hitCountdown = 0;
	done = false;
	player2 = true;
	switchPlayer();

	for(int x = 0; x < WIDTH; x++)
		for(int y = 0; y < HEIGHT; y++)
			p1Attacks[x][y] = p2Attacks[x][y] = UNCHARTED;

	loopMusic(MUSIC_PIRATE_MUSIC);
}

CRGB getColorOfATK(int hitStatus) {
	switch(hitStatus) {
	case MISS: return CRGB(0, 0, 0);
	case HIT: return CRGB(0, 255, 0);
	case SUNK: return CRGB(255, 0, 0);
	default: return CRGB(0,0,0);
	}
}

CRGB getColorOfDEF(int hitStatus) {
    return getColorOfATK(hitStatus);
}

#define screenForAtk ((player2?PLAYER2:PLAYER1) + ATK)
#define screenForDef ((player2?PLAYER1:PLAYER2) + DEF)
#define MARKER_SET_COLOR CRGB(0, 255, 0)
#define FLASH_COLOR CRGB(100, 100, 100)
#define SHOOT_TIME 30
#define FLASH_DURATION 50
bool handlePlayerTurn() {
	bool changed = false;

	auto& defBoats = player2 ? p1Boats   : p2Boats;
	int defBoatsCount = player2 ? p1BoatCount : p2BoatCount;
	int& boatsLeft = player2 ? p2BoatsLeft : p1BoatsLeft;
	auto& atkField = player2 ? p2Attacks : p1Attacks;

	if(hitCountdown > 0) {
		hitCountdown-=delta_time();

		if(hitCountdown <= 0) {
			hitCountdown = 0;
		    playSoundEffect(SOUND_EXPLOTION);
			fillScreen(screenForAtk, FLASH_COLOR);
			fillScreen(screenForDef, FLASH_COLOR);
			startTransition(screenForAtk, FLASH_DURATION);
			startTransition(screenForDef, FLASH_DURATION);

			atkField[x][y] = MISS;
		    for(int boat = 0; boat < defBoatsCount; boat++) {
				Boat& b = defBoats[boat];
				if(b.sunk)
					continue;
				if(b.hit(x, y)) {
					atkField[x][y] = HIT;
					if(b.maybeSink(atkField)) {
						b.colorSink(atkField, SUNK);
						if(--boatsLeft == 0) {
							done = true;
							player2Winner = !player2; //If all player2s boats are sunk
						}
					}
				}
			}

			switchPlayer();

			return true;
		}
	    return true;
	}

	truceTime+=delta_time();
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
		changed = true;

	bool shoot = clicked(buttons[BUTTON_A]) || (player2 && p2AI && truceTime > 100);
	if(shoot && !anyTransitionRunning()) {
		if(atkField[x][y] != UNCHARTED) {
		    playSoundEffect(SOUND_ILLEGAL_MOVE);
		} else {
			hitCountdown = SHOOT_TIME;
			playSoundEffect(SOUND_FIRE_GUN);
			setTile(screenForAtk, x, y, MARKER_SET_COLOR);
			setTile(screenForDef, x, y, MARKER_SET_COLOR);
	    }
	}

	return changed;
}

void updateBattleshipsMode(bool redraw) {
	static CRGB underAtkMarkerColor;
	static CRGB underDefMarkerColor;
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
		underAtkMarkerColor = getWrittenColor(screenForAtk, x, y);
		underDefMarkerColor = getWrittenColor(screenForDef, x, y);

		if(hitCountdown > 0) {

			setTile(screenForAtk, x, y, MARKER_SET_COLOR);
			setTile(screenForDef, x, y, MARKER_SET_COLOR);

			auto drawBullet = [&](int x, int y, CRGB color) {
								  if(y < 0)
									  return;
								  int defPlayer = (player2 ? PLAYER1 : PLAYER2);
								  int atkPlayer = (player2 ? PLAYER2 : PLAYER1);
								  if(y >= HEIGHT) {
									  y -= HEIGHT;
									  setTile(defPlayer+DEF, x, y, color);
									  setTile(atkPlayer+ATK, x, y, color);
								  } else {
									  y = HEIGHT-1-y;
									  setTile(defPlayer+ATK, x, y, color);
								  }
							  };

			static const CRGB bulletColor(255, 0, 0);
			static const CRGB bulletTrail(255, 255, 0);

			int shootDist = HEIGHT+y;
			float progress = (SHOOT_TIME-hitCountdown-10)/(float)(SHOOT_TIME-10);
			int bulletPixel = progress*shootDist;

			drawBullet(x, bulletPixel, bulletColor);
			drawBullet(x, bulletPixel-1, bulletTrail);
		}
	}

	if(done) {
		//TODO: Flash P1/P2 winner until button press
	} else if(!hitCountdown) {
		setTile(screenForAtk, x, y, interpolate(inverse(underAtkMarkerColor), underAtkMarkerColor, sin(frameCount/10.f)/2+.5f));
		setTile(screenForDef, x, y, interpolate(inverse(underDefMarkerColor), underDefMarkerColor, sin(frameCount/10.f)/2+.5f));
	}

	//TODO: Pause menu
	handleHoldEscToMenu();
}
