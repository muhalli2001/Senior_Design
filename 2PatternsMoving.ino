#include <FastLED.h>

#define LED_PIN      7
#define NUM_LEDS     50
#define VEHICLE_SIZE 4
#define PResistorL A0
#define PResistorR A1

CRGB leds[NUM_LEDS];
CRGB pattern1[VEHICLE_SIZE] = {CRGB::White, CRGB::Blue, CRGB::Blue, CRGB::White};
CRGB pattern2[VEHICLE_SIZE] = {CRGB::White, CRGB::Red, CRGB::Red, CRGB::White};

// start pattern 1 on led 1
int pattern1Position = 1;

// start pattern 2 at the end of the strip
int pattern2Position = NUM_LEDS - VEHICLE_SIZE;

// time the movement of the leds
unsigned long lastUpdateTime = 0;
unsigned long updateInterval = 200;

void setup() {

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(100);
  Serial.begin(9600);
}

void loop() {
  unsigned long currentTime = millis();

  // move the patterns once every update
  if (currentTime - lastUpdateTime >= updateInterval) {
    // Clear LEDs
    FastLED.clear();

    for (int i = 0; i < VEHICLE_SIZE; i++) {
      leds[(pattern1Position + i) % NUM_LEDS] = pattern1[i];
    }

    for (int i = 0; i < VEHICLE_SIZE; i++) {
      leds[(pattern2Position + i) % NUM_LEDS] = pattern2[i];
    }

    // show pattern movement
    FastLED.show();


    // update position and wrap around the led strip
    pattern1Position = (pattern1Position + 1) % NUM_LEDS;
    pattern2Position = (pattern2Position + 1) % NUM_LEDS;

    // store time of last update
    lastUpdateTime = currentTime;
  }

  
}

