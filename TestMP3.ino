#include <SoftwareSerial.h>

#define GREEN_LED_PIN 2

// MP3 player serial (RX, TX) - matches the main game wiring.
SoftwareSerial mp3Serial(0, 1);

// Tracks1-5 (sounds) and 10-12 (instructions).
const int tracks[] = {1, 2, 3, 4, 5, 10, 11, 12};
const int trackCount = sizeof(tracks) / sizeof(tracks[0]);
int currentTrack = 0;

// Play MP3 file (same command format as the main game / DFPlayer Mini).
void playMP3(int fileNumber) {
  byte cmd[] = {0x7E, 0x04, 0x08, 0x00, fileNumber, 0xEF};
  mp3Serial.write(cmd, 6);
}

void setup() {
  Serial.begin(9600);
  mp3Serial.begin(9600);
  pinMode(GREEN_LED_PIN, OUTPUT);
  digitalWrite(GREEN_LED_PIN, LOW);

  Serial.println("MP3 Test: Looping through tracks.");
  delay(1000);
}

void loop() {
  int track = tracks[currentTrack];

  Serial.print("Playing track ");
  Serial.println(track);

  // Blink LED to show a command was sent.
  digitalWrite(GREEN_LED_PIN, HIGH);
  playMP3(track);
  delay(200);
  digitalWrite(GREEN_LED_PIN, LOW);

  // Give each track time to play before the next one.
  delay(5000);

  // Move to the next track, wrapping back to the start.
  currentTrack = (currentTrack + 1) % trackCount;
}
