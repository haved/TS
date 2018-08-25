#include "ArduinoInterface.hpp"
#include "GameLogic.hpp"
#include "LotsOfHeader.hpp"

const float ACCEL = 0.18;
const float FRIC = 0.3;

const float WALK_SPEED = 0.4;

const float SPEED_MULT = 0.3;
const float GRAVITY = 0.1;
const float GRAVITY_HOLDING = 0.05;
const float JUMP_SPEED = -1.4;

const CRGB BG_COLOR = 0x222222;
const CRGB HEAD_COLOR = 0xDDAAAA;

const CRGB P1_COLOR = 0xFF0000;
const CRGB P2_COLOR = 0x0000FF;

const int START_HEALTH = 15;

struct PlayerData {
    float x, y;
	float xSpeed, ySpeed;
	bool onFloor;
	bool facingRight;
	float stunLock;
	bool diving;
	CRGB color;

	int health;
	int points;

	PlayerData* other;

} c_players[2];

const int COUNTDOWN_STEPS = 6;
int countdown;
float countdown_sub;
const float COUNTDOWN_SUB_TIME = 20;
const float FIRST_COUNTDOWN_SUB_TIME = 50;


bool c_player2;

void resetPlayers() {
	c_players[0].x = 1.5;
	c_players[0].color = P1_COLOR;
	c_players[1].x = WIDTH-1.5;
	c_players[1].color = P2_COLOR;

	for(int i = 0; i < 2; i++) {
		c_players[i].y = HEIGHT-4;
		c_players[i].stunLock = 0;
		c_players[i].diving = false;
		c_players[i].health = START_HEALTH;
	}

	countdown = COUNTDOWN_STEPS;
	countdown_sub = FIRST_COUNTDOWN_SUB_TIME;
}

void configureCometMode(bool player2) {
	c_player2 = player2;

	c_players[0].other = &c_players[1];
	c_players[1].other = &c_players[0];

	resetPlayers();
}

template <typename T>
void runFuncOnPlayer(PlayerData& player, T& func) {
	int x = (int)player.x;
	int y = (int)player.y;
	int front = player.facingRight ? 1 : -1;
	if(player.stunLock > 0) {
		if(x == 0) {
			x=1;
			front = 1;
		}
		if(x == WIDTH-1) {
		    x=WIDTH-2;
			front = -1;
		}
		func(x, y, 0);
		func(x-front, y-1, 1);
		func(x, y-2, 2);
	} else if(player.diving) {
		func(x, y, 0);
		func(x-front, y-1, 1);
		func(x-front*2, y-2, 2);
	} else {
		func(x, y, 0);
		func(x, y-1, 1);
		func(x, y-2, 2);
	}
}

void takeDamage(PlayerData& hit) {
	playSoundEffect(frameCount % 3 == 0 ? SOUND_DK_HIT : SOUND_DK_HIT2);
	hit.health -= 1;
	if(hit.health <= 0) {
		hit.stunLock = 10000;
		hit.ySpeed = -20;
		hit.xSpeed = 0;
	}
}

const float STUN_TIME = 30;
const float HIT_BACK = 2.4;
const float HIT_LIFT = 2;
const float ATK_BACK = 5;
const float ATK_LIFT = 1;

const float DIVE_SPEED_X = 1;
const float DIVE_SPEED_Y = 1;

void registerHit(PlayerData& hit, PlayerData& attacker) {
	hit.stunLock = STUN_TIME;
	hit.xSpeed = HIT_BACK * (attacker.facingRight ? 1 : -1);
	hit.ySpeed = -HIT_LIFT;
	hit.diving = false;
	takeDamage(hit);

	attacker.xSpeed = ATK_BACK * (attacker.facingRight ? -1 : 1);
	attacker.ySpeed = -ATK_LIFT;
	attacker.diving = false;
}

bool handleOverlap() {
	bool overlap = false;

	auto onP1 = [&](int x1, int y1, int part) {
					auto onP2 = [&](int x2, int y2, int part) {
									overlap |= x1 == x2 && y1 == y2;
								};
					runFuncOnPlayer(c_players[1], onP2);
				};
	runFuncOnPlayer(c_players[0], onP1);

	if(overlap) {
	    bool p1Over = c_players[0].y < c_players[1].y;
		bool p1Dive = c_players[0].diving;
		bool p2Dive = c_players[1].diving;

		if(p1Over && p1Dive) {
			registerHit(c_players[1], c_players[0]);
			overlap = false;
		} else if(!p1Over && p2Dive) {
			registerHit(c_players[0], c_players[1]);
			overlap = false;
		}
	}

	return overlap;
}

void updateCPlayer(PlayerData& player, int button_offset) {

	bool stunned = player.stunLock > 0;

	if(!player.diving && !stunned)
		player.facingRight = player.x < player.other->x;

	if(stunned) {
		player.stunLock -= 1;
		player.xSpeed *= 1-FRIC;
		player.diving = false;
	}
	else if(player.onFloor) {
		player.xSpeed = 0;
		if(buttons.raw[button_offset+BUTTON_LEFT])
			player.xSpeed -= WALK_SPEED;
		if(buttons.raw[button_offset+BUTTON_RIGHT])
			player.xSpeed += WALK_SPEED;
	}
	else {
		if(buttons.raw[button_offset+BUTTON_LEFT])
			player.xSpeed -= ACCEL;
		if(buttons.raw[button_offset+BUTTON_RIGHT])
			player.xSpeed += ACCEL;
		player.xSpeed *= 1-FRIC;
	}

	if(framesHeld.raw[button_offset+BUTTON_A] == 1 && player.onFloor && !stunned) {
		playSoundEffect(SOUND_DK_JUMP);
		player.ySpeed = JUMP_SPEED;
	}

	if(!player.onFloor && !stunned && framesHeld.raw[button_offset+BUTTON_B] == 1) {
		playSoundEffect(SOUND_DK_DIVE);
		player.diving = true;
	}
	if(player.diving && (!buttons.raw[button_offset+BUTTON_B] || player.onFloor))
		player.diving = false;

	if(player.diving) {
		player.ySpeed = DIVE_SPEED_Y;
		player.xSpeed = DIVE_SPEED_X * (player.facingRight ? 1 : -1);
	}

	player.ySpeed += buttons.raw[button_offset+BUTTON_A] ? GRAVITY_HOLDING : GRAVITY;
	handleOverlap();

	player.x += player.xSpeed * SPEED_MULT;
	if(player.x < 0.5) {
		player.x = 0.5;
		player.xSpeed = 0;
	}
	else if(player.x >= WIDTH-0.5) {
		player.x = WIDTH-0.5;
		player.xSpeed = 0;
	}

	player.y += player.ySpeed * SPEED_MULT;
	if(player.y < 0.5) {
		player.y = 0.5;
		player.ySpeed = 0;
	}
	else if(player.y >= HEIGHT-0.5) {
	    player.y = HEIGHT-0.5;
		player.ySpeed = 0;
		player.onFloor = true;
	} else
		player.onFloor = false;
}

const float PUSH_TIME = 10;
const float PUSH_SPEED = 2;
void doAnyPushing() {
	if((int)c_players[0].x == (int)c_players[1].x && (int)c_players[0].y == (int)c_players[1].y
	   && c_players[0].stunLock <=0 && c_players[1].stunLock <= 0) {
		c_players[0].stunLock = PUSH_TIME;
		c_players[1].stunLock = PUSH_TIME;
		c_players[0].xSpeed += c_players[1].facingRight ? PUSH_SPEED : -PUSH_SPEED;
		c_players[1].xSpeed += c_players[1].facingRight ? -PUSH_SPEED : PUSH_SPEED;
	}
}

void drawPlayer(PlayerData& player, int screen) {
	auto func = [&](int x, int y, int part) {
					if(x < 0 || y < 0 || x >=WIDTH || y >= HEIGHT)
						return;
					setTile(screen, x, y, part == 2 ? HEAD_COLOR : player.color, NORMAL_COORDS);
				};
	runFuncOnPlayer(player, func);
}

void drawHealthBar(PlayerData& player, int xShift) {
	int wd = WIDTH/2;
	int hp = player.health;
	fillRect(PLAYER1+DEF, xShift, 0, wd, START_HEALTH/wd, 0xFFFF00, NORMAL_COORDS);
	fillRect(PLAYER2+DEF, xShift, 0, wd, START_HEALTH/wd, 0xFFFF00, NORMAL_COORDS);
	int full = hp/wd;
	fillRect(PLAYER1+DEF, xShift, START_HEALTH/wd-full, wd, full, player.color, NORMAL_COORDS);
	fillRect(PLAYER2+DEF, xShift, START_HEALTH/wd-full, wd, full, player.color, NORMAL_COORDS);
	int rest = hp%wd;
	fillRect(PLAYER1+DEF, xShift, START_HEALTH/wd-full-1, rest, 1, player.color, NORMAL_COORDS);
	fillRect(PLAYER2+DEF, xShift, START_HEALTH/wd-full-1, rest, 1, player.color, NORMAL_COORDS);
}

void updateCometMode(bool redraw) {
	if(countdown <= 1) {
		updateCPlayer(c_players[0], 0);
		updateCPlayer(c_players[1], BTN_OFFSET_P2);
		doAnyPushing();
	}

	fillScreen(PLAYER1+ATK, BG_COLOR);
	fillScreen(PLAYER2+ATK, BG_COLOR);

	for(int i = 0; i < 4; i++)
		drawPlayer(c_players[i%2], i/2 ? PLAYER1+ATK : PLAYER2+ATK);

	drawHealthBar(c_players[0],0);
	drawHealthBar(c_players[1],WIDTH/2);

	if(countdown) { //6 is nothing, 5 is one red, 4 is two, 3 is three, 2 is four, 1 they are green
		countdown_sub -= 1;
		if(countdown_sub <= 0) {
			countdown--;
			countdown_sub = COUNTDOWN_SUB_TIME;

			if(countdown > 1)
				playSoundEffect(SOUND_DONE);
			else if(countdown == 1) {
				playSoundEffect(SOUND_DONE);
				loopMusic(MUSIC_FIGHT);
			}


		}
		for(int i = PLAYER1; i <= PLAYER2; i += PLAYER2-PLAYER1) {
			CRGB color = 0xFF0000;
			switch(countdown) {
			case 6:
				break;
			case 1:
				color = 0x00FF00;
			case 2:
				setTile(i+ATK, 7, 1, color, NORMAL_COORDS);
			case 3:
				setTile(i+ATK, 5, 1, color, NORMAL_COORDS);
			case 4:
				setTile(i+ATK, 3, 1, color, NORMAL_COORDS);
			case 5:
				setTile(i+ATK, 1, 1, color, NORMAL_COORDS);
			}
		}
	}

	handleHoldEscToMenu();
}
