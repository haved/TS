#pragma once

#ifdef __AVR__
#include <FastLED.h>
void assert(bool val);
#else
#include "../PureArduinoSim/FakeArduino.hpp"
#endif

#define WIDTH 10
#define HEIGHT 10

#define LCD_WIDTH 16
#define LCD_LINES 2

#define PLAYER1 0
#define PLAYER2 2
#define ATK 0
#define DEF 1

enum CoordType {
				BS_COORDS,
				NORMAL_COORDS,
};
//24: P1_ATK
//25: P1_DEF
//26: P2_ATK
//27: P2_DEF

#define BUTTON_COUNT_P1 7
#define BTN_OFFSET_P2 BUTTON_COUNT_P1
#define BUTTON_COUNT BUTTON_COUNT_P1 * 2

#define BUTTON_UP 0
#define BUTTON_DOWN 1
#define BUTTON_LEFT 2
#define BUTTON_RIGHT 3
#define BUTTON_A 4
#define BUTTON_B 5
#define BUTTON_MENU 6

float delta_time();

template<class T>
struct ButtonState {
	T raw[BUTTON_COUNT];
	inline T* one() { return raw; }
	inline T* two() { return raw+BTN_OFFSET_P2; }
};

#define allScreens(content) \
	for(int screen = 0; screen < 4; screen++){content;}
#define bothPlayers(content) do { int player=PLAYER1; content; player=PLAYER2; content; } while(false)

void setTile(int screen, int x, int y, CRGB color, CoordType ct = BS_COORDS);
void fillRect(int screen, int x, int y, int width, int height, CRGB color, CoordType ct = BS_COORDS);
void fillScreen(int screen, CRGB color);
inline void fillAllScreens(CRGB color) {allScreens(fillScreen(screen, color));}
void startTransition(int screen, int frames);
inline void startTransitionAll(int frames) {allScreens(startTransition(screen, frames));}
void updateScreens();
bool anyTransitionRunning();
void getButtonStates(ButtonState<bool>& state);
CRGB getWrittenColor(int screen, int x, int y, CoordType ct = BS_COORDS);
CRGB getCurrentColor(int screen, int x, int y, CoordType ct = BS_COORDS);

void clearLCD(int player, bool home=true);
void setLCDPosition(int player, int line, int col); //1-indexed
void printLCDText(int player, const char* text);
void printLCDNumber(int player, int number);

#include "sound/SoundInterface.hpp"

#define up(c) (c)/*((c)*(c))*/
#define down(c) (c)/*sqrt(c)*/
inline CRGB interpolate(CRGB from, CRGB to, float frac) {
	float t = frac;
	float f = 1-t;
#define intr(ch) (uint8_t)down(up(from.ch)*f+up(to.ch)*t)
	return CRGB{intr(r), intr(g), intr(b)};

}

inline CRGB inverse(CRGB color) {
#define flipped(x) (int)(down(1-up(x/255.f))*255)
	return CRGB(flipped(color.r), flipped(color.g), flipped(color.b));
}
