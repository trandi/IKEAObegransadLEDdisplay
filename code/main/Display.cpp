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

std::unordered_map<int, Pos> convertPositions() {
  std::unordered_map<int, Pos> orderToPosition;

  for (int c = 0; c < Display::COLS; c++) {
    for (int r = 0; r < Display::ROWS; r++) {
      auto order = Display::POSITIONS[c * Display::ROWS + r];
      orderToPosition[order] = {c, r};
    }
  }

  return orderToPosition;
}

void latch() {
  digitalWrite(PIN_LATCH, HIGH);
  digitalWrite(PIN_LATCH, LOW);
  // delayMicroseconds(10);
}

void sendData(std::vector<std::vector<int>>& buff,
              const std::unordered_map<int, Pos>& orderToPosition) {
  auto rows = buff.size();
  auto cols = rows > 0 ? buff[0].size() : 0;

  for (size_t i = 0; i < rows * cols; i++) {
    const auto& pos = orderToPosition.at(i);
    digitalWrite(PIN_DATA, buff[pos.c][pos.r] > 0 ? buff[pos.c][pos.r]-- : 0);
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

bool Display::setPixel(Pos pos, int greyValue) {
  // validate input
  if (pos.r < 0 || pos.r >= ROWS || pos.c < 0 || pos.c >= COLS ||
      greyValue < 0 || greyValue > MAX_GREY_LEVEL) {
    return false;
  }

  buffer_[pos.c][pos.r] = greyValue;

  return true;
}

void Display::refresh() {
  std::vector<std::vector<int>> buff{buffer_};

  for (auto pass = 0; pass < MAX_GREY_LEVEL; pass++) {
    sendData(buff, orderToPosition_);

    latch();
  }
}

void Display::off() {
  for (int c = 0; c < COLS; c++) {
    for (int r = 0; r < ROWS; r++) {
      setPixel({c, r}, 0);
    }
  }
}

void Display::turnOnHalf(bool left) {
  for (int c = 0; c < COLS; c++) {
    for (int r = 0; r < ROWS; r++) {
      if (c <= COLS / 2) {
        setPixel({c, r}, left ? 1 : 0);
      } else {
        setPixel({c, r}, left ? 0 : 1);
      }
    }
  }
}

Pos Pos::operator+(Pos another) {
  return {.c = c + another.c, .r = r + another.r};
}

Pos Pos::operator()(Direction dir) {
  switch (dir) {
    case Direction::UP:
      return {.c = c, .r = r - 1};

    default:
      return *this;
  };
}
