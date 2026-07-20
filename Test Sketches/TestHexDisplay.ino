#include "arduino_secrets.h"

#define HEX_SER_PIN 4
#define HEX_RCLK_PIN 5
#define HEX_SRCLK_PIN 6

const bool HEX_COMMON_ANODE = true;

const byte SEGMENT_DIGITS[10] = {
  0x3F, // 0
  0x06, // 1
  0x5B, // 2
  0x4F, // 3
  0x66, // 4
  0x6D, // 5
  0x7D, // 6
  0x07, // 7
  0x7F, // 8
  0x6F  // 9
};

byte displayByte(byte pattern) {
  return HEX_COMMON_ANODE ? (byte)~pattern : pattern;
}

void writeTwoDigits(byte leftPattern, byte rightPattern) {
  digitalWrite(HEX_RCLK_PIN, LOW);

  // First shifted byte lands on the second register in the chain.
  shiftOut(HEX_SER_PIN, HEX_SRCLK_PIN, MSBFIRST, displayByte(leftPattern));
  shiftOut(HEX_SER_PIN, HEX_SRCLK_PIN, MSBFIRST, displayByte(rightPattern));
  digitalWrite(HEX_RCLK_PIN, HIGH);
}

void updateHexDisplay(int value) {
  value = constrain(value, 0, 99);
  int tens = value / 10;
  int ones = value % 10;

  writeTwoDigits(SEGMENT_DIGITS[tens], SEGMENT_DIGITS[ones]);

  Serial.print("Display: ");
  Serial.print(tens);
  Serial.println(ones);
}

void setup() {
  Serial.begin(9600);
  pinMode(HEX_SER_PIN, OUTPUT);
  pinMode(HEX_SRCLK_PIN, OUTPUT);
  pinMode(HEX_RCLK_PIN, OUTPUT);
  Serial.println("Hex display Test - Counting 0-99...");
}

void loop() {
  for (int i = 0; i <= 99; i++) {
    updateHexDisplay(i);
    delay(300);
  }
}