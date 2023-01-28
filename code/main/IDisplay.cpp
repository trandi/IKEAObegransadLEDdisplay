#include "IDisplay.h"

Pos Pos::operator+(const Pos& another) {
  return {.c = c + another.c, .r = r + another.r};
}

Pos Pos::operator()(Direction dir) {
  switch (dir) {
    case Direction::UP:
      return {.c = c, .r = r + 1};
    case Direction::DOWN:
      return {.c = c, .r = r - 1};
    case Direction::LEFT:
      return {.c = c - 1, .r = r};
    case Direction::RIGHT:
      return {.c = c + 1, .r = r};

    default:
      return *this;
  };
}

bool Pos::operator==(const Pos& another) {
  return c == another.c && r == another.r;
}

void IDisplay::off() {
  auto [maxCols, maxRows] = max();
  for (int c = 0; c < maxCols; c++) {
    for (int r = 0; r < maxRows; r++) {
      setPixel({c, r}, 0);
    }
  }
}

void IDisplay::turnOnHalf(bool left) {
  auto [maxCols, maxRows] = max();
  for (int c = 0; c < maxCols; c++) {
    for (int r = 0; r < maxRows; r++) {
      if (c <= maxCols / 2) {
        setPixel({c, r}, left ? 1 : 0);
      } else {
        setPixel({c, r}, left ? 0 : 1);
      }
    }
  }
}

bool IDisplay::setPixel(const Pos& pos, int greyValue) {
  // validate input
  if (pos.r < 0 || pos.r >= max_.r || pos.c < 0 || pos.c >= max_.c ||
      greyValue < 0 || greyValue > maxGrey_) {
    return false;
  }

  buffer_[pos.c][pos.r] = greyValue;

  return true;
}