// Pen down / pen up on the OLED. Button between D2 and GND (INPUT_PULLUP:
// pin reads LOW while pressed). Shows how long the last press lasted.
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int BUTTON_PIN = 2;
Adafruit_SSD1306 display(128, 64, &Wire, -1);

bool wasPressed = false;
unsigned long pressStart = 0;

void show(const char* line1, const char* line2 = "") {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 8);
  display.println(line1);
  display.setCursor(0, 36);
  display.println(line2);
  display.display();
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true) {}
  }
  show("READY");
}

void loop() {
  bool pressed = (digitalRead(BUTTON_PIN) == LOW);

  if (pressed && !wasPressed) {          // pen down
    pressStart = millis();
    show("PEN DOWN");
  } else if (!pressed && wasPressed) {   // pen up
    char buf[16];
    snprintf(buf, sizeof(buf), "%lu ms", millis() - pressStart);
    show("PEN UP", buf);
  }

  wasPressed = pressed;
  delay(10);                             // crude debounce
}
