#include "ArduinoInterface.hpp"

#define LED_COUNT WIDTH*HEIGHT

#define DATA_PIN_MIN 24
#define SCREEN_COUNT 4

#define BUTTON_PIN_MIN 30

#define ARDUINO_FRAME_DIVISOR 3

CRGB leds[SCREEN_COUNT][LED_COUNT];
CRGB colorFrom[SCREEN_COUNT][LED_COUNT];
CRGB colorTo[SCREEN_COUNT][LED_COUNT];
int transProg[SCREEN_COUNT] = {};
int transGoal[SCREEN_COUNT] = {};
int transGoingMask = 0;
CRGB* writeToBuffer[SCREEN_COUNT] = {leds[0], leds[1], leds[2], leds[3]};

void assert(bool b);

#define IO Serial

void setup() {
  Serial.setTimeout(1000);
  Serial.begin(115200);

  FastLED.addLeds < WS2812B, DATA_PIN_MIN + 0, GRB > (leds[0], LED_COUNT);
  FastLED.addLeds < WS2812B, DATA_PIN_MIN + 1, GRB > (leds[1], LED_COUNT);
  FastLED.addLeds < WS2812B, DATA_PIN_MIN + 2, GRB > (leds[2], LED_COUNT);
  FastLED.addLeds < WS2812B, DATA_PIN_MIN + 3, GRB > (leds[3], LED_COUNT);

  for (int i = 0; i < BUTTON_COUNT; i++)
    pinMode(BUTTON_PIN_MIN + i, INPUT);
}

float delta_time() {
  return 2;
}

void fill(CRGB* leds, int count, CRGB value) {
  //TODO: Find FastLED function to do this
  for (int i = 0; i < count; i++)
    leds[i] = value;
}

void setAllLedArrays(CRGB color) {
  for (int i = 0; i < SCREEN_COUNT; i++)
    fill(leds[i], LED_COUNT, color);
}

void setScreenToFraction(int screen, float fraction) {
  for (int i = 0; i < LED_COUNT; i++)
    leds[screen][i] = interpolate(colorFrom[screen][i], colorTo[screen][i], fraction);
}

inline int getCoordForScreen(int x, int y, int screen) {
  assert(x >= 0 && y >= 0 && x < WIDTH && y < HEIGHT);

  //x=0 is Player 1's left, and Player 2's right
  //y=0 is "between" the attack and defend screen (i.e. +y is up on ATK, down on DEF)

  if(screen == PLAYER1 + ATK) {
    int tmp = x;
    x = y;
    y = tmp;  
  }

  if (screen == PLAYER2 + DEF)
    x = WIDTH - 1 - x;

  if (x % 2 == 1)
    y = HEIGHT - 1 - y;

  return y + x * HEIGHT;
}

void setTile(int screen, int x, int y, CRGB color) {
  int coord = getCoordForScreen(x, y, screen);
  writeToBuffer[screen][coord] = color;
}

void fillRect(int screen, int x1, int y1, int width, int height, CRGB color) {
  for (int x = x1; x < x1 + width; x++) {
    for (int y = y1; y < y1 + height; y++) {
      writeToBuffer[screen][getCoordForScreen(x, y, screen)] = color;
    }
  }
}

void fillScreen(int screen, CRGB color) {
  fill(writeToBuffer[screen], LED_COUNT, color);
}

void startTransition(int screen, int frames) {
  frames /= ARDUINO_FRAME_DIVISOR;
  if (frames <= 0)
    frames = 1;
  transProg[screen] = 0;
  transGoal[screen] = frames;
  transGoingMask |= 1 << screen;
  memcpy(colorFrom[screen], leds[screen], LED_COUNT * sizeof(CRGB));
  writeToBuffer[screen] = colorTo[screen];
}

void updateScreens() {
  for (int screen = 0; screen < SCREEN_COUNT; screen++) {
    if (((transGoingMask >> screen) & 0x1) == 0)
      continue;
    transProg[screen]++;
    float frac = transProg[screen] / (float) transGoal[screen];
    setScreenToFraction(screen, frac);
    if (transProg[screen] >= transGoal[screen]) {
      transGoingMask &= ~(1 << screen);
      writeToBuffer[screen] = leds[screen];
    }
  }
  FastLED.show();
  Serial.println("Update");
}

bool anyTransitionRunning() {
  return transGoingMask != 0;
}

void getButtonStates(ButtonState<bool>& state) {
  for (int i = 0; i < BUTTON_COUNT; i++) {
    bool but = digitalRead(BUTTON_PIN_MIN + i);
    state.raw[i] = but;
  }
}

CRGB getWrittenColor(int screen, int x, int y) {
  return writeToBuffer[screen][getCoordForScreen(x, y, screen)];
}
CRGB getCurrentColor(int screen, int x, int y) {
  return leds[screen][getCoordForScreen(x, y, screen)];
}

void assert(bool b) {
  for (int i = 0; !b; i = (i + 1) % LED_COUNT) {
    leds[0][i] = CRGB::Green;
    FastLED.show();
    leds[0][i] = CRGB::Blue;
    delay(10);
  }
}

void playSoundEffect(SoundCode soundCode) {
  Serial.print(">s"); Serial.write(soundCode);
}
void pauseSoundEffects() {
  Serial.print(">p");
}
void resumeSoundEffects() {
  Serial.print(">r");
}
void stopSoundEffects() {
  Serial.print(">S");
}

void loopMusic(MusicCode musicCode) {
  Serial.print(">M"); Serial.write(musicCode);
}
void pauseMusic() {
  Serial.print(">P");
}
void resumeMusic() {
  Serial.print(">R");
}
void fadeOutMusic(char millis) {
  Serial.print(">F"); Serial.write(millis);
}
