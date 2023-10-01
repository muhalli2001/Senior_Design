#include <FastLED.h>

#define LED_PIN 6
#define NUM_LEDS 120

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

const int SIG_PIN_2 = A0;

CRGB leds[NUM_LEDS];
CRGB pattern1[VEHICLE_SIZE] = {CRGB::White, CRGB::Blue, CRGB::Blue, CRGB::White}; 
int pattern1Position = 1;
unsigned long lastUpdateTime = 0;
unsigned long updateInterval = 200; 


bool isStopped=false;
bool pattern1_isStopped=false;

struct Zone_Info {
    bool isOccupied;
    int zoneBegin;
    int zoneEnd;
};

Zone_Info zone1; 
Zone_Info zone2;

int SearchVehicle(int zoneBegin, int zoneEnd)
{
    if(pattern1Position>zoneBegin && pattern1Position<zoneEnd)
    {
      return 1;
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
  zone1.zoneEnd=38;

  zone2.isOccupied=false;
  zone2.zoneBegin=39;
  zone2.zoneEnd=70;

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(100);

  // Initialize serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  // Loop through all 16 channels
  unsigned long currentTime = millis();
  int powerState = 0;
  int enableState = 0;

  if (currentTime - lastUpdateTime >= updateInterval) {
    FastLED.clear();

    for (int i = 0; i < VEHICLE_SIZE; i++) {
    leds[(pattern1Position + i) % NUM_LEDS] = pattern1[i];
    }

    FastLED.show();

    if(isStopped==false || pattern1_isStopped==false)
    {
      pattern1Position = (pattern1Position + 1) % NUM_LEDS;
    }
    else
    {
      pattern1Position=pattern1Position;
    }

  lastUpdateTime = currentTime;

  for (int channel = 0; channel < 16; channel++) {
    // Select the current channel on the multiplexer
    selectChannel(channel);

    // Read the button state from the current channel
    int buttonState = digitalRead(SIG_PIN);
    
    if(channel == 1){
      int sensorValue = analogRead(SIG_PIN_2);


    Serial.print("Channel ");
    Serial.print(channel);
    Serial.print(": ");
    Serial.println(sensorValue);
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
          Serial.println("Estop was pressed");
    
        }
        
        if(channel == enableChannel){
          Serial.println("Enable button was pressed");
          enableState = 1;
        }

        if(channel == dispatchChannel && enableState == 1){
          Serial.println("Dispatch button was pressed while enable was held");
        }
        
        if(channel == resetChannel && enableState == 1){
          Serial.println("Reset button was pressed");
        }

        

        if(channel == stationChannel){
          Serial.println("Station stop was pressed");
        }
      }

    }

    delay(10);
  }

  if(powerState == 0){
    digitalWrite(powerLED, LOW);
  }
  }
  
}

