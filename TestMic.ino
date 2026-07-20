#include "arduino_secrets.h"

//readjusted for LM386

#define MIC_PIN A1
#define GREEN_LED_PIN 2
#define RED_LED_PIN 3

// Change this after watching the Serial Monitor
const int micThreshold = 80;

void setup() {
  Serial.begin(9600);

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);

  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);

  Serial.println("LM386 Sound Sensor Test");
}

void loop() {
  int signalMax = 0;
  int signalMin = 1023;

  // Collect samples for 50 ms
  unsigned long startTime = millis();

  while (millis() - startTime < 50) {
    int sample = analogRead(MIC_PIN);

    if (sample > signalMax)
      signalMax = sample;

    if (sample < signalMin)
      signalMin = sample;
  }

  int amplitude = signalMax - signalMin;

  Serial.print("Amplitude: ");
  Serial.println(amplitude);

  if (amplitude > micThreshold) {
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(RED_LED_PIN, LOW);

    Serial.println("SOUND DETECTED");
  } else {
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, LOW);
  }

  delay(20);
}