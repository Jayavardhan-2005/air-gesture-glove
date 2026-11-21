// Wokwi (Uno) simulation: calibrate one flex sensor (potentiometer stand-in) on A0.
// Press the button (pin 2) once with the finger straight, once with it bent.
// Then prints a 0-100% bend value with hysteresis (bent >66%, released <33%).
const int FLEX_PIN = A0;
const int BUTTON_PIN = 2;

int straightVal = 0;
int bentVal = 0;
bool isBent = false;

void waitForPress() {
  while (digitalRead(BUTTON_PIN) == HIGH) {}
  delay(30);
  while (digitalRead(BUTTON_PIN) == LOW) {}
  delay(30);
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600);

  Serial.println("Hold finger STRAIGHT, press button");
  waitForPress();
  straightVal = analogRead(FLEX_PIN);

  Serial.println("Hold finger BENT, press button");
  waitForPress();
  bentVal = analogRead(FLEX_PIN);

  Serial.print("straight="); Serial.print(straightVal);
  Serial.print(" bent="); Serial.println(bentVal);
}

void loop() {
  int value = analogRead(FLEX_PIN);
  long range = bentVal - straightVal;
  if (range == 0) range = 1;                       // avoid divide by zero
  int percent = (long)(value - straightVal) * 100 / range;
  percent = constrain(percent, 0, 100);

  if (!isBent && percent > 66) isBent = true;
  else if (isBent && percent < 33) isBent = false;

  Serial.print(percent); Serial.print("%  ");
  Serial.println(isBent ? "BENT" : "STRAIGHT");
  delay(100);
}
