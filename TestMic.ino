// Serial Monitor at 9600 baud should show live readings.

#define MIC_PIN A1          //Microphone should be on pin A1
#define GREEN_LED_PIN 2
#define RED_LED_PIN 3

// Adjust this after watching the Serial Monitor values.
int micThreshold = 400;

void setup() {
  Serial.begin(9600);
  pinMode(MIC_PIN, INPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  digitalWrite(GREEN_LED_PIN, LOW);
  Serial.println("Mic Test");
}

// Reads the mic and lights the green LED when sound is detected.
void loop() {
  int value = analogRead(MIC_PIN);

  Serial.print("Mic: ");
  Serial.print(value);

  if (value > micThreshold) {
    Serial.println("DETECTED");
    digitalWrite(GREEN_LED_PIN, HIGH);
  } else {
    Serial.println();
    digitalWrite(GREEN_LED_PIN, LOW);
  }

  delay(50);
}
