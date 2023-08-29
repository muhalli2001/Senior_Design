// using the FastLed library
#include <FastLED.h>

// Data for led is at pin 7 on the microcontroller
#define LED_PIN      7
// This is basically the size of the strip, for the one we have 300 is the real maximum
#define NUM_LEDS     70
// On the strip, a vehicle will be 4 leds long. 
#define VEHICLE_SIZE 4

// Photoresistor to detect light connected to port A0
#define PResistorL A0
// Photoresistor to detect light connected to port A1
#define PResistorR A1
// Button connected to port 2
#define Button1 2

// FastLed uses a CRGB arry. Each led is a CRGB object. The object is used to make changes to the led's color & brightness
CRGB leds[NUM_LEDS];

// smaller size array for controlling 4 leds at a time (vehicles = pattern1 & pattern2)
CRGB pattern1[VEHICLE_SIZE] = {CRGB::White, CRGB::Blue, CRGB::Blue, CRGB::White}; 
CRGB pattern2[VEHICLE_SIZE] = {CRGB::White, CRGB::Red, CRGB::Red, CRGB::White};

// The first pattern begins at led position 1
int pattern1Position = 1;

// The second pattern begins at the end of the strip
int pattern2Position = NUM_LEDS - VEHICLE_SIZE;

// These two variables are responsible for updating the movement. Right now, every 200 ms, the patterns move 1 by 1.
unsigned long lastUpdateTime = 0;
unsigned long updateInterval = 200; 

// These are temporary but are used to stop the leds in the loop() function further down. 
bool isStopped=false;
bool pattern1_stop=false;
bool pattern2_stop=false;

// function to stop all patterns on the LED
void stopAllPatterns()
{
  isStopped = true; // false or toggle, not sure yet
}

// function to stop only pattern1
void stopPattern1()
{
  pattern1_stop = true;
}

// function to stop only pattern2
void stopPattern2()
{
  pattern2_stop = true;
}

// setting up a few components connected to the microcontroller.
void setup() {

  // Photoresistor 1, sends data to the microcontroller
  pinMode(PResistorL, INPUT); 

  // Photoresistor 2, sends data to the microcontroller
  pinMode(PResistorR, INPUT); 

  // Button, normally pulled up to 5V. When pressed goes low 
  pinMode(Button1, INPUT_PULLUP); 

  // Led type: Ws2812B
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);

  // sets the entire strip's brightness to 100. Max: 255
  FastLED.setBrightness(100);

  // serial monitor that is able to send and receive messaages from the microcontroller (for print statements during runtime)
  // Helpful for seeing the real time data from the photoresistors
  Serial.begin(9600);
}

void loop() {

  // Gets the current time in milliseconds since the program began running
  unsigned long currentTime = millis();


  // update pattern every X milliseconds
  if (currentTime - lastUpdateTime >= updateInterval) {

    // clear entire strip. Won't display the clear, only clears Array Data
    FastLED.clear();

    // This is temporary. If all vehicles can move & the current pattern can move, allow the pattern to be updated.  
    if(isStopped==false)
    {
      // iterates 4 times
      for (int i = 0; i < VEHICLE_SIZE; i++) {

      // updates the patterns one by one with this logic:
      // get the current position stored in pattern1Position
      // add i to it to get the right part of the vehicle [0][1][2][3][patternPosition][patternPosition+1][patternPosition+2][patternPosition+3][8][9][10]
      leds[(pattern1Position + i) % NUM_LEDS] = pattern1[i];
      }
    }
    
    // Same logic, now for pattern 2. If all vehicles can move & the current pattern can move, allow the pattern to be updated.
    if(isStopped==false)
    {
      // same logic as pattern1, just now updating pattern2
      for (int i = 0; i < VEHICLE_SIZE; i++) {
      leds[(pattern2Position + i) % NUM_LEDS] = pattern2[i];
      }
    }
    

    // display the updated pattern to the strip
    FastLED.show();

   if(isStopped==false)
   {
     // If the patterns can move, we can update the positions
      pattern1Position = (pattern1Position + 1) % NUM_LEDS;
      pattern2Position = (pattern2Position + 1) % NUM_LEDS;
   }
    
   else
   {
     // don't update the pattern position if they are stopped.
     pattern1Position=pattern1Position;
     pattern2Position=pattern2Position;
     continue;
   }


    // Update the time 
    lastUpdateTime = currentTime;
    

    // photoresistors read the newly updated position of the LEDS
    // All this does is constantly update the readings from the sensors and prints to the console
    // It does it every 200 ms as well
    int leftLightValue = analogRead(PResistorL);
    int rightLightValue = analogRead(PResistorR);
    Serial.print("Left Photoresistor: ");
    Serial.print(leftLightValue);
    Serial.print(" - Right Photoresistor: ");
    Serial.println(rightLightValue);
  }


  // This just prints a line if a button gets pressed
  if (digitalRead(Button1) == LOW) {
    Serial.println("Button is pressed");
  }

  
}


