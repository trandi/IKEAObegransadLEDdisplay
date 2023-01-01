#include "PingPong.h"

namespace {

int toPaddleDelta(Direction dir) {
  switch (dir) {
    case Direction::UP:
      return 1;
    case Direction::LEFT:
      return 1;
    case Direction::DOWN:
      return -1;
    case Direction::RIGHT:
      return -1;
    default:
      return 0;
  }
}

}  // namespace

void Paddle::paint(int grey, std::shared_ptr<IDisplay> display) {
  for (int i = pos_; i < pos_ + size_; i++) {
    display->setPixel({col_, i}, grey);
  }
}

void Paddle::move(int delta, std::shared_ptr<IDisplay> display) {
  paint(0, display);  // remove

  // move
  auto oldPos = pos_;
  pos_ += delta;
  pos_ = max(0, min(max_ - size_, pos_));
  speed_ = pos_ - oldPos;

  paint(grey_, display);  // paint new
}

bool Paddle::contains(Pos point) {
  return point.c == col_ && point.r >= pos_ && point.r < pos_ + size_;
}

void Ball::reset() {
  pos_ = Pos{.c = max_.c / 2, .r = max_.r / 2};
  speed_ = Pos{.c = 1, .r = 0};
}

BallTickResult Ball::tick(std::shared_ptr<Paddle> leftPaddle,
                          std::shared_ptr<Paddle> rightPaddle,
                          std::shared_ptr<IDisplay> display) {
  display->setPixel(pos_, 0);  // remove

  Pos newPos{pos_ + speed_};

  // out on the sides -> point lost
  if (newPos.c < 0) {
    return BallTickResult::LEFT_LOST;
  } else if (newPos.c >= max_.c) {
    return BallTickResult::RIGHT_LOST;
  }

  // hit top/bottom walls -> bounce on Y axis / rows
  if (newPos.r < 0) {
    newPos.r = 0;
    speed_.r = -speed_.r;
  }
  if (newPos.r >= max_.r) {
    newPos.r = max_.r - 1;
    speed_.r = -speed_.r;
  }

  // hit paddles -> bounce on X axis / cols
  if (leftPaddle->contains(newPos)) {
    newPos.c++;
    speed_.c = -speed_.c;
    speed_.r += leftPaddle->speed();
  }
  if (rightPaddle->contains(newPos)) {
    newPos.c--;
    speed_.c = -speed_.c;
    speed_.r += rightPaddle->speed();
  }

  pos_ = newPos;

  display->setPixel(pos_, 8);  // paint new

  return BallTickResult::OK;
}

void PingPongGame::tick(Direction joyLeft, Direction joyRight,
                        GamepadPtr gamePadLeft, GamepadPtr gamePadRight) {
  leftPaddle_->move(toPaddleDelta(joyLeft), display_);
  rightPaddle_->move(toPaddleDelta(joyRight), display_);

  auto ballTickResult = ball_.tick(leftPaddle_, rightPaddle_, display_);
  if (ballTickResult != BallTickResult::OK) {
    ball_.reset();
    if (ballTickResult == BallTickResult::LEFT_LOST) {
      if (score_.incrementRight()) {
        restart(false, gamePadRight);
      }
    } else {
      if (score_.incrementLeft()) {
        restart(true, gamePadLeft);
      }
    }

    score_.display(display_);
  }
}

void Score::display(std::shared_ptr<IDisplay> display) {
  for (int i = 0; i < left_; i++) {
    display->setPixel({1 + i, 0}, GREY);
  }

  auto maxCols = display->max().c;

  for (int i = 0; i < right_; i++) {
    display->setPixel({maxCols - 2 - i, 0}, GREY);
  }
}

void PingPongGame::restart(bool leftWins, GamepadPtr gamePad) {
  display_->off();
  delay(500);
  display_->turnOnHalf(leftWins);
  if (gamePad) {
    gamePad->setRumble(0xc0 /* force */, 0xc0 /* duration */);
  }
  delay(500);
  display_->off();

  delay(500);
  display_->turnOnHalf(leftWins);
  if (gamePad) {
    gamePad->setRumble(0xc0 /* force */, 0xc0 /* duration */);
  }
  delay(500);
  display_->off();

  score_.reset();
}