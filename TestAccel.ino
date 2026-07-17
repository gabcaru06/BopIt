#include <SPI.h>

#define ACCEL_CS A4
#define GREEN_LED_PIN 2
// TERRY NOTE PROPER ADRESS IS 0x68

// Adjust Threshold
int accelThreshold = 100;

int readAccelerator() {
  digitalWrite(ACCEL_CS, LOW);
  SPI.transfer(0x28 | 0x80); // read bit set (only reads a single register may need to change this)
  int value = SPI.transfer(0);
  digitalWrite(ACCEL_CS, HIGH);
  return value;
}

void setup() {
  Serial.begin(9600);
  pinMode(GREEN_LED_PIN, OUTPUT);
  digitalWrite(GREEN_LED_PIN, LOW);

  pinMode(ACCEL_CS, OUTPUT);
  digitalWrite(ACCEL_CS, HIGH);
  SPI.begin();

  Serial.println("Accelerometer Test");
}

// Reads one axis register and lights the green LED when motion
void loop() {
  int value = readAccelerator();

  Serial.print("Accel: ");
  Serial.print(value);

  if (value > accelThreshold) {
    Serial.println("DETECTED");
    digitalWrite(GREEN_LED_PIN, HIGH);
  } else {
    Serial.println();
    digitalWrite(GREEN_LED_PIN, LOW);
  }

  delay(50);
}
