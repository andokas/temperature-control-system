// 74HC595 Shift Register Pins
const int dataPin = 8;   // DS (Serial Data Input)
const int latchPin = 11; // STCP (Storage Register Clock)
const int clockPin = 12; // SHCP (Shift Register Clock)

// Sensor and relay pins
const int lm35Pin = A0;
const int ldrPin = A1;
const int relayPin = 9;  // Connected to TIP120 base with 1kΩ resistor

// Temperature threshold to activate fan (in Celsius)
const float tempUmbral = 25.0;

void setup() {
  // Configure shift register pins
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  
  // Configure relay pin
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Relay off initially
  
  // Initialize serial communication
  Serial.begin(9600);
}

void loop() {
  // Read temperature from LM35
  int lecturaTemp = analogRead(lm35Pin);
  float temperatura = (lecturaTemp * 5.0 * 100.0) / 1024.0;
  
  // Read light level from photoresistor
  int nivelLuz = analogRead(ldrPin);
  
  // Display values in Serial Monitor
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.print(" C | Luz: ");
  Serial.println(nivelLuz);
  
  // Control fan (relay) based on temperature
  if (temperatura > tempUmbral) {
    digitalWrite(relayPin, HIGH); // Activate fan
  } else {
    digitalWrite(relayPin, LOW);  // Turn off fan
  }
  
  // Calculate LED pattern based on temperature
  // LED 1 (Q0): Normal temperature (< 25°C)
  // LED 2 (Q1): Medium temperature (25-30°C) 
  // LED 3 (Q2): High temperature (> 30°C)
  // LED 4 (Q3): Low light indicator
  
  byte patronLeds = 0;
  
  // Temperature LEDs
  if (temperatura < 25) {
    patronLeds = 0b00000001; // Only LED 1 (green - normal)
  } else if (temperatura < 30) {
    patronLeds = 0b00000011; // LED 1 and 2 (yellow - medium)
  } else {
    patronLeds = 0b00000111; // All 3 LEDs (red - high)
  }
  
  // Add LED 4 if low light detected
  if (nivelLuz < 300) {
    patronLeds |= 0b00001000; // Activate LED 4 (bit 3)
  }
  
  // Send data to shift register
  actualizarShiftRegister(patronLeds);
  
  delay(500); // Update every half second
}

void actualizarShiftRegister(byte datos) {
  // Prepare to send data
  digitalWrite(latchPin, LOW);
  
  // Send the 8 bits
  shiftOut(dataPin, clockPin, MSBFIRST, datos);
  
  // Activate latch to display data
  digitalWrite(latchPin, HIGH);
}