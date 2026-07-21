#include <SoftwareSerial.h>

#define GREEN_LED_PIN 2


SoftwareSerial mp3Serial(8, 9); // mp3Serial(RX, TX)

// Tracks 1-5 (sounds) and 10-12 (instructions).
const int tracks[] = {1, 2, 3, 4, 5, 10, 11, 12};
const int trackCount = sizeof(tracks) / sizeof(tracks[0]);
int currentTrack = 0;

// Play MP3 file 
void playMP3(int fileNumber) {
  // DFPlayer Mini 10-byte command frame
  //   7E FF 06 CMD FB PARA_MSB PARA_LSB CS_MSB CS_LSB EF
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

void setup() {
  Serial.begin(9600);
  mp3Serial.begin(9600);
  pinMode(GREEN_LED_PIN, OUTPUT);
  digitalWrite(GREEN_LED_PIN, LOW);

  Serial.println("MP3 Test: Looping through tracks.");
  delay(1500);  // Let the DFPlayer Mini finish powering up before commands
}

void loop() {
  int track = tracks[currentTrack];

  Serial.print("Playing track ");
  Serial.println(track);

  // Blink LED to show a command was sent
  digitalWrite(GREEN_LED_PIN, HIGH);
  playMP3(track);
  delay(200);
  digitalWrite(GREEN_LED_PIN, LOW);

  // Wait a bit before playing the next
  delay(5000);

  // Move to the next track, wraps back to start
  currentTrack = (currentTrack + 1) % trackCount;
}
