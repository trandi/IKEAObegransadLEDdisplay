#pragma once

enum class Direction { UP, DOWN, LEFT, RIGHT, NONE };

struct Pos {
  int c;
  int r;

  Pos operator()(Direction dir);

  Pos operator+(const Pos& another);

  bool operator==(const Pos& another);
};

struct IDisplay {
  // grey value : 0 = off -> MAX_GREY_LEVEL fully on
  // @return false if input out of range
  virtual bool setPixel(const Pos& pos, int greyValue);

  virtual void off();

  virtual void turnOnHalf(bool left);

  // does the MAX_GREY_LEVEL passes generating the nuances of grey
  virtual void refresh();

  virtual Pos max();

  virtual int maxGrey();
};