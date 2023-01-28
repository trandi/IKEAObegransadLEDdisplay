#pragma once

#include <vector>

enum class Direction { UP, DOWN, LEFT, RIGHT, NONE };

struct Pos {
  int c;
  int r;

  Pos operator()(Direction dir);

  Pos operator+(const Pos& another);

  bool operator==(const Pos& another);
};

class IDisplay {
  const Pos max_;
  const int maxGrey_;

 protected:
  std::vector<std::vector<int>> buffer_;

  explicit IDisplay(const Pos& max, int maxGrey)
      : max_{max},
        maxGrey_{maxGrey},
        buffer_{
            std::vector<std::vector<int>>(max.r, std::vector<int>(max.c, 0))} {}

 public:
  // grey value : 0 = off -> MAX_GREY_LEVEL fully on
  // @return false if input out of range
  bool setPixel(const Pos& pos, int greyValue);

  Pos max() { return max_; }

  int maxGrey() { return maxGrey_; }

  void off();

  void turnOnHalf(bool left);

  // does the MAX_GREY_LEVEL passes generating the nuances of grey
  virtual void refresh() = 0;
};