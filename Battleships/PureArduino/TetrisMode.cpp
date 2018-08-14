#include "ArduinoInterface.hpp"
#include "GameLogic.hpp"
#include "LotsOfHeader.hpp"
#include <stdint.h>

CRGB t_colors[] = {0x222222, 0xAA0000, 0x0000AA, 0x00AAAA, 0xAAAA00, 0xAA6600, 0x00AA00, 0xAA0066};
enum TetrisColor:int {BG, RED, BLUE, CYAN, YELLOW, ORANGE, GREEN, PINKK};

struct TetrisShape {
    int line1;
	int line2;
	TetrisColor color;
	int startX;
};

TetrisShape t_shapes[] = { //Lowest bit furthest to the left
						  {0b1111, 0, CYAN, 3}, //I
						  {0b111, 0b100, ORANGE, 4}, //L
						  {0b111, 0b1, BLUE, 4}, //J
						  {0b110, 0b11, GREEN, 4}, //S
						  {0b11, 0b110, RED, 4}, //Z
						  {0b11, 0b11, YELLOW, 4}, //O
						  {0b010, 0b111, PINKK, 4} //T
};
const int t_shapeCount = sizeof(t_shapes)/sizeof(*t_shapes);
const int MAX_LINE_LENGTH = 4;

const float LINE_CLEAR_FLASH_TIME = 60;
const float LVL1_FALL_TIME = 20;
const float LVL1_STICK_TIME = 40;

const int LINE_CLEAR_SCORE[] = {0, 500, 1500, 2500, 4000, 9999};

#define WIDTH 10
#define HEIGHT 10

bool t_player2;
struct PlayerData {
    uint16_t seed;

	TetrisColor board[WIDTH][HEIGHT];
	TetrisShape* currentShape;
	TetrisShape* nextShape;
	int currentRot;
	int currentXPos;
	int currentYPos;
	float timeLeftToGravity;

	bool onFloor;
	float timeLeftToStick;

	long fullLines = 0; //Bitmask
	float lineClearFlashLeft;

	int score;
	bool lost;

	int level;
	float givenTimeToFall;
	float givenTimeToStick;

} t_players[2];


void levelUp(PlayerData& player) {
	player.level++;
	player.givenTimeToFall = LVL1_FALL_TIME;
	player.givenTimeToStick = LVL1_STICK_TIME;
}

template<typename F>
void funcOnLine(F& func, int line, int lineX, int lineY, int dx, int dy) {
    for(int i = 0; i < MAX_LINE_LENGTH; i++) {
		bool filled = (line>>i) & 0x1;
		if(filled) {
			int x = lineX + i*dx;
			int y = lineY + i*dy;
			func(x, y);
		}
	}
}

template<typename F>
void funcOnPiece(PlayerData& player, F& func) {
    funcOnLine(func, player.currentShape->line1, player.currentXPos, player.currentYPos,   1, 0);
	funcOnLine(func, player.currentShape->line2, player.currentXPos, player.currentYPos+1, 1, 0);
}

bool legalPos(PlayerData& player) {
	bool illegal = false;

	auto checker = [&](int x, int y) {
					   illegal |= x < 0 || x >= WIDTH || y >= HEIGHT || y < 0 || player.board[x][y];
				   };

	funcOnPiece(player, checker);

	return !illegal;
}

TetrisShape* genRandPiece(uint16_t& seed) {

	uint16_t bit  = ((seed >> 0) ^ (seed >> 2) ^ (seed >> 3) ^ (seed >> 5) ) & 1;
	seed = (seed >> 1) | (bit << 15);
	return &t_shapes[seed%t_shapeCount];
}

void newPiece(PlayerData& player) {
    if(!player.nextShape)
		player.nextShape = genRandPiece(player.seed);
	player.currentShape = player.nextShape;
	player.nextShape = genRandPiece(player.seed);

	player.currentRot = 0;
	player.currentXPos = player.currentShape->startX;
	player.currentYPos = 0;
	player.timeLeftToGravity = player.givenTimeToFall;
	player.onFloor = false;

	player.lost = !legalPos(player);
}

void resetPlayer(PlayerData& player, int seed) {
	player.seed = seed;
	for(int x = 0; x < WIDTH; x++) {
		for(int y = 0; y < HEIGHT; y++) {
			player.board[x][y] = BG;
		}
	}

	player.level = 0;
	levelUp(player);

	player.currentShape = nullptr;
	player.nextShape = nullptr;
	newPiece(player);

	player.lost = false;
	player.score = 0;
	player.fullLines = 0; //Bitmask
}

void configureTetrisMode(bool player2) {
	t_player2 = player2;
	resetPlayer(t_players[0], frameCount);
	resetPlayer(t_players[1], frameCount);
}

void checkIfCleared(PlayerData& player) {
	player.fullLines = 0; //Lowest bit is highest Y
	for(int y = 0; y < HEIGHT; y++) {
		player.fullLines <<= 1;
		int x;
		for(x = 0; x < WIDTH && player.board[x][y]; x++);
		if(x == WIDTH)
			player.fullLines += 1;
	}
	if(player.fullLines != 0)
		player.lineClearFlashLeft = LINE_CLEAR_FLASH_TIME;
}

void removeClearedLines(PlayerData& player) {
	int mask = player.fullLines;

	int clearedLines = 0;

	for(int y = HEIGHT-1; y >= 0; y--) {
		bool cleared = mask & 0x1;
		mask >>= 1;
		if(cleared) {
			clearedLines++;
			for(int y2 = y; y2 > 0; y2--) {
				for(int x = 0; x < WIDTH; x++)
					player.board[x][y2] = player.board[x][y2-1];
			}
			for(int x = 0; x < WIDTH; x++)
				player.board[x][0] = BG;
			y++; //Do this line again
		}
	}
	player.fullLines = 0;
	playSoundEffect(SOUND_EXPLOTION);

	player.score += LINE_CLEAR_SCORE[clearedLines];
}

void flashPieceToBoard(PlayerData& player) {

	auto color = player.currentShape->color;
	auto flashFunc = [&](int x, int y) {
						 player.board[x][y] = color;
							 };
	funcOnPiece(player, flashFunc);
	playSoundEffect(SOUND_FIRE_GUN);

	player.currentShape = nullptr;
	checkIfCleared(player);
}

bool updatePlayer(PlayerData& player, int button_offset) {
	if(player.lost)
		return false;

	if(player.fullLines) {
		player.lineClearFlashLeft -= delta_time();
		if(player.lineClearFlashLeft <= 0)
			removeClearedLines(player);
		return true;
	}

	bool changed = false;

	if(!player.currentShape) {
		newPiece(player);
		changed = true;
	}

	if(clicked(framesHeld.raw[button_offset+BUTTON_LEFT])) {
		player.currentXPos -= 1;
		if(!legalPos(player))
			player.currentXPos += 1;
		else
			changed = true;
	}
	if(clicked(framesHeld.raw[button_offset+BUTTON_RIGHT])) {
		player.currentXPos += 1;
		if(!legalPos(player))
			player.currentXPos -= 1;
		else
			changed = true;
	}

	if(changed)
		player.onFloor = false;

	if(player.onFloor) {
		player.timeLeftToStick -= delta_time();
		if(player.timeLeftToStick <= 0) {
			flashPieceToBoard(player);
			changed = true;
		}
	} else {
		float fallSpeedModif = buttons.raw[button_offset+BUTTON_DOWN] ? 3 : 1;
		player.timeLeftToGravity-=delta_time()*fallSpeedModif;
		if(player.timeLeftToGravity <= 0) {
			player.timeLeftToGravity = player.givenTimeToFall;
			player.currentYPos += 1;
			if(!legalPos(player)) {
				player.currentYPos -= 1;
				player.onFloor = true;
				player.timeLeftToStick = player.givenTimeToStick;
			}
			else
				changed = true;
		}
	}

	return changed;
}

 void drawPiece(PlayerData& player, int screen) {
	 if(!player.currentShape)
		 return;
	 auto color = t_colors[player.currentShape->color];

	 auto drawer = [&](int x, int y) {
					   if(x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
						   setTile(screen, screen >= PLAYER2 ? WIDTH-x-1 : x, HEIGHT-1-y, color);
				   };

	 funcOnPiece(player, drawer);
}

void redrawBoard(PlayerData& player, int screen) {
    for(int y = 0; y < HEIGHT; y++) {
		int screenY = HEIGHT-1-y;
		bool flashLine = (player.fullLines >> screenY) & 0x1;
		bool hidden = (int)player.lineClearFlashLeft & 0x1;
		if(flashLine && hidden)
			fillRect(screen, 0, screenY, WIDTH, 1, t_colors[BG]);
		else
			for(int x = 0; x < WIDTH; x++) {
				setTile(screen, screen >= PLAYER2 ? WIDTH-1-x : x, screenY, t_colors[player.board[x][y]]);
			}
	}

	drawPiece(player, screen);
}

void updateTetrisMode(bool redraw) {
	if(redraw) {
		//TODO: loopMusic(MUSIC_TETRIS);
		fillScreen(PLAYER1+ATK, CRGB::Blue);
		fillScreen(PLAYER2+ATK, CRGB::Blue);
		fillScreen(PLAYER1+DEF, t_colors[BG]);
		fillScreen(PLAYER2+DEF, t_colors[BG]);
	}

	bool redrawP1 = updatePlayer(t_players[0], 0) || redraw;
	bool redrawP2 = t_player2 && (updatePlayer(t_players[1], BTN_OFFSET_P2) || redraw);

	if(redrawP1) {
		if(t_players[0].lost)
			fillScreen(PLAYER1+DEF, CRGB::Red);
		else
			redrawBoard(t_players[0], PLAYER1+ATK);
	}

	if(redrawP2) {
		if(t_players[1].lost)
			fillScreen(PLAYER2+DEF, CRGB::Red);
		else
			redrawBoard(t_players[1], PLAYER2+ATK);
	}

	handleHoldEscToMenu();
}
