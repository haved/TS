#include <FastLED.h>

#define LED_COUNT 100
CRGB leds[LED_COUNT];

void setup() {
  FastLED.addLeds<WS2812B, 24, GRB>(leds, LED_COUNT);
}

void loop() {
  for(int i = 0; i < LED_COUNT; i++) {
    leds[i] = CRGB::Yellow;
    FastLED.show();
    delay(20);
    leds[i] = CRGB(10, 100, 10);
  }
}
