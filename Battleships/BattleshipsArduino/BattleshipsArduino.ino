#include <FastLED.h>

#define WIDTH 10
#define HEIGHT 10
#define LED_COUNT WIDTH*HEIGHT

#define DATA_PIN_MIN 24
#define SCREEN_COUNT 4

//First player, then attack or defend
const int SCREEN[2][2] = {{0, 1}, {2, 3}};
#define PLAYER1 0
#define PLAYER2 1
#define ATK 0
#define DEF 1

CRGB leds[SCREEN_COUNT][LED_COUNT];

#define BUTTON_PIN_MIN 30
#define BUTTON_COUNT 12
#define P2_BTN_OFFSET 6

const char BTN_CODE[] = {'U', 'D', 'L', 'R', 'A', 'S'};

#define IO Serial

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN_MIN+0, GRB>(leds[0], LED_COUNT);
  FastLED.addLeds<WS2812B, DATA_PIN_MIN+1, GRB>(leds[1], LED_COUNT);
  FastLED.addLeds<WS2812B, DATA_PIN_MIN+2, GRB>(leds[2], LED_COUNT);
  FastLED.addLeds<WS2812B, DATA_PIN_MIN+3, GRB>(leds[3], LED_COUNT);
  IO.setTimeout(1000/60);
}

void fill(CRGB* leds, int count, CRGB value) {
  //TODO: Find FastLED function to do this
  for(int i = 0; i < count; i++)
    leds[i] = value;
}

void setAllLedArrays(CRGB color) {
  for(int i = 0; i < SCREEN_COUNT; i++)
    fill(leds[i], LED_COUNT, color);
}

int getCoordForScreen(int x, int y, int screen) {
  //x=0 is Player 1's left, and Player 2's right
  //y=0 is "between" the attack and defend screen (i.e. +y is up on ATK, down on DEF)
  return x+y*WIDTH; //TODO
}

int waitForChar() {
  while(!IO.available());
  return IO.read();
}

CRGB readColor() {
  int r = waitForChar();
  int g = waitForChar();
  int b = waitForChar();
  return CRGB(r, g, b);
}

void loop() {
  if(IO.find("BS+")) {
    int c = waitForChar();
    if(c == 'P') {
      int playerId = waitForChar() == '1' ? PLAYER1 : PLAYER2;
      int atkDef = waitForChar() == 'A' ? ATK : DEF;
      int screen = SCREEN[playerId][atkDef];
      c = waitForChar();
      if(c == 'S') { //Single tile
        int x = waitForChar()-'0';
        int y = waitForChar()-'0';
        int coord = getCoordForScreen(x, y, screen);
        leds[screen][coord] = readColor();
      }
      FastLED.show();
    }
    if(c == 'C')
      setAllLedArrays(CRGB::Black);
  }

  static bool prevButtons[BUTTON_COUNT] = {};
  for(int i = 0; i < BUTTON_COUNT; i++) {
    bool state = digitalRead(BUTTON_PIN_MIN+i);
    if(state != prevButtons[i]) {
      IO.print("BS+BTN ");
      if(i >= P2_BTN_OFFSET) {
        IO.print("P2");
        IO.println(BTN_CODE[i-P2_BTN_OFFSET]);
      }
      else {
        IO.print("P1");
        IO.println(BTN_CODE[i]);
      }
      prevButtons[i] = state;
    }
  }
}
