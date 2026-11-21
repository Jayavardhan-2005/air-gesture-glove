// Wokwi (Uno) simulation of Mode 2: three flex sensors (potentiometer stand-ins).
// index only = 1 HUNGRY, index+middle = 2 WASHROOM, index+middle+ring = 3 EMERGENCY.
// A pattern must be held for 1 s. The alert repeats until the button (pin 2) acknowledges.
// NOTE: uses fixed straight/bent values here. On hardware, run per-finger calibration first.
const int SENSOR_PINS[3] = {A0, A1, A2};
const int BUZZER_PIN = 8;
const int BUTTON_PIN = 2;
const unsigned long HOLD_MS = 1000;

int straightVal[3] = {200, 200, 200};   // placeholders, replace with calibrated values
int bentVal[3]     = {800, 800, 800};
bool fingerBent[3] = {false, false, false};

int lastPattern = 0;
unsigned long patternSince = 0;
bool alertActive = false;
int alertPattern = 0;
unsigned long lastBeep = 0;

int percentActive(int i) {
  int value = analogRead(SENSOR_PINS[i]);
  long range = bentVal[i] - straightVal[i];
  if (range == 0) range = 1;
  return constrain((long)(value - straightVal[i]) * 100 / range, 0, 100);
}

void updateFingers() {
  for (int i = 0; i < 3; i++) {
    int p = percentActive(i);
    if (!fingerBent[i] && p > 66) fingerBent[i] = true;
    else if (fingerBent[i] && p < 33) fingerBent[i] = false;
  }
}

// 0 = none/invalid, 1 = hungry, 2 = washroom, 3 = emergency
int readPattern() {
  if (fingerBent[0] && !fingerBent[1] && !fingerBent[2]) return 1;
  if (fingerBent[0] &&  fingerBent[1] && !fingerBent[2]) return 2;
  if (fingerBent[0] &&  fingerBent[1] &&  fingerBent[2]) return 3;
  return 0;
}

const char* nameOf(int p) {
  if (p == 1) return "HUNGRY";
  if (p == 2) return "WASHROOM";
  if (p == 3) return "EMERGENCY";
  return "";
}

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  updateFingers();
  int pattern = readPattern();

  if (pattern != lastPattern) {
    lastPattern = pattern;
    patternSince = millis();
  }

  if (pattern != 0 && millis() - patternSince >= HOLD_MS) {
    if (!alertActive || alertPattern != pattern) {
      alertActive = true;
      alertPattern = pattern;
      Serial.print("ALERT: "); Serial.println(nameOf(pattern));
    }
  }

  if (alertActive) {
    unsigned long interval = (alertPattern == 3) ? 200 : 800;   // emergency beeps faster
    int freq = (alertPattern == 3) ? 2000 : 1000;
    if (millis() - lastBeep >= interval) {
      tone(BUZZER_PIN, freq, 100);
      lastBeep = millis();
    }
    if (digitalRead(BUTTON_PIN) == LOW) {
      alertActive = false;
      noTone(BUZZER_PIN);
      Serial.println("ACKNOWLEDGED");
    }
  }
  delay(20);
}
