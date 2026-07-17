
#define PHOTO_PIN A0        //Photo sensor on pin A0
#define GREEN_LED_PIN 2

// Adjust this after watching the Serial Monitor values.
int photoThreshold = 300;

void setup() {
  Serial.begin(9600);
  pinMode(PHOTO_PIN, INPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  digitalWrite(GREEN_LED_PIN, LOW);
  Serial.println("Photo Sensor Test");
}

// Reads the light level and lights the green LED when the
void loop() {
  int value = analogRead(PHOTO_PIN);

  Serial.print("Photo: ");
  Serial.print(value);

  if (value > photoThreshold) {
    Serial.println("  <-- DETECTED");
    digitalWrite(GREEN_LED_PIN, HIGH);
  } else {
    Serial.println();
    digitalWrite(GREEN_LED_PIN, LOW);
  }

  delay(50);
}
