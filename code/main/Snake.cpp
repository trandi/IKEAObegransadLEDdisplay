#include "Snake.h"

void SnakeGame::tick(Direction dir, Direction /*rightDir*/,
                     GamepadPtr /*gamePadLeft*/, GamepadPtr /*gamePadRight*/) {
  if (dir != Direction::NONE) {
    dir_ = dir;
  }

  auto head = snake_.front();
  Pos newHead = head(dir_);

  if (std::find(snake_.begin(), snake_.end(), newHead) != snake_.end()) {
    Console.printf("Bit itself ! %d", dir_);
    // biting itself
    init();
    return;
  }

  paint(0);  // remove
  snake_.pop_back();
  snake_.insert(snake_.begin(), std::move(newHead));
  paint(4);  // repaint
}

void SnakeGame::init() {
  dir_ = Direction::RIGHT;
  snake_ = std::vector<Pos>{{3, 0}, {2, 0}, {1, 0}, {0, 0}};
  paint(4);
}

void SnakeGame::paint(int grey) {
  for (const auto& pos : snake_) {
    display_->setPixel(pos, grey);
  }
}