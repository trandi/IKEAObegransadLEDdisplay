#pragma once

#include <memory>

#include "Display.h"

struct Pos {
  int x;
  int y;
};

class Paddle {
  int size_;
  int col_;  // column is fixed
  int pos_;
  int max_;      // max vertical size of the column
  int grey_{3};  // grey shade 0 .. Display::MAX_GREY_LEVEL
  int speed_{0};

  void paint(int grey, std::shared_ptr<Display> display);

 public:
  // initialise in the middle
  explicit Paddle(int size, int col, int max)
      : size_{size}, col_{col}, pos_{(max - size) / 2}, max_{max} {}

  // @return new position
  void move(int delta, std::shared_ptr<Display> display);

  bool contains(Pos point);

  int speed() { return speed_; }
};

enum class BallTickResult { OK, LEFT_LOST, RIGHT_LOST };

class Ball {
  Pos pos_;
  Pos speed_{1, 0};
  Pos max_;
  int paddleLeftCol_;
  int paddleRightCol_;
  int paddleSize_;

 public:
  // initialise in the middle
  explicit Ball(Pos max, int paddleLeftCol, int paddleRightCol, int paddleSize)
      : max_{max},
        paddleLeftCol_{paddleLeftCol},
        paddleRightCol_{paddleRightCol},
        paddleSize_{paddleSize} {
    reset();
  }

  // @return true if point lost
  BallTickResult tick(std::shared_ptr<Paddle> leftPaddle,
                      std::shared_ptr<Paddle> rightPaddle,
                      std::shared_ptr<Display> display);

  void reset();
};

class Score {
  int left_{0};
  int right_{0};

  static constexpr int GREY{1};
  static constexpr int MAX{5};

 public:
  // @returns true if end of game
  bool incrementLeft() {
    left_++;
    return left_ == MAX;
  }
  bool incrementRight() {
    right_++;
    return right_ == MAX;
  }

  void display(std::shared_ptr<Display> display);

  void reset() {
    left_ = 0;
    right_ = 0;
  }
};

class PingPongGame {
  Ball ball_;
  std::shared_ptr<Paddle> leftPaddle_;
  std::shared_ptr<Paddle> rightPaddle_;
  Pos max_;
  std::shared_ptr<Display> display_;
  Score score_;

  static constexpr int PADDLE_SIZE{4};

 public:
  explicit PingPongGame(Pos max, std::shared_ptr<Display> display)
      : ball_{max, 0, max.x - 1, PADDLE_SIZE},
        leftPaddle_{std::make_shared<Paddle>(PADDLE_SIZE, 0, max.y)},
        rightPaddle_{std::make_shared<Paddle>(PADDLE_SIZE, max.x - 1, max.y)},
        max_{std::move(max)},
        display_{std::move(display)} {}

  void tick(int joyLeft, int joyRight);

  void restart(bool leftWins);
};
