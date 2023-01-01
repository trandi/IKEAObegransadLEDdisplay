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