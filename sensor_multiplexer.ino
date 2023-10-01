// Define the control pins for the multiplexer
const int s0Pin = 2;
const int s1Pin = 3;
const int s2Pin = 4;
const int s3Pin = 5;

// Define the enable pin for the multiplexer
const int enablePin = 6;

void setup() {
  // Initialize multiplexer control pins as OUTPUT
  pinMode(s0Pin, OUTPUT);
  pinMode(s1Pin, OUTPUT);
  pinMode(s2Pin, OUTPUT);
  pinMode(s3Pin, OUTPUT);

  // Initialize enable pin as OUTPUT
  pinMode(enablePin, OUTPUT);

  // Enable the multiplexer
  digitalWrite(enablePin, LOW);

  // Begin serial communication
  Serial.begin(9600);
}

void loop() {
  // Loop through all 16 channels of the multiplexer
  for (int channel = 0; channel < 16; channel++) {
    // Select the channel using binary values on control pins
    digitalWrite(s0Pin, bitRead(channel, 0));
    digitalWrite(s1Pin, bitRead(channel, 1));
    digitalWrite(s2Pin, bitRead(channel, 2));
    digitalWrite(s3Pin, bitRead(channel, 3));

    // Read the analog value from the selected channel
    int sensorValue = analogRead(A0); // Change A0 to the appropriate analog pin

    // Print the sensor value to the serial monitor
    Serial.print("Channel ");
    Serial.print(channel);
    Serial.print(": ");
    Serial.println(sensorValue);

    // You can add your logic or processing here based on the sensorValue

    delay(1000); // Delay for 1 second before switching to the next channel
  }
}