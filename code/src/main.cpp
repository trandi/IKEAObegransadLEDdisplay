#include <Arduino.h>

#include "Display.h"

// constexpr uint8_t TFT_LED = 4;

Display display;

void setup() {
  // pinMode(TFT_LED, OUTPUT);

  pinMode(PIN_LATCH, OUTPUT);
  pinMode(PIN_CLK, OUTPUT);
  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_ENABLE, OUTPUT);
}

void loop() {
  // digitalWrite(TFT_LED, HIGH);
  // delay(1000);
  // digitalWrite(TFT_LED, LOW);
  // delay(1000);

  for (uint8_t r = 0; r < ROWS; r++) {
    for (uint8_t c = 0; c < COLS; c++) {
      display.setPixel(r, c, 1);
      display.refresh();
      delay(30);
      display.setPixel(r, c, 0);
    }
  }

  delay(1000);
}