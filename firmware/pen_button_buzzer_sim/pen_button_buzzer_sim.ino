// Wokwi (Arduino Uno) simulation: pen down/up with buzzer feedback.
// Button on pin 2 (INPUT_PULLUP), buzzer on pin 8.
const int BUTTON_PIN = 2;
const int BUZZER_PIN = 8;

bool wasPressed = false;
unsigned long pressStart = 0;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  bool pressed = (digitalRead(BUTTON_PIN) == LOW);

  if (pressed && !wasPressed) {
    pressStart = millis();
    Serial.println("PEN DOWN");
    digitalWrite(BUZZER_PIN, HIGH);
    delay(50);
    digitalWrite(BUZZER_PIN, LOW);
  } else if (!pressed && wasPressed) {
    Serial.print("PEN UP, held ");
    Serial.print(millis() - pressStart);
    Serial.println(" ms");
    digitalWrite(BUZZER_PIN, HIGH);
    delay(50);
    digitalWrite(BUZZER_PIN, LOW);
  }

  wasPressed = pressed;
  delay(10);
}
