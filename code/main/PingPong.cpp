#include "PingPong.h"

void Paddle::paint(int grey, std::shared_ptr<Display> display) {
  for (int i = pos_; i < pos_ + size_; i++) {
    display->setPixel(col_, i, grey);
  }
}

void Paddle::move(int delta, std::shared_ptr<Display> display) {
  paint(0, display);  // remove

  // move
  pos_ += delta;
  pos_ = max(0, min(max_ - size_, pos_));

  paint(grey_, display);  // paint new
}

bool Paddle::contains(Pos point) {
  return point.x == col_ && point.y >= pos_ && point.y < pos_ + size_;
}

void Ball::reset() { pos_ = Pos{.x = max_.x / 2, .y = max_.y / 2}; }

bool Ball::tick(std::shared_ptr<Paddle> leftPaddle,
                std::shared_ptr<Paddle> rightPaddle,
                std::shared_ptr<Display> display) {
  display->setPixel(pos_.x, pos_.y, 0);  // remove

  Pos newPos{pos_.x + speed_.x, pos_.y + speed_.y};

  // out on the sides -> point lost
  if (newPos.x < 0 || newPos.x >= max_.x) {
    return true;
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
  }
  if (rightPaddle->contains(newPos)) {
    newPos.x--;
    speed_.x = -speed_.x;
  }

  pos_ = newPos;

  display->setPixel(pos_.x, pos_.y, 8);  // paint new

  return false;
}

void PingPongGame::tick(int joyLeft, int joyRight) {
  leftPaddle_->move(joyLeft, display_);
  rightPaddle_->move(joyRight, display_);

  if (ball_.tick(leftPaddle_, rightPaddle_, display_)) {
    ball_.reset();
  }
}