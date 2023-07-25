#include <FastLED.h>

#define LED_PIN     7
#define NUM_LEDS    60
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define OVERALL_BRIGHTNESS 50
const int ledPin = 11;// Intialize Pin11 for connecting LED
const int LDRPin = A0;// Initialize PIN A0 LDR PIN
CRGB leds[NUM_LEDS];

//this is the port where the button is connected
#define blackbuttonPin 3

bool button_switch = false;
unsigned long lastButtonPress;
uint8_t hue = 0;

void setup() {
  Serial.begin(9600);
  pinMode(blackbuttonPin, INPUT_PULLUP);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness (OVERALL_BRIGHTNESS);
  pinMode(ledPin, OUTPUT);// Define LED pin as output
  pinMode(LDRPin, INPUT);
}


void loop() {

  int btnState = digitalRead(blackbuttonPin);
  int ldrStatus = analogRead(LDRPin);

  if (btnState == LOW) {
    if (millis() - lastButtonPress > 50) {

      //This toggles the state of "button_switch" from
      //either "true" to "false" or vice versa
      button_switch = !button_switch;
      Serial.println("BUTTON PRESSED ");
    }
    lastButtonPress = millis();
  }


  if (button_switch == true) {

     uint8_t pos = map(beat8(20, 0), 0, 255, 0, NUM_LEDS -1);
    leds[pos] = CRGB::White;

    fadeToBlackBy(leds, NUM_LEDS, 128);

    EVERY_N_MILLISECONDS(10) {
      //Serial.println(pos);
    }
  
    FastLED.show();
    
  }

  else if (button_switch == false) {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
  }

  FastLED.show();
    // read LDR lignt intensity as analog value
// Control LED based on LDR light intensity
  if (ldrStatus >= 900) {

    digitalWrite(ledPin, HIGH);
    Serial.print(" LIGHT ON : ");
    Serial.println(ldrStatus);

  } else {

    digitalWrite(ledPin, LOW);
    Serial.print("LIGHT OFF: ");
    Serial.println(ldrStatus);// Print LDR analog value on serial port
  }
}
