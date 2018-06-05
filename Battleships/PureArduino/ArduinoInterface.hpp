#ifdef Arduino_h
#include <FastLED.h>
#else

#define _USE_MATH_DEFINES
#include <cmath>
#define dtor(x) ((double)x/180*M_PI)
#define sin(x) sin(dtor(x))
#define cos(x) cos(dtor(x))
#define tan(x) tan(dtor(x))
#include <inttypes.h>
using channel = uint8_t;
class CRGB {
public:
	channel r, g, b;
	CRGB() : r(0), g(0), b(0) {}
	CRGB(channel r, channel g, channel b) : r(r), g(g), b(b) {}

	static CRGB White; //These are defined in Util.cpp in PureArduinoSim
	static CRGB Black;
	static CRGB Red;
	static CRGB Green;
	static CRGB Blue;
};

#endif

#define WIDTH 10
#define HEIGHT 10

#define PLAYER1 0
#define PLAYER2 2
#define ATK 0
#define DEF 1
//24: P1_ATK
//25: P1_DEF
//26: P2_ATK
//27: P2_DEF

#define BUTTON_COUNT_P1 7
#define BUTTON_COUNT BUTTON_COUNT_P1 * 2

#define BUTTON_UP 0
#define BUTTON_DOWN 1
#define BUTTON_LEFT 2
#define BUTTON_RIGHT 3
#define BUTTON_A 4
#define BUTTON_B 5
#define BUTTON_MENU 6

template<class T>
struct ButtonState {
	T raw[BUTTON_COUNT];
	inline T* one() { return raw; }
	inline T* two() { return raw+BUTTON_COUNT_P1; }
};

void setTile(int screen, int x, int y, CRGB color);
void fillRect(int screen, int x, int y, int width, int height, CRGB color);
void fillScreen(int screen, CRGB color);
inline void fillAllScreens(CRGB color) {
  for(int i = 0; i < 4; i++)
    fillScreen(i, color);
}
void startTransition(int screen, int frames);
void updateScreens();
bool anyTransitionRunning();
void getButtonStates(ButtonState<bool>& state);

inline int getInternalScreenIndex(bool player2, bool attack) {
	return (player2 ? PLAYER2 : PLAYER1) + (attack ? ATK : DEF);
}

inline CRGB interpolate(CRGB from, CRGB to, float frac) {
	float t = frac;
	float f = 1-t;
#define intr(ch) (uint8_t)(from.ch*f+to.ch*t)
	return CRGB{intr(r), intr(g), intr(b)};
}
