#include <SoftwareSerial.h>
#include <Wire.h>

// Pin definitions
#define MIC_PIN A1
#define PHOTO_PIN A2
#define GREEN_LED_PIN 2
#define RED_LED_PIN 3
#define BUTTON_PIN 7
#define HEX_SER_PIN 4
#define HEX_RCLK_PIN 5
#define HEX_SRCLK_PIN 6

// MPU6050 accelerometer (I2C)
#define MPU6050_ADDR 0x68

// MP3 player serial
SoftwareSerial mp3Serial(8, 9);

// Game variables
int score = 0;
int responseTime = 2000;  // milliseconds
const int MIN_RESPONSE_TIME = 300;

// Sensor thresholds (TODO: change values to correct values after testing)
int micThreshold = 80;
int photoThreshold = 300;
int accelThreshold = 5000;

const bool HEX_COMMON_ANODE = true;

// Actions
#define INTIMIDATE 1
#define ADMIRE 2
#define BRIBE 3

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
  if (HEX_COMMON_ANODE) {
    return (byte)~pattern;
  } else {
    return pattern;
  }
}

void writeTwoDigits(byte leftPattern, byte rightPattern) {
  digitalWrite(HEX_RCLK_PIN, LOW);
  // First shifted byte lands on the second (farther) register in the chain.
  shiftOut(HEX_SER_PIN, HEX_SRCLK_PIN, MSBFIRST, displayByte(leftPattern));
  shiftOut(HEX_SER_PIN, HEX_SRCLK_PIN, MSBFIRST, displayByte(rightPattern));
  digitalWrite(HEX_RCLK_PIN, HIGH);
}

//HARDWARE INITIALIZATION
void initializeHardware() {
  Serial.begin(9600);
  mp3Serial.begin(9600);
  Wire.begin();
  
  // Input sensors
  pinMode(MIC_PIN, INPUT);
  pinMode(PHOTO_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Output LEDs
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);

  // 74HC595 display control pins
  pinMode(HEX_SER_PIN, OUTPUT);
  pinMode(HEX_SRCLK_PIN, OUTPUT);
  pinMode(HEX_RCLK_PIN, OUTPUT);

  // Accelerometer (MPU6050 over I2C)
  writeRegister(0x6B, 0x00);  // Wake MPU6050
  delay(100);

  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x75);  // WHO_AM_I register
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_ADDR, 1);
  byte accelId = Wire.read();
  Serial.print("WHO_AM_I: ");
  Serial.println(accelId, HEX);
  if (accelId == MPU6050_ADDR) {
    Serial.println("MPU6050 detected");
  } else {
    Serial.println("MPU6050 NOT detected");
  }
  
  // Initialize hex display with score 0
  updateHexDisplay(0);
  
  Serial.println("Hardware initialized");
}

// Play MP3 file
// TODO: Add MP3 files to microSD card (01.mp3-05.mp3, 10.mp3-12.mp3)
void playMP3(int fileNumber) {
  // DFPlayer Mini 10-byte command frame:
  //   7E FF 06 CMD FB PARA_MSB PARA_LSB CS_MSB CS_LSB EF
  // CMD 0x03 = play track by index; parameter = track number.
  byte cmd = 0x03;
  byte paraMSB = highByte(fileNumber);
  byte paraLSB = lowByte(fileNumber);
  int16_t checksum = -(0xFF + 0x06 + cmd + 0x00 + paraMSB + paraLSB);
  byte frame[10] = {
    0x7E, 0xFF, 0x06, cmd, 0x00,
    paraMSB, paraLSB,
    highByte(checksum), lowByte(checksum),
    0xEF
  };
  mp3Serial.write(frame, 10);
}

// SENSOR READ FUNCTIONS 
void writeRegister(byte reg, byte data) {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

int16_t readWord(byte reg) {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU6050_ADDR, 2);

  int16_t value = Wire.read() << 8 | Wire.read();

  return value;
}

int readAccelerator() {
  int16_t accelX = readWord(0x3B);
  return abs(accelX);
}

int readPhotoSensor() {
  return analogRead(PHOTO_PIN);
}

int readMicrophone() {
  int signalMax = 0;
  int signalMin = 1023;

  // Collect samples for 50 ms (readjusted for LM386)
  unsigned long startTime = millis();

  while (millis() - startTime < 50) {
    int sample = analogRead(MIC_PIN);

    if (sample > signalMax)
      signalMax = sample;

    if (sample < signalMin)
      signalMin = sample;
  }

  return signalMax - signalMin;
}

// HEX DISPLAY
void updateHexDisplay(int value) {
  value = constrain(value, 0, 99);
  int tens = value / 10;
  int ones = value % 10;

  writeTwoDigits(SEGMENT_DIGITS[tens], SEGMENT_DIGITS[ones]);
  
  Serial.print("Display: ");
  Serial.println(value);
}

// LED FEEDBACK
void flashGreenLED() {
  digitalWrite(GREEN_LED_PIN, HIGH);
  delay(1000);
  digitalWrite(GREEN_LED_PIN, LOW);
}

void flashRedLED() {
  digitalWrite(RED_LED_PIN, HIGH);
  delay(1000);
  digitalWrite(RED_LED_PIN, LOW);
}

// GAME FUNCTIONS 
int getRandomAction() {
  return random(1, 4);  // 1=INTIMIDATE, 2=ADMIRE, 3=BRIBE
}

void playInstruction(int action) {
  // Tracks 6, 7, 8 are INTIMIDATE, ADMIRE, BRIBE instructions
  playMP3(5 + action);
  delay(1000);
}

bool waitForAction(int expectedAction) {
  unsigned long startTime = millis();
  
  while (millis() - startTime < responseTime) {
    if (expectedAction == INTIMIDATE && readAccelerator() > accelThreshold) {
      return true;
    }
    if (expectedAction == ADMIRE && readMicrophone() > micThreshold) {
      return true;
    }
    if (expectedAction == BRIBE && readPhotoSensor() < photoThreshold) {
      return true;
    }
    delay(10);
  }
  
  return false;  // Timeout
}

void decreaseResponseTime() {
  responseTime = max(MIN_RESPONSE_TIME, responseTime - 100);
}

// Setup
void setup() {
  randomSeed(analogRead(A0));
  initializeHardware();
  playMP3(1);  // Startup sound
}

// Main loop
void loop() {
  // Wait for player to press start button
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(50);
    playMP3(2);  // Start sound
    delay(1000);
    
    score = 0;
    responseTime = 2000;
    
    // MAIN GAME LOOP
    while (score < 99) {
      // Randomly select command
      int action = getRandomAction();
      
      // Play instruction
      playInstruction(action);
      
      // Wait for player input
      if (waitForAction(action)) {
        // Correct action
        score++;
        updateHexDisplay(score);
        flashGreenLED();
        playMP3(3);  // Success sound
        delay(500);
        
        // Decrease response time for next round
        decreaseResponseTime();
      } else {
        // Wrong action or timeout
        flashRedLED();
        playMP3(4);  // Failure sound
        delay(1000);
        break;  // Game over
      }
    }
    
    // Check win condition
    if (score >= 99) {
      playMP3(5);  // Victory sound
      Serial.print("WIN! Final Score: ");
      Serial.println(score);
    } else {
      Serial.print("Game Over! Final Score: ");
      Serial.println(score);
    }
    
    delay(2000);
  }
  
  delay(10);
}
