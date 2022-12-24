#include "Display.h"

namespace {

/*
MISO  27  // GPIO27 -> NOT USED
MOSI  26  // GPIO26 -> PIN_DATA / DI 3rd
SCK   25  // GIPO25 -> PIN_CLK  / CLK 4th
CS    33  // GPIO33 -> PIN_LATCH / CLA 5th
      32  // GPIO32 -> PIN_ENABLE / EN 2nd
*/
constexpr uint8_t PIN_DATA{26};
constexpr uint8_t PIN_CLK{25};
constexpr uint8_t PIN_LATCH{33};
constexpr uint8_t PIN_ENABLE{32};

std::unordered_map<int, std::pair<int, int>> convertPositions() {
  std::unordered_map<int, std::pair<int, int>> orderToPosition;

  for (int r = 0; r < Display::ROWS; r++) {
    for (int c = 0; c < Display::COLS; c++) {
      auto order = Display::POSITIONS[r * Display::COLS + c];
      orderToPosition[order] = {r, c};
    }
  }

  return orderToPosition;
}

void latch() {
  digitalWrite(PIN_LATCH, HIGH);
  digitalWrite(PIN_LATCH, LOW);
  // delayMicroseconds(10);
}

void sendData(
    std::vector<std::vector<int>>& buff,
    const std::unordered_map<int, std::pair<int, int>>& orderToPosition) {
  auto rows = buff.size();
  auto cols = rows > 0 ? buff[0].size() : 0;

  for (size_t i = 0; i < rows * cols; i++) {
    const auto& [r, c] = orderToPosition.at(i);
    digitalWrite(PIN_DATA, buff[r][c] > 0 ? buff[r][c]-- : 0);
    digitalWrite(PIN_CLK, HIGH);
    digitalWrite(PIN_CLK, LOW);
  }
}

}  // namespace

Display::Display() : orderToPosition_(convertPositions()) {
  pinMode(PIN_LATCH, OUTPUT);
  pinMode(PIN_CLK, OUTPUT);
  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_ENABLE, OUTPUT);
}

void Display::setPixel(int x, int y, int greyValue) {
  auto actualX = min(x, COLS - 1);
  auto actualY = min(y, ROWS - 1);
  buffer_[actualX][actualY] = min(greyValue, MAX_GREY_LEVEL);
}

void Display::refresh() {
  std::vector<std::vector<int>> buff{buffer_};

  for (auto pass = 0; pass < MAX_GREY_LEVEL; pass++) {
    sendData(buff, orderToPosition_);

    latch();
  }
}