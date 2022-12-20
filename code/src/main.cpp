#include <Arduino.h>

#include "Display.h"

// constexpr uint8_t TFT_LED = 4;

Display display;
TaskHandle_t dispRefreshHandle;

void dispRefresh(void* arg) {
  Serial.printf("Display refresh running on core: %d\n", xPortGetCoreID());

  while (true) {
    display.refresh();
    vTaskDelay(1);
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);
  // pinMode(TFT_LED, OUTPUT);

  pinMode(PIN_LATCH, OUTPUT);
  pinMode(PIN_CLK, OUTPUT);
  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_ENABLE, OUTPUT);

  xTaskCreatePinnedToCore(dispRefresh, /* Function to implement the task */
                          "dispRefreshTask",  /* Name of the task */
                          10000,              /* Stack size in words */
                          NULL,               /* Task input parameter */
                          1,                  /* Priority of the task */
                          &dispRefreshHandle, /* Task handle. */
                          0); /* Core where the task should run */

  // pattern testing all 7 levels of grey
  for (int i = 0; i < 8; i++) {
    display.setPixel(i, i, i);
    display.setPixel(8 + i, 8 + i, i);
  }

  Serial.printf("Main code running on core: %d\n", xPortGetCoreID());
}

void loop() {
  // digitalWrite(TFT_LED, HIGH);
  // delay(1000);
  // digitalWrite(TFT_LED, LOW);
  // delay(1000);

  // for (uint8_t r = 0; r < ROWS; r++) {
  //   for (uint8_t c = 0; c < COLS; c++) {
  //     display.setPixel(r, c, 1);
  //     display.refresh();
  //     delay(30);
  //     display.setPixel(r, c, 0);
  //   }
  // }

  vTaskDelay(1);
}
