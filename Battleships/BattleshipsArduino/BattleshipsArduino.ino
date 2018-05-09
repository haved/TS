#include <FastLED.h>

#define WIDTH 10
#define HEIGHT 10
#define LED_COUNT WIDTH*HEIGHT

#define DATA_PIN_MIN 24
#define SCREEN_COUNT 4

//24: P1_ATK
//25: P1_DEF
//26: P2_ATK
//27: P2_DEF

#define PLAYER1 0
#define PLAYER2 2
#define ATK 0
#define DEF 1

#define BUTTON_PIN_MIN 30
#define P2_BTN_OFFSET 7
#define BUTTON_COUNT P2_BTN_OFFSET*2

CRGB leds[SCREEN_COUNT][LED_COUNT];
CRGB colorFrom[SCREEN_COUNT][LED_COUNT];
CRGB colorTo[SCREEN_COUNT][LED_COUNT];
int transProg[SCREEN_COUNT] = {};
int transGoal[SCREEN_COUNT] = {};
int transGoalSum = 0;
CRGB* writeToBuffer[SCREEN_COUNT] = {leds[0], leds[1], leds[2], leds[3]};

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

inline int getCoordForScreen(int x, int y, int screen) {
  //x=0 is Player 1's left, and Player 2's right
  //y=0 is "between" the attack and defend screen (i.e. +y is up on ATK, down on DEF)
  return x+y*WIDTH; //TODO
}

inline int getInternalScreenIndex(bool player2, bool attack) {
  return (player2 ? PLAYER2 : PLAYER1) + (attack ? ATK : DEF);
}

inline int waitForChar() {
  while(!IO.available());
  return IO.read();
}

inline CRGB readColor() {
  int r = waitForChar();
  int g = waitForChar();
  int b = waitForChar();
  return CRGB(r, g, b);
}

inline int readInternalScreen() {
  int byt = waitForChar()-'A';
  bool player2 = byt >= 2;
  bool attack = byt % 2 == 0;
  return getInternalScreenIndex(player2, attack);
}

inline void readXY(int* x, int* y) {
  int byt = waitForChar();
  *x = byt % WIDTH;
  *y = byt / WIDTH;
}

void handleButtonInput();

void loop() {
  static changesDone = 0;
  changesDone++;
  int byt = waitForChar();
  if(byt == 'S') { //Set single tile
    int screen = readInternalScreen();
    int x, y;
    readXY(&x, &y);
    int coord = getCoordForScreen(x, y, screen);
    writeToBuffer[screen][coord] = readColor();
  }
  else if(byt == 'R') { //Rectangle fill
    
  }
  else if(byt == 'T') {
    int screen = readInternalScreen();
    int frames = waitForByte();
    if(frames <= 0)
      frames = 1;
    transGoalSum -= transGoal[screen];
    transGoalSum += transGoal[screen]=frames;
    transProg[screen] = 0;
    memcpy(transitionFrom[screen], leds[screen], LED_COUNT*sizeof(CRGB));
    writeToBuffer[screen] = transitionTo[screen];
  }
  else if(byt == 'U') { // Repaint
    if(changesDone == 1 && transGoalSum == 0)
      goto doneWithUpdate;

    for(int screen = 0; screen < SCREEN_COUNT; screen++) {
      if(transGoal[screen] == 0)
        continue;
        
    }
    FastLED.show();

  doneWithUpdate:
    changesDone = 0;
    handleButtonInput();
    IO.print(">>"); //Have to tell them we are done
    IO.flush(); //                          - ASAP
  }
}

void handleButtonInput() {
  static bool buttonStates[BUTTON_COUNT];
  for(int i = 0; i < BUTTON_COUNT; i++) {
    bool but = digitalRead(BUTTON_PIN_MIN+i);
    if(but != buttonStates[i]) {
      IO.print(but ? ">D" : ">U");
      IO.write('A' + i); //How we do key codes
      buttonStates[i] = but;
    }
  }
}

