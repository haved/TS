#include "Battleship.hpp"
#include "LotsOfHeader.hpp"
#include "GameLogic.hpp"

#define BATTLESHIPS_DELTA_TIME 1

const int NUKE_REQ = 17;
#define MISS_PTS 3
#define HIT_PTS 1
#define SINK_PTS 0

Boat *p1Boats, *p2Boats;
int p1BoatCount, p2BoatCount;
int p1BoatsLeft, p2BoatsLeft;
bool p2AI;

//p1Attacks means where p1 has attacked
int p1Attacks[WIDTH][HEIGHT] = {};
int p2Attacks[WIDTH][HEIGHT] = {};

int x, y;
bool player2Turn;

bool firingNuke;
float hitCountdown;
float truceTime;

bool done;
bool player2Winner;

int p1TilesLeft, p2TilesLeft;
int p1Points, p2Points;

#define UNCHARTED 0
#define MISS 1
#define HIT 2
#define SUNK 3

void switchPlayer() {
	player2Turn^=true;
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
	p1Points = p2Points = 0;

    p1TilesLeft = p2TilesLeft = 0;
	for(int i = 0; i < p1BoatCount; i++)
		p1TilesLeft += p1Boats[i].width * p1Boats[i].height;
	for(int i = 0; i < p2BoatCount; i++)
		p2TilesLeft += p2Boats[i].width * p2Boats[i].height;

	truceTime = 0;
	hitCountdown = 0;
	done = false;
	player2Turn = true;
	switchPlayer();

	for(int x = 0; x < WIDTH; x++)
		for(int y = 0; y < HEIGHT; y++)
			p1Attacks[x][y] = p2Attacks[x][y] = UNCHARTED;

	fadeOutMusic(20);
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

bool canFireNuke() {
	bool pointReq = (player2Turn ? p2Points : p1Points) >= NUKE_REQ;
	return pointReq && x>0 && y>0 && x<WIDTH-1 && y<HEIGHT-1;
}

#define screenForAtk ((player2Turn?PLAYER2:PLAYER1) + ATK)
#define screenForDef ((player2Turn?PLAYER1:PLAYER2) + DEF)
#define MARKER_SET_COLOR CRGB(255, 255, 255)
#define NUKE_MARKER_SET_COLOR CRGB(255, 0, 255)
#define FLASH_COLOR CRGB(100, 100, 100)
#define SHOOT_TIME 30
#define FLASH_DURATION 50
#define FLASH_DURATION_SINK 100
#define NUKE_FLASH_DURATION 200
#define NUKE_SHOT_SLOWNESS 0.16f
bool handlePlayerTurn() {
	bool changed = false;

	auto& defBoats = player2Turn ? p1Boats   : p2Boats;
	int enemyDefBoatsCount = player2Turn ? p1BoatCount : p2BoatCount;
	int& enemyBoatsLeft = player2Turn ? p1BoatsLeft : p2BoatsLeft;
	auto& atkField = player2Turn ? p2Attacks : p1Attacks;
	int& defTilesLeft = player2Turn ? p1TilesLeft : p2TilesLeft;
	int& defNukePoints = player2Turn ? p1Points : p2Points;

	int& points = player2Turn ? p2Points : p1Points;

	if(hitCountdown > 0) {
		hitCountdown-=BATTLESHIPS_DELTA_TIME * (firingNuke ? NUKE_SHOT_SLOWNESS : 1);

		if(hitCountdown <= 0) {
			hitCountdown = 0;

			auto handleHit = [&](int x, int y, bool effects) {
								 if(atkField[x][y] != UNCHARTED)
									 return;
								 atkField[x][y] = MISS;
								 for(int boat = 0; boat < enemyDefBoatsCount; boat++) {
									 Boat& b = defBoats[boat];
									 if(b.sunk)
										 continue;
									 if(b.hit(x, y)) {
										 points += HIT_PTS;
										 atkField[x][y] = HIT;
										 if(--defTilesLeft == 1)
											 defNukePoints += NUKE_REQ;
										 if(b.maybeSink(atkField)) {
											 //Sunk
											 points += SINK_PTS;

											 if(effects) {
												 playSoundEffect(SOUND_EXPLOTION_SINK);
												 fillScreen(screenForAtk, FLASH_COLOR);
												 fillScreen(screenForDef, FLASH_COLOR);
												 startTransition(screenForAtk, FLASH_DURATION_SINK);
												 startTransition(screenForDef, FLASH_DURATION_SINK);
											 }

											 b.colorSink(atkField, SUNK);
											 if(--enemyBoatsLeft == 0) {
												 done = true;
												 player2Winner = player2Turn;
											 }

										 } else if(effects) {
											 playSoundEffect(SOUND_EXPLOTION_HIT);
											 fillScreen(screenForAtk, FLASH_COLOR);
											 fillScreen(screenForDef, FLASH_COLOR);
											 startTransition(screenForAtk, FLASH_DURATION);
											 startTransition(screenForDef, FLASH_DURATION);
										 }
									 }
								 }
								 if(atkField[x][y] == MISS)
									 points += MISS_PTS;

							 };

			if(firingNuke) {
				int newPoints = points - NUKE_REQ;
				for(int i = -1; i <= 1; i++) {
					for(int j = -1; j <= 1; j++) {
						handleHit(x+i, y+j, false);
					}
				}
				points = newPoints;
				stopSoundEffects();
				playSoundEffect(SOUND_NUKE_HIT);
				allScreens(fillScreen(screen, FLASH_COLOR));
				allScreens(startTransition(screen, NUKE_FLASH_DURATION));
			} else {
				handleHit(x, y, true);
			}

			switchPlayer();

			return true;
		}
	    return true;
	}

	truceTime+=BATTLESHIPS_DELTA_TIME;
    int* buttons = framesHeld.raw+(player2Turn*BTN_OFFSET_P2);

	int oldX = x, oldY = y;
	if(clicked(buttons[BUTTON_LEFT]))
		x+=LEFT(player2Turn);
	if(clicked(buttons[BUTTON_RIGHT]))
		x+=RIGHT(player2Turn);
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

	bool shoot = pressed(buttons[BUTTON_A]) || (player2Turn && p2AI && truceTime > 100);
	if(shoot && !anyTransitionRunning()) {
		if(atkField[x][y] != UNCHARTED) {
		    playSoundEffect(SOUND_ILLEGAL_MOVE);
		} else {
			hitCountdown = SHOOT_TIME;
			firingNuke = false;
			playSoundEffect(SOUND_FIRE_GUN);
			changed = true;
	    }
	}

	bool nuke_try = pressed(buttons[BUTTON_B]);
	if(nuke_try && !anyTransitionRunning()) {
		if(!canFireNuke()) {
			playSoundEffect(SOUND_ILLEGAL_MOVE);
		} else {
			hitCountdown = SHOOT_TIME;
			firingNuke = true;
			playSoundEffect(SOUND_FIRE_GUN);
			playSoundEffect(SOUND_FIRE_NUKE);
			changed = true;
		}
	}

	return changed;
}

#define SHOOT_AIM_COLOR 0xFF3333
#define NUKE_AIM_COLOR 0x0000FF
void updateBattleshipsMode(bool redraw) {

	if(redraw) {
		loopMusic(MUSIC_PIRATE_MUSIC);
		bothPlayers(clearLCD(player));
	}

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

			if(firingNuke) {
				fillRect(screenForAtk, x-1, y-1, 3, 3, NUKE_MARKER_SET_COLOR);
				fillRect(screenForDef, x-1, y-1, 3, 3, NUKE_MARKER_SET_COLOR);
			} else {
				setTile(screenForAtk, x, y, MARKER_SET_COLOR);
				setTile(screenForDef, x, y, MARKER_SET_COLOR);
			}

			auto drawBullet = [&](int x, int y, CRGB color) {
								  if(y < 0)
									  return;
								  int defPlayer = (player2Turn ? PLAYER1 : PLAYER2);
								  int atkPlayer = (player2Turn ? PLAYER2 : PLAYER1);
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
		int winnerScreen = player2Winner ? PLAYER2+ATK : PLAYER1+ATK;
	    int loserScreen = player2Winner ? PLAYER1+ATK : PLAYER2+ATK;

	    const CRGB HEART_COLOR = interpolate(0xFFFF00, 0x00FFFF, sin(frameCount/20.f)*0.4f+.6f);

		fillRect(winnerScreen, 0, 4, 5, 1, HEART_COLOR, NORMAL_COORDS);
		fillRect(winnerScreen, 0, 6, 5, 1, HEART_COLOR, NORMAL_COORDS);
		fillRect(winnerScreen, 1, 3, 1, 5, HEART_COLOR, NORMAL_COORDS);
		fillRect(winnerScreen, 3, 3, 1, 5, HEART_COLOR, NORMAL_COORDS);
		fillRect(winnerScreen, 6, 1, 2, 7, HEART_COLOR, NORMAL_COORDS);
		fillRect(winnerScreen, 5, 8, 4, 1, HEART_COLOR, NORMAL_COORDS);
		setTile(winnerScreen, 5, 2, HEART_COLOR, NORMAL_COORDS);

		const CRGB CATHULU_GREEN = interpolate(0x00AA33, 0x000000, sin(frameCount/20.f)*0.4f+.6f);
		const CRGB CATHULU_EYES = 0xFF4422;
		fillRect(loserScreen, 3, 1, 4, 1, CATHULU_GREEN, NORMAL_COORDS);
		fillRect(loserScreen, 2, 2, 6, 4, CATHULU_GREEN, NORMAL_COORDS);
		fillRect(loserScreen, 3, 3, 1, 2, CATHULU_EYES, NORMAL_COORDS);
		fillRect(loserScreen, 6, 3, 1, 2, CATHULU_EYES, NORMAL_COORDS);
		fillRect(loserScreen, 3, 6, 1, 2, CATHULU_GREEN, NORMAL_COORDS);
		fillRect(loserScreen, 6, 6, 1, 2, CATHULU_GREEN, NORMAL_COORDS);
		setTile(loserScreen, 2, 8, CATHULU_GREEN, NORMAL_COORDS);
		setTile(loserScreen, 7, 8, CATHULU_GREEN, NORMAL_COORDS);
	} else if(!hitCountdown) {
		if(canFireNuke()) {
			fillRect(screenForAtk, x-1, y-1, 3, 3, interpolate(NUKE_AIM_COLOR, 0xFF00FF, sin(frameCount/10.f)/2+.5f));
			fillRect(screenForDef, x-1, y-1, 3, 3, interpolate(NUKE_AIM_COLOR, 0xFF00FF, sin(frameCount/10.f)/2+.5f));
		}
		setTile(screenForAtk, x, y, interpolate(SHOOT_AIM_COLOR, underAtkMarkerColor, sin(frameCount/10.f)/2+.5f));
		setTile(screenForDef, x, y, interpolate(SHOOT_AIM_COLOR, underDefMarkerColor, sin(frameCount/10.f)/2+.5f));
	}

	handleHoldEscToMenu();
}
