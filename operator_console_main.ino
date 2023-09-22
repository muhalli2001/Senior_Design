const int s0 = 8;
const int s1 = 9;
const int s2 = 10;
const int s3 = 11;

// Define the signal pin for the multiplexer
const int SIG_PIN = A0; // Connect this to the SIG pin of the multiplexer
const int estopChannel = 6;
const int keyChannel = 1;
const int enableChannel = 2;
const int resetChannel = 3;
const int dispatchChannel = 4;
const int stationChannel = 5;
const int powerLED = 7;

void setup() {
  // Initialize the multiplexer control pins as OUTPUT
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);

  // Initialize the signal pin as INPUT
  pinMode(SIG_PIN, INPUT_PULLUP);

  // Initialize serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  // Loop through all 16 channels
  int powerState = 0;


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
        }
        
        if(channel == enableChannel){
          Serial.println("Enable button was pressed");
        }

        if(channel == resetChannel){
          Serial.println("Reset button was pressed");
        }

        if(channel == dispatchChannel){
          Serial.println("Dispatch button was pressed");
        }

        if(channel == stationChannel){
          Serial.println("Station stop was pressed");
        }
      }

    }

    delay(20);
  }

  if(powerState == 0){
    digitalWrite(powerLED, LOW);
  }
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