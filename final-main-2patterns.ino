#include <FastLED.h>

#define LED_PIN 6
#define NUM_LEDS 140

#define VEHICLE_SIZE 4

const int s0 = 8;
const int s1 = 9;
const int s2 = 10;
const int s3 = 11;

const int s0Pin = 2;
const int s1Pin = 3;
const int s2Pin = 4;
const int s3Pin = 5;

// Define the signal pin for the multiplexer
const int SIG_PIN = A5; // Connect this to the SIG pin of the multiplexer
const int estopChannel = 6;
const int keyChannel = 1;
const int enableChannel = 2;
const int resetChannel = 3;
const int dispatchChannel = 4;
const int stationChannel = 5;
const int powerLED = 7;

const int zone1beg = 14;
const int zone1end = 1;
const int zone2beg = 2;
const int zone2end = 3;
const int zone3beg = 4;
const int zone3end = 5;
const int zone4beg = 6;
const int zone4end = 7;
const int zone5beg = 8;
const int zone5end = 9;
const int zone6beg = 10;
const int zone6end = 11;
const int zone7beg = 12;
const int zone7end = 13;

bool zone1beginSensor = false;
bool zone1endSensor = false;
bool zone2beginSensor = false;
bool zone2endSensor = false;
bool zone3beginSensor = false;
bool zone3endSensor = false;
bool zone4beginSensor = false;
bool zone4endSensor = false;
bool zone5beginSensor = false;
bool zone5endSensor = false;
bool zone6beginSensor = false;
bool zone6endSensor = false;
bool zone7beginSensor = false;
bool zone7endSensor = false;

const int SIG_PIN_2 = A0;

CRGB leds[NUM_LEDS];
CRGB pattern1[VEHICLE_SIZE] = {CRGB::White, CRGB::Blue, CRGB::Blue, CRGB::White}; 
CRGB pattern2[VEHICLE_SIZE] = {CRGB::White, CRGB::Red, CRGB::Red, CRGB::White};

int pattern1Position = 1;

// The second pattern begins at the end of the strip
int pattern2Position = 22;
unsigned long lastUpdateTime = 0;
unsigned long updateInterval = 200; 


bool isStopped=false;
bool pattern1_isStopped=false;
bool estopCleared = true;
bool stationStopCleared = true;
bool pattern2_isStopped = false;

struct Zone_Info {
    bool isOccupied;
    int zoneBegin;
    int zoneEnd;
};

Zone_Info zone1; 
Zone_Info zone2;
Zone_Info zone3;
Zone_Info zone4;
Zone_Info zone5;
Zone_Info zone6;
Zone_Info zone_station;

int SearchVehicle(int zoneBegin, int zoneEnd)
{
    if(pattern1Position>zoneBegin && pattern1Position<zoneEnd)
    {
      return 1;
    }
    else if(pattern2Position>zoneBegin && pattern2Position < zoneEnd)
    {
      return 2;
    }
    else
    {
      return 0;
    }
}


void StopVehicle(int zoneBegin, int zoneEnd)
{
  int vehicle = SearchVehicle(zoneBegin, zoneEnd);

  if(vehicle==0)
  {
    return;
  }
  if(vehicle==1)
  {
    pattern1_isStopped=true;
  }
  if(vehicle==2)
  {
    pattern2_isStopped=true;
  }
}

void MoveVehicle(int zoneBegin, int zoneEnd)
{
  int vehicle = SearchVehicle(zoneBegin, zoneEnd);
  if(vehicle==0)
  {
    return;
  }
  if(vehicle==1)
  {
    pattern1_isStopped=false;
  }
  if(vehicle==2)
  {
    pattern2_isStopped=false;
  }
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

  digitalWrite(s0Pin, bitRead(channel, 0));
  digitalWrite(s1Pin, bitRead(channel, 1));    
  digitalWrite(s2Pin, bitRead(channel, 2));
  digitalWrite(s3Pin, bitRead(channel, 3));

}
void setup() {
  // Initialize the multiplexer control pins as OUTPUT
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  
  pinMode(s0Pin, OUTPUT);
  pinMode(s1Pin, OUTPUT);
  pinMode(s2Pin, OUTPUT);
  pinMode(s3Pin, OUTPUT);

  // Initialize the signal pin as INPUT
  pinMode(SIG_PIN, INPUT_PULLUP);

  zone1.isOccupied=false;
  zone1.zoneBegin=0;
  zone1.zoneEnd=19;

  zone2.isOccupied=false;
  zone2.zoneBegin=20;
  zone2.zoneEnd=39;

  zone3.isOccupied=false;
  zone3.zoneBegin=40;
  zone3.zoneEnd=59;
  
  zone4.isOccupied=false;
  zone4.zoneBegin=60;
  zone4.zoneEnd=79;

  zone5.isOccupied=false;
  zone5.zoneBegin=80;
  zone5.zoneEnd=99;

  zone6.isOccupied=false;
  zone6.zoneBegin=100;
  zone6.zoneEnd=119;

  zone_station.isOccupied=false;
  zone_station.zoneBegin=120;
  zone_station.zoneEnd=139;

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(100);

  // Initialize serial communication for debugging
  Serial.begin(9600);

  FastLED.clear();

}

void loop() {
  // Loop through all 16 channels
  unsigned long currentTime = millis();
  int powerState = 0;
  int enableState = 1;

  if (currentTime - lastUpdateTime >= updateInterval) {
    FastLED.clear();

    for (int i = 0; i < VEHICLE_SIZE; i++) {
    leds[(pattern1Position + i) % NUM_LEDS] = pattern1[i];
    }

    for (int i = 0; i < VEHICLE_SIZE; i++) {
      leds[(pattern2Position + i) % NUM_LEDS] = pattern2[i];
      }

    FastLED.show();

    if(isStopped==false && pattern1_isStopped==false && estopCleared)
    {
      pattern1Position = (pattern1Position + 1) % NUM_LEDS;
    }
    else
    {
      pattern1Position=pattern1Position;
    }
    if(isStopped==false && pattern2_isStopped==false && estopCleared)
    {
      pattern2Position = (pattern2Position + 1) % NUM_LEDS;
    }
    else
    {
      pattern2Position=pattern2Position;
    }

  lastUpdateTime = currentTime;

  for (int channel = 0; channel < 16; channel++) {
    // Select the current channel on the multiplexer
    selectChannel(channel);

    // Read the button state from the current channel
    int buttonState = digitalRead(SIG_PIN);
    
 
     int sensorValue = analogRead(SIG_PIN_2);


    // Serial.print("Channel ");
    // Serial.print(channel);
    // Serial.print(": ");
    // Serial.println(sensorValue);
    
    if(sensorValue>900)
    {
        /*Serial.println("channel");
        Serial.println(channel);
        Serial.println("tripped");
        Serial.println(sensorValue);*/
        Serial.print("zone 1 occupied: ");
        Serial.println(zone1.isOccupied);

        Serial.print("zone 2 occupied: ");
        Serial.println(zone2.isOccupied);

        Serial.print("zone 3 occupied: ");
        Serial.println(zone3.isOccupied);

        Serial.print("zone 4 occupied: ");
        Serial.println(zone4.isOccupied);

        Serial.print("zone 5 occupied: ");
        Serial.println(zone5.isOccupied);

        Serial.print("zone 6 occupied: ");
        Serial.println(zone6.isOccupied);

        Serial.print("zone station occupied: ");
        Serial.println(zone_station.isOccupied);
        if(channel == zone1end)
        {
          zone1endSensor=true;
        }
        if(channel == zone2beg && zone1endSensor == false)
        {
          //busy zone2
          zone2.isOccupied = true;
          //clear zone1
          zone1.isOccupied = false;
        }
        if(channel == zone2end)
        {
          zone2endSensor=true;
        }
        if(channel == zone3beg && zone2endSensor == false)
        {
          // busy zone 3

          // clear zone 2
        }
        if(channel == zone6end)
        {
          zone6endSensor = true;
        }
        if(channel == zone7beg && zone6endSensor == false)
        {
          // busy zone 7 (station)
          zone_station.isOccupied = true;
          Serial.println("**station is occupied");
          // clear zone 6
          zone6.isOccupied = false;
        }
        if(channel == zone7end)
        {
          zone7endSensor=true;
        }
        if(channel == zone1beg && zone7end == false)
        {
          // busy zone 1
          zone1.isOccupied = true;
          // clear zone 7 (station)
          zone_station.isOccupied = false;
          Serial.println("**station is clear");
        }

    }
    else
    {
      if(channel == zone1beg)
      {
        zone1beginSensor=false;
      }
      if(channel == zone1end)
      {
        zone1endSensor=false;
      }
      if(channel == zone2beg)
      {
        zone2beginSensor=false;
      }
      if(channel == zone2end)
      {
        zone2endSensor=false;
      }
      if(channel == zone3beg)
      {
        zone3beginSensor=false;
      }
      if(channel == zone3end)
      {
        zone3endSensor=false;
      }
      if(channel == zone4beg)
      {
        zone4beginSensor=false;
      }
      if(channel == zone4end)
      {
        zone4endSensor=false;
      }
      if(channel == zone5beg)
      {
        zone5beginSensor=false;
      }
      if(channel == zone5end)
      {
        zone5endSensor=false;
      }
      if(channel == zone6beg)
      {
        zone6beginSensor=false;
      }
      if(channel == zone6end)
      {
        zone6endSensor=false;
      }
      if(channel == zone7beg)
      {
        zone7beginSensor = false;
      }
      if(channel == zone7endSensor)
      {
        zone7endSensor = false;
      }
    }

    if(zone_station.isOccupied)
    {
      StopVehicle(zone6.zoneBegin, zone6.zoneEnd);
    }
    if(!zone_station.isOccupied)
    {
      MoveVehicle(zone6.zoneBegin, zone6.zoneEnd);
    }
    

    if(buttonState == HIGH)
    {
      if(channel == keyChannel){
        //Serial.println("Key switch was turned to on");
        turnOnPowerLED();
        powerState = 1;
      }
      
      if(powerState == 1){

        if(channel == estopChannel){
          Serial.println("Estop was jpressed");
          estopCleared = false;
          stopAllVehicles();
    
        }
        
        if(channel == enableChannel){
          Serial.println("Enable button was pressed");
          enableState = 1;
        }

        if(channel == dispatchChannel && enableState == 1 && estopCleared){
          Serial.println("Dispatch button was pressed while enable was held");
          moveAllVehicles();
        }

        if(channel == dispatchChannel && enableState == 1 && stationStopCleared && !zone1.isOccupied){
          MoveVehicle(zone_station.zoneBegin,zone_station.zoneEnd);
        }
        
        if(channel == resetChannel && enableState == 1){
          Serial.println("Reset button was pressed");
          estopCleared = true;
          stationStopCleared = true;
        }

        if(channel == stationChannel){
          Serial.println("Station stop was pressed");
          StopVehicle(zone_station.zoneBegin, zone_station.zoneEnd);
          stationStopCleared = false;
        }
      }

    }

    delay(10);
  }

  if(powerState == 0){
    digitalWrite(powerLED, LOW);
    stopAllVehicles();
  }
  }
  
}
