// using the FastLed library
#include <FastLED.h>

// Data for led is at pin 7 on the microcontroller
#define LED_PIN      7
// This is basically the size of the strip, for the one we have 300 is the real maximum
#define NUM_LEDS     70
// On the strip, a vehicle will be 4 leds long. 
#define VEHICLE_SIZE 4

// Photoresistor to detect light connected to port A0
#define Zone1EndLDR A0
// Photoresistor to detect light connected to port A1
#define Zone2BeginLDR A1
// Button connected to port 2
#define Button1 2
int sensorValue;
int sensorFlagValue;
// FastLed uses a CRGB arry. Each led is a CRGB object. The object is used to make changes to the led's color & brightness
CRGB leds[NUM_LEDS];

// smaller size array for controlling 4 leds at a time (vehicles = pattern1 & pattern2)
CRGB pattern1[VEHICLE_SIZE] = {CRGB::White, CRGB::Blue, CRGB::Blue, CRGB::White}; 
//CRGB pattern2[VEHICLE_SIZE] = {CRGB::White, CRGB::Red, CRGB::Red, CRGB::White};

// The first pattern begins at led position 1
int pattern1Position = 1;

// The second pattern begins at the end of the strip
//int pattern2Position = NUM_LEDS - VEHICLE_SIZE;

// The third pattern begins on the strip
//int pattern3Position = ...;

// These two variables are responsible for updating the movement. Right now, every 200 ms, the patterns move 1 by 1.
unsigned long lastUpdateTime = 0;
unsigned long updateInterval = 200; 

// These are used to time the photoresistor delay around 1000ms
const unsigned long delayDuration = 300; // 1000ms delay

// 1 variable for 2 photoresistors
// zone1end, zone2begin
unsigned long SensorTrip_zone12 = 0;

// zone2end, zone3begin
//unsigned long SensorTrip_zone23 = 0;

// zone3end, zone4begin
//unsigned long SensorTrip_zone34 = 0;

// zone4end, zone5begin
//unsigned long SensorTrip_zone45 = 0;

// zone5end, zone6begin

// zone6end, zone_station_begin

// zone station_end, zone1begin


// These are temporary but are used to stop the leds in the loop() function further down. 

bool isStopped=false;
bool pattern1_isStopped=false;
//bool pattern2_isStopped=false;
//bool pattern3_isStopped=false;

// Creating a struct to store information pertaining to block zones
struct Zone_Info {
    bool isOccupied;
    int zoneBegin;
    int zoneEnd;
};

// two zones defined
Zone_Info zone1; 
Zone_Info zone2;
//Zone_Info zone3;
//Zone_Info zone4;
//Zone_Info zone5;
//Zone_Info zone6;
//Zone_Info zone_station;

const int s0 = 8;
const int s1 = 9;
const int s2 = 10;
const int s3 = 11;

// Define the signal pin for the multiplexer
const int SIG_PIN = A5; // Connect this to the SIG pin of the multiplexer
const int estopChannel = 6;
const int keyChannel = 1;
const int enableChannel = 2;
const int resetChannel = 3;
const int dispatchChannel = 4;
const int stationChannel = 5;
const int powerLED = 7;

// Finds the Vehicle in Block Zone
int SearchVehicle(int zoneBegin, int zoneEnd)
{
    if(pattern1Position>zoneBegin && pattern1Position<zoneEnd)
    {
      return 1;
    }
    // else if(pattern2Position>zoneBegin && pattern2Position<zoneEnd)
    // {
    //   return 2;
    // }
    // else if(pattern3Position>zoneBegin && pattern3Position<zoneEnd)
    // {
    //   return 3;
    // }
    else
    {
      return 0;
    }
}

// Stops Vehicle in block zone
void StopVehicle(int zoneBegin, int zoneEnd)
{
  //Serial.println("stop function called");

  int vehicle = SearchVehicle(zoneBegin, zoneEnd);

  // no vehicle found in zone
  if(vehicle==0)
  {
    return;
  }
  if(vehicle==1)
  {
    pattern1_isStopped=true;
  }
  // else if(vehicle==2)
  // {
  //   pattern2_isStopped=true;
  // }
  // else if(vehicle==3)
  // {
  //   pattern3_isStopped=true;
  // }
}

// Move Vehicle in Block Zone
void MoveVehicle(int zoneBegin, int zoneEnd)
{
  //Serial.println("move function called");
  int vehicle = SearchVehicle(zoneBegin, zoneEnd);
  if(vehicle==0)
  {
    return;
  }
  if(vehicle==1)
  {
    pattern1_isStopped=false;
  }
  // else if(vehicle==2)
  // {
  //   pattern2_isStopped=false;
  // }
  // else if(vehicle==3)
  // {
  //   pattern3_isStopped=false;
  // }
  
}

void stopAllVehicles()
{
  isStopped=true;
}
void moveAllVehicles()
{
  isStopped=false;
}

void turnOnPowerLED(){
  digitalWrite(powerLED,HIGH);
}


// Function to select a channel on the multiplexer
void selectChannel(int channel) {
  // Calculate the binary representation of the channel
  digitalWrite(s0, bitRead(channel, 0));
  digitalWrite(s1, bitRead(channel, 1));
  digitalWrite(s2, bitRead(channel, 2));
  digitalWrite(s3, bitRead(channel, 3));
}
// setting up a few components connected to the microcontroller.
void setup() {

  // init zone1
  zone1.isOccupied=false;
  zone1.zoneBegin=0;
  zone1.zoneEnd=38;

  // init zone2
  zone2.isOccupied=false;
  zone2.zoneBegin=39;
  zone2.zoneEnd=70;

  // We'd need to init zones 3-station

  // Photoresistor 1, sends data to the microcontroller
  pinMode(Zone1EndLDR, INPUT); 

  // Photoresistor 2, sends data to the microcontroller
  pinMode(Zone2BeginLDR, INPUT); 

  // Button, normally pulled up to 5V. When pressed goes low 
  pinMode(Button1, INPUT_PULLUP); 

  // Led type: Ws2812B
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);

  // sets the entire strip's brightness to 100. Max: 255
  FastLED.setBrightness(100);
  
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);

  // Initialize the signal pin as INPUT
  pinMode(SIG_PIN, INPUT_PULLUP);
  // serial monitor that is able to send and receive messaages from the microcontroller (for print statements during runtime)
  // Helpful for seeing the real time data from the photoresistors
 

  FastLED.clear();
  leds[20]= CRGB::White;
  FastLED.show();

  sensorFlagValue= analogRead(Zone2BeginLDR); 
  sensorValue = sensorFlagValue - 200;
  
  Serial.println(sensorFlagValue);
  Serial.println(sensorValue);  
  FastLED.clear();
  FastLED.show();

   Serial.begin(9600);
}

void loop() {
  int powerState = 0;
  int enableState = 0;
  for (int channel = 0; channel < 16; channel++) {
    // Select the current channel on the multiplexer
    selectChannel(channel);

    // Read the button state from the current channel
    int buttonState = digitalRead(SIG_PIN);
    if(buttonState == HIGH)
    {
      if(channel == keyChannel){
        //Serial.println("Key switch was turned to on");
        turnOnPowerLED();
        powerState = 1;
      }
      
      if(powerState == 1){

        if(channel == estopChannel){
          Serial.println("Estop was pressed");
          stopAllVehicles();
        }
        
        if(channel == enableChannel){
          Serial.println("Enable button was pressed");
          enableState = 1;
        }

        if(channel == resetChannel){
          Serial.println("Reset button was pressed");
          moveAllVehicles();
        }

        if(channel == dispatchChannel && enableState == 1){
          Serial.println("Dispatch button was pressed while enable was held");
          //send vehicle in station to the next zone
        }

        if(channel == stationChannel){
          Serial.println("Station stop was pressed");
          //stop vehicle in station and have all the other vehicles line up behind it
        }
      }

    }

    delay(10);
  }

  if(powerState == 0){
    digitalWrite(powerLED, LOW);
  }

  // Gets the current time in milliseconds since the program began running
  unsigned long currentTime = millis();

  //turn on the LED here
  
  
  
  // update pattern every X milliseconds
  if (currentTime - lastUpdateTime >= updateInterval) {

    // clear entire strip. Won't display the clear, only clears Array Data
    FastLED.clear();
    
      // iterates 4 times
      for (int i = 0; i < VEHICLE_SIZE; i++) {

      // updates the patterns one by one with this logic:
      // get the current position stored in pattern1Position
      // add i to it to get the right part of the vehicle [0][1][2][3][patternPosition][patternPosition+1][patternPosition+2][patternPosition+3][8][9][10]

      // important: this doesn't move the vehicle forward by 1.
      leds[(pattern1Position + i) % NUM_LEDS] = pattern1[i];
      }
    
    
    // Same logic, now for pattern 2. If all vehicles can move & the current pattern can move, allow the pattern to be updated.

    //   same logic as pattern1, just now updating pattern2
    //   for (int i = 0; i < VEHICLE_SIZE; i++) {
    //   leds[(pattern2Position + i) % NUM_LEDS] = pattern2[i];
    //   }
  


    // display the updated pattern to the strip
    FastLED.show();

   if(isStopped==false || pattern1_isStopped==false)
   {
     // This is how it moves forward
      pattern1Position = (pattern1Position + 1) % NUM_LEDS;
      
   }
   else
   {
      // This is how it stops
      pattern1Position=pattern1Position;
   }
  //  if(isStopped==false || pattern2_isStopped==false)
  //  {
  //     // If the patterns can move, we can update the positions
  //     pattern2Position = (pattern2Position + 1) % NUM_LEDS;
  //  }
  //  else
  //  {
  //     pattern2Position=pattern2Position;
  //  }
    



    // Update the time 
    lastUpdateTime = currentTime;
    

    // photoresistors read the newly updated position of the LEDS
    // All this does is constantly update the readings from the sensors and prints to the console
    // It does it every 200 ms as well
    // int leftLightValue = analogRead(Zone1EndLDR);
    // int rightLightValue = analogRead(Zone2BeginLDR);
    // Serial.print("Left Photoresistor: ");
    // Serial.print(leftLightValue);
    // Serial.print(" - Right Photoresistor: ");
    // Serial.println(rightLightValue);
  }

  // ZONE LOGIC: make zone busy/clear:

  // 1. Clear Zone 1, Busy Zone 2
  // First Time Sensors Get Tripped
  if (analogRead(Zone1EndLDR) > sensorValue && analogRead(Zone2BeginLDR) > sensorValue) {
    // Record the time when the first condition became true
    Serial.println("Sensors Tripped");
    SensorTrip_zone12 = currentTime;
  }

  // Wait for X Milliseconds (delayDuration)
  if (SensorTrip_zone12 != 0 && currentTime - SensorTrip_zone12 >= delayDuration) {
    Serial.println("Delayed For Milliseconds");

    // The delay has elapsed, check if the sensors are still higher than normal
    if (analogRead(Zone1EndLDR) > sensorValue && analogRead(Zone2BeginLDR) > sensorValue) {

      Serial.println("Sensors still tripped ***");
      // Clear zone 1
      zone1.isOccupied = false;
      Serial.println("zone1 is cleared");
      // busy zone 2
      zone2.isOccupied = true;
      Serial.println("zone2 is busy");

      Serial.println("PATTERN1POSITION");
      Serial.println(pattern1Position);
    }

    // Reset the first condition true time for the next iteration
    SensorTrip_zone12 = 0;
  }

  // 2. Clear Zone 2, Busy Zone 3 (same logic, different variables in some spots)

  // First Time Sensors Get Tripped
  // if (analogRead(Zone2EndLDR) > 1000 && analogRead(Zone3BeginLDR) > 800) {
  //   // Record the time when the first condition became true
  //   Serial.println("Sensors Tripped");
  //   SensorTrip_zone23 = currentTime;
  // }

  // // Wait for X Milliseconds (delayDuration)
  // if (SensorTrip_zone23 != 0 && currentTime - SensorTrip_zone23 >= delayDuration) {
  //   Serial.println("Delayed For Some Milliseconds");

  //   // The delay has elapsed, check if the sensors are still higher than normal
  //   if (analogRead(Zone2EndLDR) > 700 && analogRead(Zone3BeginLDR) > 700) {

  //     Serial.println("Sensors still tripped ***");
  //     // Clear zone 2
  //     zone2.isOccupied = false;
  //     Serial.println("zone2 is cleared");
  //     // busy zone 3
  //     zone3.isOccupied = true;
  //     Serial.println("zone3 is busy");

  //   }

  //   // Reset the first condition true time for the next iteration
  //   SensorTrip_zone23 = 0;
  // }  

  // 3. Clear Zone 3, Busy Zone 4

  // 4. Clear Zone 4, Busy Zone 5

  // 5. Clear Zone 5, Busy Zone 6

  // 6. CLear Zone 6, Busy Station

  // 7. Clear Station, Busy Zone 1

  // ZONE LOGIC: Stop and Move Vehicles

  // if the next zone is busy, stop the vehicle in previous zone
  // if the next zone is clear, move the vehicle in previous zone


  // 1. Station Zone

  // if(zone1.isOccupied)
  // {
        // StopVehicle(zone_station_begin, zone_station_end);
  // }
  // else if(!zone1.isOccupied)
  // {
        // MoveVehicle(zone_station_begin, zone_station_begin);
  // }

  // 2. Zone 1

  if(zone2.isOccupied)
  {
    StopVehicle(zone1.zoneBegin,zone1.zoneEnd);
  }
  else if(!zone2.isOccupied)
  {
    MoveVehicle(zone1.zoneBegin,zone1.zoneEnd);
  }

  // 3. Zone 2

  // if(zone3.isOccupied)
  // {
  //   StopVehicle(zone2.zoneBegin,zone2.zoneEnd);
  // }
  // else if(!zone3.isOccupied)
  // {
  //   MoveVehicle(zone2.zoneBegin,zone2.zoneEnd);
  // }

  // 4. Zone 3

  // 5. Zone 4

  // 6. Zone 5

  // 7. Zone 6

  // This just prints a line if a button gets pressed
  if (digitalRead(Button1) == LOW) {
    Serial.println("Button is pressed");
  }

  
}