#include <ESP32Servo.h>

// Create servo object
Servo myServo;

// Define control pin
const int servoPin = 1; // GPIO 1 connected to Yellow signal wire

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting Positional Servo Control...");

  // Allow standard 50Hz PWM signal for digital servos
  ESP32PWM::allocateTimer(0);
  myServo.setPeriodHertz(50);

  // Attach servo to GPIO 1 with pulse widths mapped to 500us - 2500us
  // (Matched to the DS-M005 datasheet range for full 0° to 180° rotation)
  myServo.attach(servoPin, 500, 2500);
}

void loop() {
  Serial.println("Moving to 0 degrees...");
  myServo.write(0);
  delay(2000);

  Serial.println("Moving to 90 degrees (Center)...");
  myServo.write(90);
  delay(2000);

  Serial.println("Moving to 180 degrees...");
  myServo.write(180);
  delay(2000);
}