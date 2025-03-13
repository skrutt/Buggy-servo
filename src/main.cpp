#include <Arduino.h>
#include <DigiCDC.h> // Include DigiCDC library

#define MOTOR_PIN1 0
#define MOTOR_PIN2 1
#define PPM_PIN 2
#define POT_PIN 4

#define POT_MIN 0      // Minimum potentiometer value
#define POT_MAX 1023   // Maximum potentiometer value
#define POT_RANGE (POT_MAX - POT_MIN) // Potentiometer range

#define DEADZONE 50    // Define a deadzone value

volatile uint32_t startTime;
volatile uint32_t pulseWidth;
volatile uint32_t pulseWidthMin = 1400; // Initialize to maximum possible value
volatile uint32_t pulseWidthMax = 1800; // Initialize to minimum possible value

void readPPM(); // Function declaration
void printPulseWidth(); // Function declaration

void setup() {
  pinMode(MOTOR_PIN1, OUTPUT); // Motor control pin 0
  pinMode(MOTOR_PIN2, OUTPUT); // Motor control pin 1
  pinMode(PPM_PIN, INPUT); // INT0 is on pin 2
  pinMode(POT_PIN, INPUT); // Potentiometer input pin
  attachInterrupt(0, readPPM, CHANGE); // Attach interrupt to INT0
  SerialUSB.begin(); // Initialize SerialUSB
}

void loop() {
  uint16_t potValue = (analogRead(POT_PIN) - POT_MIN ) * 1000 / POT_RANGE; // Convert potentiometer value to microseconds
  SerialUSB.print("Potentiometer Value: ");
  SerialUSB.println(potValue);

  printPulseWidth(); // Call the function to print pulse width
  uint16_t scaledPulseWidth = (pulseWidth - pulseWidthMin) * 1000 / (pulseWidthMax - pulseWidthMin);
  SerialUSB.print("Scaled Pulse Width: ");
  SerialUSB.println(scaledPulseWidth);

  // Compare potentiometer value to PPM signal and steer motor
  if (abs(scaledPulseWidth - potValue) < DEADZONE) {
    digitalWrite(MOTOR_PIN1, HIGH); // Set both motor outputs high when inside the deadzone
    digitalWrite(MOTOR_PIN2, HIGH);
  } else if (scaledPulseWidth > potValue) {
    digitalWrite(MOTOR_PIN1, HIGH); // Steer motor in one direction
    digitalWrite(MOTOR_PIN2, LOW);
  } else {
    digitalWrite(MOTOR_PIN1, LOW); // Steer motor in the opposite direction
    digitalWrite(MOTOR_PIN2, HIGH);
  }

}

void readPPM() {
  if (digitalRead(PPM_PIN) == HIGH) {
    startTime = micros(); // Record the start time
  } else {
    pulseWidth = micros() - startTime; // Calculate the pulse width
    if (pulseWidth < pulseWidthMin) {
      pulseWidthMin = pulseWidth; // Update minimum pulse width
    }
    if (pulseWidth > pulseWidthMax) {
      pulseWidthMax = pulseWidth; // Update maximum pulse width
    }
  }
}

void printPulseWidth() {
  SerialUSB.print("Pulse Width: ");
  SerialUSB.print(pulseWidth);
  SerialUSB.print(" us, Min: ");
  SerialUSB.print(pulseWidthMin);
  SerialUSB.print(" us, Max: ");
  SerialUSB.println(pulseWidthMax);
}