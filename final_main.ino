#include <FastLED.h>
#include <Servo.h>

#define LED_PIN 10
#define NUM_LEDS 438

#define VEHICLE_SIZE 6

const int s0 = 2;
const int s1 = 3;
const int s2 = 4;
const int s3 = 5;

const int s0Pin = 6;
const int s1Pin = 7;
const int s2Pin = 8;
const int s3Pin = 9;

const servoMotor1Channel = 2;
const servoMotor2Channel = 9;
const servoMotor3Channel = 10;
const servoMotor4Channel = 11;
const servoMotor5Channel = 12;
const servoMotor6Channel = 13;
const servoMotor7Channel = 14;
const servoMotor8Channel = 15;

Servo servoMotor1;
Servo servoMotor2;
Servo servoMotor3;
Servo servoMotor4;
Servo servoMotor5;
Servo servoMotor6;
Servo servoMotor7;
Servo servoMotor8;

// Define the signal pin for the multiplexer
const int SIG_PIN = A1; // Connect this to the SIG pin of the multiplexer
const int estopChannel = 6;
const int keyChannel = 4;
const int enableChannel = 3;
const int resetChannel = 8;
const int dispatchChannel = 7;
const int stationChannel = 5;
const int powerLED = 11;

const int zone1beg = 3;
const int zone1end = 4;
const int zone2beg = 5;
const int zone2end = 6;
const int zone3beg = 7;
const int zone3end = 8;
const int zone4beg = 9;
const int zone4end = 10;
const int zone5beg = 11;
const int zone5end = 12;
const int zone6beg = 13;
const int zone6end = 14;
const int zone7beg = 15;
const int zone7end = 2;

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
CRGB pattern1[VEHICLE_SIZE] = {CRGB::White, CRGB::Blue, CRGB::Blue, CRGB::Blue ,CRGB::Blue, CRGB::White}; 
CRGB pattern2[VEHICLE_SIZE] = {CRGB::White, CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red, CRGB::White};
CRGB pattern3[VEHICLE_SIZE] = {CRGB::White, CRGB::Yellow, CRGB::Yellow, CRGB::Yellow, CRGB::Yellow, CRGB::White};
CRGB pattern4[VEHICLE_SIZE] = {CRGB::White, CRGB::Green, CRGB::Green, CRGB::Green, CRGB::Green, CRGB::White};
CRGB pattern5[VEHICLE_SIZE] = {CRGB::White, CRGB::Purple, CRGB::Purple, CRGB::Purple, CRGB::Purple, CRGB::White};

int pattern1Position = 6;

// The second pattern begins at the end of the strip
int pattern2Position = 420;
int pattern3Position = 413;
int pattern4Position = 406;
int pattern5Position = 399;
unsigned long lastUpdateTime = 0;
unsigned long updateInterval = 200; 


bool isStopped=false;
bool pattern1_isStopped=false;
bool estopCleared = true;
bool stationStopCleared = true;
bool pattern2_isStopped = false;
bool pattern3_isStopped = false;
bool pattern4_isStopped = false;
bool pattern5_isStopped = false;

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
    }else if(pattern3Position>zoneBegin && pattern3Position < zoneEnd){
    return 3;
    }
    else if(pattern4Position>zoneBegin && pattern4Position < zoneEnd){
        return 4;
    }else if(pattern5Position>zoneBegin && pattern5Position < zoneEnd)
    {
        return 5;
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
    if(vehicle==3)
  {
    pattern3_isStopped=true;
  }
  if(vehicle==4)
  {
    pattern4_isStopped=true;
  }
    if(vehicle==5)
  {
    pattern5_isStopped=true;
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
      if(vehicle==3)
  {
    pattern3_isStopped=false;
  }
  if(vehicle==4)
  {
    pattern4_isStopped=false;
  }
 if(vehicle==5)
  {
    pattern5_isStopped=false;
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

void activateBrakes(int channel,Servo brake)
{
    selectChannel(channel);
    brake.write(90);
}
void releaseBrakes(int channel,Servo brake)
{
    selectChannel(channel);
    brake.write(0);
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
  zone1.zoneBegin=6;
  zone1.zoneEnd=52;

  zone2.isOccupied=false;
  zone2.zoneBegin=55;
  zone2.zoneEnd=146;

  zone3.isOccupied=false;
  zone3.zoneBegin=149;
  zone3.zoneEnd=170;
  
  zone4.isOccupied=false;
  zone4.zoneBegin=173;
  zone4.zoneEnd=284;

  zone5.isOccupied=false;
  zone5.zoneBegin=287;
  zone5.zoneEnd=318;

  zone6.isOccupied=false;
  zone6.zoneBegin=321;
  zone6.zoneEnd=408;

  zone_station.isOccupied=false;
  zone_station.zoneBegin=411;
  zone_station.zoneEnd=426;

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(100);

    servoMotor1.attach(servoMotor1Channel);
    servoMotor2.attach(servoMotor2Channel);
    servoMotor3.attach(servoMotor3Channel);
    servoMotor4.attach(servoMotor4Channel);
    servoMotor5.attach(servoMotor5Channel);
    servoMotor6.attach(servoMotor6Channel);
    servoMotor7.attach(servoMotor7Channel);
    servoMotor8.attach(servoMotor8Channel);
    
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
    for (int i = 0; i < VEHICLE_SIZE; i++) {
      leds[(pattern3Position + i) % NUM_LEDS] = pattern3[i];
      }
    for (int i = 0; i < VEHICLE_SIZE; i++) {
      leds[(pattern4Position + i) % NUM_LEDS] = pattern4[i];
      }
    for (int i = 0; i < VEHICLE_SIZE; i++) {
      leds[(pattern5Position + i) % NUM_LEDS] = pattern5[i];
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

   if(isStopped==false && pattern3_isStopped==false && estopCleared)
    {
      pattern3Position = (pattern3Position + 1) % NUM_LEDS;
    }
    else
    {
      pattern3Position=pattern3Position;
    }

       if(isStopped==false && pattern4_isStopped==false && estopCleared)
    {
      pattern4Position = (pattern4Position + 1) % NUM_LEDS;
    }
    else
    {
      pattern4Position=pattern4Position;
    }

       if(isStopped==false && pattern5_isStopped==false && estopCleared)
    {
      pattern5Position = (pattern5Position + 1) % NUM_LEDS;
    }
    else
    {
      pattern5Position=pattern5Position;
    }

  lastUpdateTime = currentTime;

  for (int channel = 0; channel < 16; channel++) {
    // Select the current channel on the multiplexer
    selectChannel(channel);

    // Read the button state from the current channel
    int buttonState = digitalRead(SIG_PIN);
    
    int sensorValue = analogRead(SIG_PIN_2);

    if(sensorValue>900)
    {
       Serial.print("Channel ");
       Serial.print(channel);
       Serial.print(": ");
       Serial.println(sensorValue);

      if(channel == zone1end)
      {
        zone1.isOccupied = false;
        zone2.isOccupied = true;
      }

      if(channel == zone1beg)
      {
        zone1.isOccupied=true;
        if(zone2.isOccupied){
          StopVehicle(zone1.zoneBegin, zone1.zoneEnd);
        }else
        {
          MoveVehicle(zone1.zoneBegin, zone1.zoneEnd);
        }
      }

      if(channel == zone2end)
      {
        zone2.isOccupied = false;
        zone3.isOccupied = true;
      }

      if(channel == zone2beg)
      {
        zone2.isOccupied=true;
        if(zone3.isOccupied){
          StopVehicle(zone2.zoneBegin, zone2.zoneEnd);
          activateBrakes(servoMotor1Channel,servoMotor1); 
          activateBrakes(servoMotor2Channel,servoMotor2); 
        }else
        {
          MoveVehicle(zone2.zoneBegin, zone2.zoneEnd);
          releaseBrakes(servoMotor1Channel,servoMotor1); 
          releaseBrakes(servoMotor2Channel,servoMotor2); 
        }
      }

    if(channel == zone3end)
      {
        zone3.isOccupied = false;
        zone4.isOccupied = true;
      }

      if(channel == zone3beg)
      {
        zone3.isOccupied=true;
        if(zone4.isOccupied){
          StopVehicle(zone3.zoneBegin, zone3.zoneEnd);
        }else
        {
          MoveVehicle(zone3.zoneBegin, zone3.zoneEnd);
        }
      }

      if(channel == zone4end)
      {
        zone4.isOccupied = false;
        zone5.isOccupied = true;
      }

      if(channel == zone4beg)
      {
        zone4.isOccupied=true;
        if(zone5.isOccupied){
          StopVehicle(zone4.zoneBegin, zone4.zoneEnd);
          activateBrakes(servoMotor3Channel,servoMotor3); 
          activateBrakes(servoMotor4Channel,servoMotor4); 
        }else
        {
          MoveVehicle(zone4.zoneBegin, zone4.zoneEnd);
          releaseBrakes(servoMotor3Channel,servoMotor3); 
          releaseBrakes(servoMotor4Channel,servoMotor4);
        }
      }

      if(channel == zone5end)
      {
        zone5.isOccupied = false;
        zone6.isOccupied = true;
      }

      if(channel == zone5beg)
      {
        zone5.isOccupied=true;
        if(zone6.isOccupied){
          StopVehicle(zone5.zoneBegin, zone5.zoneEnd);
        }else
        {
          MoveVehicle(zone5.zoneBegin, zone5.zoneEnd);
        }
      }

      if(channel == zone6end)
      {
        zone6.isOccupied = false;
        zone_station.isOccupied = true;
      }

      if(channel == zone6beg)
      {
        zone6.isOccupied=true;
        if(zone_station.isOccupied){
          StopVehicle(zone6.zoneBegin, zone6.zoneEnd);
          activateBrakes(servoMotor5Channel,servoMotor3); 
          activateBrakes(servoMotor6Channel,servoMotor4);
          activateBrakes(servoMotor7Channel,servoMotor3); 
          activateBrakes(servoMotor8Channel,servoMotor4); 
        }else
        {
          MoveVehicle(zone6.zoneBegin, zone6.zoneEnd);
          releaseBrakes(servoMotor5Channel,servoMotor3); 
          releaseBrakes(servoMotor6Channel,servoMotor4);
          releaseBrakes(servoMotor7Channel,servoMotor3); 
          releaseBrakes(servoMotor8Channel,servoMotor4); 
        }
      }

      if(channel == zone7end) 
      {
        zone_station.isOccupied = false;
        zone1.isOccupied = true;
        Serial.print("station cleared");
      }

      if(channel == zone7beg)
      {
        zone_station.isOccupied=true;
        if(zone1.isOccupied){
          StopVehicle(zone_station.zoneBegin, zone_station.zoneEnd);
        }else
        {
          MoveVehicle(zone_station.zoneBegin, zone_station.zoneEnd);
        }
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


    if(buttonState == HIGH)
    {
      if(channel == keyChannel){
        turnOnPowerLED();
        powerState = 1;
      }
      
      if(powerState == 1){

        if(channel == estopChannel){
          estopCleared = false;
          stopAllVehicles();
    
        }
        
        if(channel == enableChannel){
          enableState = 1;
        }

        if(channel == dispatchChannel && enableState == 1 && estopCleared){
          moveAllVehicles();
        }

        if(channel == dispatchChannel && enableState == 1 && stationStopCleared){
          MoveVehicle(zone_station.zoneBegin,zone_station.zoneEnd);
        }
        
        if(channel == resetChannel && enableState == 1){
          estopCleared = true;
          stationStopCleared = true;
        }

        if(channel == stationChannel){
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
