// OLED "HELLO" test. Board: Arduino Nano 33 BLE. Display: SSD1306 128x64, I2C 0x3C.
// Wiring: GND->GND, VDD->3V3, SCK->A5 (SCL), SDA->A4.
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found");
    while (true) {}
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println("HELLO");
  display.display();
}

void loop() {}
