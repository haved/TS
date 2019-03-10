#include "ArduinoInterface.hpp"
#include "GameLogic.hpp"
#include "LotsOfHeader.hpp"
#include <stdint.h>

CRGB t_colors[] =      {0x222222, 0xFF0000, 0x0000FF, 0x00FFFF, 0xFFFF00, 0xFF9900, 0x00FF00, 0xFF0099, 0x666666};
CRGB t_colors_lost[] = {0x222222, 0x999999, 0x999999, 0x999999, 0x999999, 0x999999, 0x999999, 0x999999, 0x555555};
enum TetrisColor:int {BG, RED, BLUE, CYAN, YELLOW, ORANGE, GREEN, PINKK, TRASH_TILE};

#define TETRIS_DELTA_TIME 1

struct TetrisShape {
    int line1;
	int line2;
	TetrisColor color;
	int width, height;
};

const TetrisShape t_shapes[] = { //Lowest bit furthest to the left
						  {0b0, 0b1111, CYAN, 4, 1}, //I
						  {0b111, 0b100, BLUE, 3, 2}, //J
						  {0b111, 0b1, ORANGE, 3, 2}, //L
						  {0b110, 0b11, GREEN, 3, 2}, //S
						  {0b11, 0b110, RED, 3, 2}, //Z
						  {0b11, 0b11, YELLOW, 2, 2}, //O
						  {0b111, 0b010, PINKK, 3, 2} //T
};

const int T_SHAPE_COUNT = sizeof(t_shapes)/sizeof(*t_shapes);
const int MAX_LINE_LENGTH = 4;

const float LOST_ANIMATION_SPEED = 0.05;
const float LINE_CLEAR_FLASH_TIME = 30;
const float LVL1_FALL_TIME = 20;
const float LVL1_STICK_TIME = 25;
const float DOWN_BUTTON_SPEED_MODIF = 5;

const int LINE_CLEAR_SCORE[] = {0, 100, 300, 500, 800, 9999};
const int TRASH_LINE_SEND[] = {0, 0, 0, 1, 2};

const int DEFAULT_ROT = 2;

const int SCORE_PER_LEVEL_1P = 400;
const int SCORE_PER_LEVEL_2P = 1000;

//#define WIDTH 10 //Already defined
//#define HEIGHT 10

bool t_player2;
struct PlayerData {
    uint16_t seed;
	TetrisShape const *(seven_bag[T_SHAPE_COUNT]);
	int seven_bag_left = 0;

	TetrisColor board[WIDTH][HEIGHT];
	const TetrisShape* currentShape;
	const TetrisShape* nextShape;
	const TetrisShape* holdShape;
	bool holdUsed;
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
    float lost_animation;

	int awaitingTrashLines;

	int level;
	float givenTimeToFall;
	float givenTimeToStick;

	PlayerData* otherPlayer;
} t_players[2];

void levelUp(PlayerData& player) {
	player.level++;
	player.givenTimeToFall = LVL1_FALL_TIME-player.level;
	player.givenTimeToStick = LVL1_STICK_TIME;
	//TODO: Play sound
}

inline int getScoreReqPerLevel() {
	return t_player2 ? SCORE_PER_LEVEL_2P : SCORE_PER_LEVEL_1P;
}

void addScore(PlayerData& player, int score) {
	player.score += score;
    while(player.level*getScoreReqPerLevel() < player.score)
		levelUp(player);
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
void funcOnPiece(const TetrisShape* shape, int x, int y, int rot, F& func) {
	int w = shape->width;

	switch(rot) {
	case 0:
		funcOnLine(func, shape->line1, x-w/2, y,   1, 0);
		funcOnLine(func, shape->line2, x-w/2, y+1, 1, 0);
		break;
	case 1:
		funcOnLine(func, shape->line1, x,   y-w/2, 0, 1);
		funcOnLine(func, shape->line2, x-1, y-w/2, 0, 1);
		break;
	case 2:
		funcOnLine(func, shape->line1, x+w/2, y,   -1, 0);
		funcOnLine(func, shape->line2, x+w/2, y-1, -1, 0);
		break;
	case 3:
		funcOnLine(func, shape->line1, x,   y+w/2, 0, -1);
		funcOnLine(func, shape->line2, x+1, y+w/2, 0, -1);
		break;
	}
}

bool legalPos(PlayerData& player) {
	bool illegal = false;

	auto checker = [&](int x, int y) {
					   illegal |= x < 0 || x >= WIDTH || y >= HEIGHT || (y >= 0 && player.board[x][y]);
				   };

	funcOnPiece(player.currentShape, player.currentXPos, player.currentYPos, player.currentRot, checker);

	return !illegal;
}

void resetPiece(PlayerData& player) {
	player.currentRot = DEFAULT_ROT;
	player.currentXPos = WIDTH/2-1; //Starting rot is 2
	player.currentYPos = 0; //Starting rot is 2
	player.timeLeftToGravity = player.givenTimeToFall;
	player.onFloor = false;
	player.holdUsed = false;

	player.lost = !legalPos(player);
}

uint16_t genRandNum(uint16_t& seed) {
	uint16_t bit  = ((seed >> 0) ^ (seed >> 2) ^ (seed >> 3) ^ (seed >> 5) ) & 1;
	seed = (seed >> 1) | (bit << 15);
	return seed;
}

template <typename T>
void swap(T& a, T& b) {
	auto tmp = a;
	a = b;
	b = tmp;
}

const TetrisShape* genRandPiece(PlayerData& player) {
	if(player.seven_bag_left == 0) {
		player.seven_bag_left = T_SHAPE_COUNT;
		for(int i = 0; i < T_SHAPE_COUNT; i++)
			player.seven_bag[i] = &t_shapes[i];

		//Shuffle
		for(int i = 0; i < T_SHAPE_COUNT; i++) {
			int x = genRandNum(player.seed)%(T_SHAPE_COUNT-i);
			swap(player.seven_bag[i], player.seven_bag[i+x]);
		}
	}

	player.seven_bag_left--;
	return player.seven_bag[player.seven_bag_left];
}

void newPiece(PlayerData& player) {
    if(!player.nextShape)
		player.nextShape = genRandPiece(player);
	player.currentShape = player.nextShape;
	player.nextShape = genRandPiece(player);

	resetPiece(player);
}

void resetPlayer(PlayerData& player, int seed) {
	player.seed = seed;
	player.seven_bag_left = 0;

	for(int x = 0; x < WIDTH; x++) {
		for(int y = 0; y < HEIGHT; y++) {
			player.board[x][y] = BG;
		}
	}

	player.level = 0;
	levelUp(player);

	player.currentShape = nullptr;
	player.nextShape = nullptr;

	player.lost = false;
	player.lost_animation = HEIGHT;
	player.score = 0;
	player.fullLines = 0; //Bitmask

	player.awaitingTrashLines = 0;

	newPiece(player);
}

void configureTetrisMode(bool player2) {
	t_players[0].otherPlayer = &t_players[1];
	t_players[1].otherPlayer = &t_players[0];

	t_player2 = player2;
	resetPlayer(t_players[0], frameCount);
	resetPlayer(t_players[1], frameCount);
}

bool checkIfCleared(PlayerData& player) {
	player.fullLines = 0; //Lowest bit is highest Y
	for(int y = 0; y < HEIGHT; y++) {
		player.fullLines <<= 1;
		int x;
		for(x = 0; x < WIDTH && player.board[x][y]; x++);
		if(x == WIDTH)
			player.fullLines += 1;
	}
	if(player.fullLines != 0) {
		player.lineClearFlashLeft = LINE_CLEAR_FLASH_TIME;
		playSoundEffect(SOUND_CLEAR4_TETRO);
		return true;
	}
	return false;
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

	addScore(player, LINE_CLEAR_SCORE[clearedLines]);

	int sendTrash = TRASH_LINE_SEND[clearedLines];
	while(sendTrash > 0 && player.awaitingTrashLines > 0) {
		sendTrash--;
		player.awaitingTrashLines--;
	}
	player.otherPlayer->awaitingTrashLines += sendTrash;
}

void flashPieceToBoard(PlayerData& player) {
	auto color = player.currentShape->color;
	auto flashFunc = [&](int x, int y) {
						 if(x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
							 player.board[x][y] = color;
							 };
	funcOnPiece(player.currentShape, player.currentXPos, player.currentYPos, player.currentRot, flashFunc);

	player.currentShape = nullptr;
	if(!checkIfCleared(player))
		playSoundEffect(SOUND_SLOW_HIT_TETRO);
}

int wallKickTries[][2] = {
						  {0,0},
						 {-1,0},
						 { 1,0},
						 {-1,1},
						 { 1,1},
						 {-1,-1},
						 { 1,-1},
						 { -2,0},
						 { 2,0}
};
const int wallKickTryCount = sizeof(wallKickTries)/sizeof(*wallKickTries);

bool tryWallKick(PlayerData& player) {
	int& X = player.currentXPos;
	int& Y = player.currentYPos;
	int oldX = X, oldY = Y;

	for(int i = 0; i < wallKickTryCount; i++) {
		player.currentXPos = oldX+wallKickTries[i][0];
		player.currentYPos = oldY+wallKickTries[i][0];
		if(legalPos(player))
			return true;
	}

	return false;
}

bool rotatePiece(PlayerData& player, int dir) {
	int oldRot = player.currentRot;
	int& X = player.currentXPos;
	int& Y = player.currentYPos;
	int oldX = X, oldY = Y;
	int width = player.currentShape->width;

	if(width % 2 == 0) {
		if(dir>0)
			switch(oldRot) {
			case 0: Y++; break;
			case 1: X--; break;
			case 2: Y--; break;
			case 3: X++; break;
			}
		else
			switch(oldRot) {
			case 1: Y--; break;
			case 2: X++; break;
			case 3: Y++; break;
			case 0: X--; break;
			}
	}

    player.currentRot+=4+dir;
	player.currentRot%=4;

	if(!tryWallKick(player)) {
		player.currentRot = oldRot;
		player.currentXPos = oldX;
		player.currentYPos = oldY;
		return false;
	}

	playSoundEffect(SOUND_ROTATE_TETRO);
	return true;
}

void addAnyTrashLines(PlayerData& player) {
	while(player.awaitingTrashLines) {
		player.awaitingTrashLines--;
		for(int y = 0; y<HEIGHT-1; y++) {
			for(int x = 0; x < WIDTH; x++) {
				player.board[x][y] = player.board[x][y+1];
			}
		}
		int hole = player.seed % WIDTH;
		for(int x = 0; x < WIDTH; x++)
			player.board[x][HEIGHT-1] = x == hole ? BG : TRASH_TILE;
	}
}

bool updatePlayer(PlayerData& player, int button_offset) {
	if(player.lost) {
		if(player.lost_animation > 0) {
			player.lost_animation -= delta_time() * LOST_ANIMATION_SPEED;
			return true;
		}
		return false;
	}

	if(player.fullLines) {
		player.lineClearFlashLeft -= TETRIS_DELTA_TIME;
		if(player.lineClearFlashLeft <= 0)
			removeClearedLines(player);
		return true;
	}

	bool changed = false;

	if(!player.currentShape) {
		addAnyTrashLines(player);
		newPiece(player);
		changed = true;
	}

	if(clicked(framesHeld.raw[button_offset+BUTTON_B]) && !player.holdUsed) {
		if(player.holdShape) {
		    auto tmp = player.holdShape;
			player.holdShape = player.currentShape;
			player.currentShape = tmp;
			resetPiece(player);
		} else {
			player.holdShape = player.currentShape;
			newPiece(player);
		}
		player.holdUsed = true;
		changed = true;
	}
	if(clicked(framesHeld.raw[button_offset+BUTTON_A])) {
	    changed |= rotatePiece(player, 1);
	}
	if(clicked(framesHeld.raw[button_offset+BUTTON_MENU])) {
	    changed |= rotatePiece(player, -1);
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

	float fallSpeedModif = buttons.raw[button_offset+BUTTON_DOWN] ? DOWN_BUTTON_SPEED_MODIF : 1;
	if(player.onFloor) {
		player.timeLeftToStick -= TETRIS_DELTA_TIME;
		if(player.timeLeftToStick <= 0) {
			flashPieceToBoard(player);
			changed = true;
		}
	} else {
		player.timeLeftToGravity-=TETRIS_DELTA_TIME*fallSpeedModif;
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

void drawPiece(const TetrisShape* shape, int x, int y, int rot, int screen) {
	if(!shape)
		return;
	auto color = t_colors[shape->color];

	auto drawer = [&](int x, int y) {
					  if(x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
						  setTile(screen, x, y, color, NORMAL_COORDS);
				  };

	funcOnPiece(shape, x, y, rot, drawer);

}

void drawNextPiece(PlayerData& player, int screen) {
	fillRect(screen, WIDTH/2-3, 1, 6, 4, CRGB::Black, NORMAL_COORDS);
	if(player.nextShape != nullptr)
		drawPiece(player.nextShape, WIDTH/2-1, 3, DEFAULT_ROT, screen);
}

void drawLevel(PlayerData& player, int screen) {
	fillRect(screen, 0, 0, WIDTH, 1, CRGB::White, NORMAL_COORDS);
	if(player.level <= 10)
		fillRect(screen, 0, 0, player.level, 1, CRGB::Red, NORMAL_COORDS);
	else if(player.level <= 20)
		fillRect(screen, 0, 0, player.level-10, 1, CRGB::Green, NORMAL_COORDS);
	else if(player.level <= 30)
		fillRect(screen, 0, 0, player.level-20, 1, CRGB::Blue, NORMAL_COORDS);
}

void drawHold(PlayerData& player, int screen) {
	fillRect(screen, 6, 6, 4, 4, CRGB::Red, NORMAL_COORDS);
	if(player.holdShape != nullptr)
		drawPiece(player.holdShape, 7, 8, DEFAULT_ROT, screen);
}

void redrawBoard(PlayerData& player, int screen) {
    for(int y = 0; y < HEIGHT; y++) {
		bool flashLine = (player.fullLines >> (HEIGHT-1-y)) & 0x1;
		bool hidden = (int)player.lineClearFlashLeft & 0x1;
		if(flashLine && hidden)
			fillRect(screen, 0, y, WIDTH, 1, t_colors[BG], NORMAL_COORDS);
		else {
			auto& palette = y > player.lost_animation ? t_colors_lost : t_colors;
			for(int x = 0; x < WIDTH; x++) {
				setTile(screen, x, y, palette[player.board[x][y]], NORMAL_COORDS);
			}
		}
	}

	if(player.currentShape && !player.lost)
		drawPiece(player.currentShape, player.currentXPos, player.currentYPos, player.currentRot, screen);
}

void updateTetrisMode(bool redraw) {
	if(redraw) {
		loopMusic(t_player2 ? MUSIC_TETRIS_2P : MUSIC_TETRIS);
		fillScreen(PLAYER1+ATK, CRGB::Blue);
		fillScreen(PLAYER2+ATK, CRGB::Blue);
		fillScreen(PLAYER1+DEF, t_colors[BG]);
		fillScreen(PLAYER2+DEF, t_colors[BG]);
	}

	bool redrawP1 = updatePlayer(t_players[0], 0) || redraw;
	bool redrawP2 = t_player2 && (updatePlayer(t_players[1], BTN_OFFSET_P2) || redraw);

	if(redrawP1) {
		redrawBoard(t_players[0], PLAYER1+ATK);
		drawNextPiece(t_players[0], PLAYER1+DEF);
		drawLevel(t_players[0], PLAYER1+DEF);
		drawHold(t_players[0], PLAYER1+DEF);
	}

	if(redrawP2) {
		redrawBoard(t_players[1], PLAYER2+ATK);
		drawNextPiece(t_players[1], PLAYER2+DEF);
		drawLevel(t_players[1], PLAYER2+DEF);
		drawHold(t_players[1], PLAYER2+DEF);
	}

	handleHoldEscToMenu();
}
