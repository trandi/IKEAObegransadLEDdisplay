#include "Display.h"

namespace {

std::unordered_map<uint8_t, std::pair<uint8_t, uint8_t>> convertPositions() {
  std::unordered_map<uint8_t, std::pair<uint8_t, uint8_t>> orderToPosition;

  for (uint8_t r = 0; r < ROWS; r++) {
    for (uint8_t c = 0; c < COLS; c++) {
      orderToPosition[kPositions[r * COLS + c]] = {r, c};
    }
  }

  return orderToPosition;
}

uint8_t buff[ROWS][COLS];

}  // namespace

Display::Display() : orderToPosition_(convertPositions()) {}

void Display::setPixel(uint8_t x, uint8_t y, uint8_t greyValue) {
  buffer_[x][y] = constrain(greyValue, 0, 7);
}

void Display::refresh() {
  std::copy(&buffer_[0][0], &buffer_[0][0] + ROWS * COLS, &buff[0][0]);

  for (auto pass = 0; pass < 7; pass++) {
    for (size_t i = 0; i < ROWS * COLS; i++) {
      const auto& [r, c] = orderToPosition_.at(i);
      digitalWrite(PIN_DATA, buff[r][c] > 0 ? buff[r][c]-- : 0);
      digitalWrite(PIN_CLK, HIGH);
      digitalWrite(PIN_CLK, LOW);
    }

    digitalWrite(PIN_LATCH, HIGH);
    digitalWrite(PIN_LATCH, LOW);
    // delayMicroseconds(10);
  }
}