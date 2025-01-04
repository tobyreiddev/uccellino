#include <FastLED.h>
#define NUM_LEDS 35
#define DATA_PIN 3
#define CLOCK_PIN 2

struct CRGB leds[NUM_LEDS];

void setup() {
  delay(3000);
  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
  FastLED.setBrightness(100);
}

void loop() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::White;
  }
  FastLED.show();
}
