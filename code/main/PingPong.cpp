#include "PingPong.h"

void Paddle::paint(int grey, std::shared_ptr<Display> display) {
  for (int i = pos_; i < pos_ + size_; i++) {
    display->setPixel(col_, i, grey);
  }
}

void Paddle::move(int delta, std::shared_ptr<Display> display) {
  paint(0, display);  // remove

  // move
  auto oldPos = pos_;
  pos_ += delta;
  pos_ = max(0, min(max_ - size_, pos_));
  speed_ = pos_ - oldPos;

  paint(grey_, display);  // paint new
}

bool Paddle::contains(Pos point) {
  return point.x == col_ && point.y >= pos_ && point.y < pos_ + size_;
}

void Ball::reset() {
  pos_ = Pos{.x = max_.x / 2, .y = max_.y / 2};
  speed_ = Pos{.x = 1, .y = 0};
}

BallTickResult Ball::tick(std::shared_ptr<Paddle> leftPaddle,
                          std::shared_ptr<Paddle> rightPaddle,
                          std::shared_ptr<Display> display) {
  display->setPixel(pos_.x, pos_.y, 0);  // remove

  Pos newPos{pos_.x + speed_.x, pos_.y + speed_.y};

  // out on the sides -> point lost
  if (newPos.x < 0) {
    return BallTickResult::LEFT_LOST;
  } else if (newPos.x >= max_.x) {
    return BallTickResult::RIGHT_LOST;
  }

  // hit top/bottom walls -> bounce on Y axis
  if (newPos.y < 0) {
    newPos.y = 0;
    speed_.y = -speed_.y;
  }
  if (newPos.y >= max_.y) {
    newPos.y = max_.y - 1;
    speed_.y = -speed_.y;
  }

  // hit paddles -> bounce on X axis
  if (leftPaddle->contains(newPos)) {
    newPos.x++;
    speed_.x = -speed_.x;
    speed_.y += leftPaddle->speed();
  }
  if (rightPaddle->contains(newPos)) {
    newPos.x--;
    speed_.x = -speed_.x;
    speed_.y += rightPaddle->speed();
  }

  pos_ = newPos;

  display->setPixel(pos_.x, pos_.y, 8);  // paint new

  return BallTickResult::OK;
}

void PingPongGame::tick(int joyLeft, int joyRight) {
  leftPaddle_->move(joyLeft, display_);
  rightPaddle_->move(joyRight, display_);

  auto ballTickResult = ball_.tick(leftPaddle_, rightPaddle_, display_);
  if (ballTickResult != BallTickResult::OK) {
    ball_.reset();
    if (ballTickResult == BallTickResult::LEFT_LOST) {
      if (score_.incrementRight()) {
        restart(false);
      }
    } else {
      if (score_.incrementLeft()) {
        restart(true);
      }
    }

    score_.display(display_);
  }
}

void Score::display(std::shared_ptr<Display> display) {
  for (int i = 0; i < left_; i++) {
    display->setPixel(1 + i, 0, GREY);
  }

  for (int i = 0; i < right_; i++) {
    display->setPixel(Display::COLS - 2 - i, 0, GREY);
  }
}

void PingPongGame::restart(bool leftWins) {
  display_->off();
  delay(500);
  display_->turnOnHalf(leftWins);
  delay(500);
  display_->off();
  delay(500);
  display_->turnOnHalf(leftWins);
  delay(500);
  display_->off();

  score_.reset();
}