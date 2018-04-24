#include <FastLED.h>

#define WIDTH 10
#define HEIGHT 10
#define LED_COUNT WIDTH*HEIGHT
#define PLAYER_1_DEFEND_DATA 24
#define PLAYER_2_DEFEND_DATA 26
#define PLAYER_1_ATTACK_DATA 25
#define PLAYER_2_ATTACK_DATA 27

CRGB player_1_defend_LEDS[LED_COUNT];
CRGB player_2_defend_LEDS[LED_COUNT];
CRGB player_1_attack_LEDS[LED_COUNT];
CRGB player_2_attack_LEDS[LED_COUNT];

void setup() {
  FastLED.addLeds<WS2812B, PLAYER_1_DEFEND_DATA, GRB>(player_1_defend_LEDS, LED_COUNT);
  FastLED.addLeds<WS2812B, PLAYER_2_DEFEND_DATA, GRB>(player_2_defend_LEDS, LED_COUNT);
  FastLED.addLeds<WS2812B, PLAYER_1_ATTACK_DATA, GRB>(player_1_attack_LEDS, LED_COUNT);
  FastLED.addLeds<WS2812B, PLAYER_2_ATTACK_DATA, GRB>(player_2_attack_LEDS, LED_COUNT);
}

#define MENU_MODE;
int mode = -1;

MenuMode menuMode;

void loop() {
  handleInput();

  static int prevMode;
  bool modeChanged = prevMode != mode;
  prevMode = mode;

  switch(mode) {
    case MENU_MODE:
    updateMaybeChangeToMode(menuMode, modeChanged);
  }
  delay(1000/60);
}

void handleInput() {
  
}

void fill(CRGB* leds, int count, CRGB value) {
  //TODO: Find FastLED function to do this
  for(int i = 0; i < count; i++)
    leds[i] = value;
}

void clearAllLedArrays() {
  fill(player_1_defend_LEDS, LED_COUNT, CRGB::Black);
  fill(player_2_defend_LEDS, LED_COUNT, CRGB::Black);
  fill(player_1_attack_LEDS, LED_COUNT, CRGB::Black);
  fill(player_2_attack_LEDS, LED_COUNT, CRGB::Black);
}

template<typename T>
void updateMaybeChangeToMode(T mode, bool changeTo);

template<typename T>
void updateMaybeChangeToMode(T mode, bool changeTo) {
  if(changeTo)
    mode.onChangeToMode();
  mode.updateMode();
}

class Mode {
public:
  void onChangeToMode();
  void updateMode();    
};

class MenuMode : public Mode {
public:
  void onChangeMode() override {
    clearAllLedArrays();
  }

  void updateMode() override {
    
  }
};

