#pragma once

#include <memory>

#include "IGame.h"

class Paddle {
  int size_;
  int col_;  // column is fixed
  int pos_{0};
  int max_;      // max vertical size of the column
  int grey_{3};  // grey shade 0 .. display.maxGrey()
  int speed_{0};

  void paint(int grey, std::shared_ptr<IDisplay> display);

 public:
  // initialise in the middle
  explicit Paddle(int size, int col, int max)
      : size_{size}, col_{col}, max_{max} {
    reset();
  }

  // @return new position
  void move(int delta, std::shared_ptr<IDisplay> display);

  bool contains(Pos point);

  int speed() { return speed_; }

  void reset();
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
                      std::shared_ptr<IDisplay> display);

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

  void display(std::shared_ptr<IDisplay> display);

  void reset() {
    left_ = 0;
    right_ = 0;
  }
};

class PingPongGame : public IGame {
  Ball ball_;
  std::shared_ptr<Paddle> leftPaddle_;
  std::shared_ptr<Paddle> rightPaddle_;
  Score score_;

  static constexpr int PADDLE_SIZE{4};

  void finish(bool leftWins, GamepadPtr gamePad);

 public:
  explicit PingPongGame(std::shared_ptr<IDisplay> display)
      : IGame(display),
        ball_{std::move(display->max()), 0, display->max().r - 1, PADDLE_SIZE},
        leftPaddle_{std::make_shared<Paddle>(PADDLE_SIZE, 0, display->max().c)},
        rightPaddle_{std::make_shared<Paddle>(PADDLE_SIZE, display->max().r - 1,
                                              display->max().c)} {}

  void tick(Direction joyLeft, Direction joyRight, GamepadPtr gamePadLeft,
            GamepadPtr gamePadRight) override;

  void init() override;

  int gameSpeed() override { return 1; }
};
