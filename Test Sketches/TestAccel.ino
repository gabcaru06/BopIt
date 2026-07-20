#include "arduino_secrets.h"

#include <Wire.h>

#define GREEN_LED_PIN 2

int accelThreshold = 5000;

void writeRegister(byte reg, byte data)
{
  Wire.beginTransmission(0x68);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}


int16_t readWord(byte reg)
{
  Wire.beginTransmission(0x68);
  Wire.write(reg);
  Wire.endTransmission(false);

  Wire.requestFrom(0x68, 2);

  int16_t value = Wire.read() << 8 | Wire.read();

  return value;
}


void setup()
{
  Serial.begin(9600);

  pinMode(GREEN_LED_PIN, OUTPUT);

  Wire.begin();

  // Wake MPU6050
  writeRegister(0x6B, 0x00);

  delay(100);

  // Check device ID
  Wire.beginTransmission(0x68);
  Wire.write(0x75);
  Wire.endTransmission(false);

  Wire.requestFrom(0x68, 1);

  byte id = Wire.read();

  Serial.print("WHO_AM_I: ");
  Serial.println(id, HEX);

  if(id == 0x68)
    Serial.println("MPU6050 detected");
  else
    Serial.println("MPU6050 NOT detected");
}


void loop()
{
  int16_t accelX = readWord(0x3B);

  Serial.print("Accel X: ");
  Serial.println(accelX);


  if(abs(accelX) > accelThreshold)
  {
    digitalWrite(GREEN_LED_PIN, HIGH);
    Serial.println("MOTION DETECTED");
  }
  else
  {
    digitalWrite(GREEN_LED_PIN, LOW);
  }

  delay(100);
}
