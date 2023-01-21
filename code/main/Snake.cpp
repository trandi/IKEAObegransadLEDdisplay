#include "Snake.h"

void SnakeGame::tick(Direction dir, Direction /*rightDir*/, GamepadPtr gamePad,
                     GamepadPtr /*gamePadRight*/) {
  if (dir != Direction::NONE) {
    dir_ = dir;
  }

  auto head = snake_.front();
  Pos newHead = head(dir_);

  // END GAME conditions

  bool bitItself{std::find(snake_.begin(), snake_.end(), newHead) !=
                 snake_.end()};
  auto mx = display_->max();
  bool hitEdge{newHead.c < 0 || newHead.c >= mx.c || newHead.r < 0 ||
               newHead.r >= mx.r};
  if (bitItself || hitEdge) {
    endGame(gamePad);
    init();
    return;
  }

  paint(0);  // remove

  // Prey
  if (newHead == prey_) {
    prey_ = randomPos();
    gameSpeed_ += 1;
  } else {
    snake_.pop_back();
  }

  snake_.insert(snake_.begin(), std::move(newHead));
  paint(4);  // repaint
}

void SnakeGame::init() {
  display_->off();
  dir_ = Direction::RIGHT;
  snake_ = std::vector<Pos>{{3, 0}, {2, 0}, {1, 0}, {0, 0}};
  paint(4);

  gameSpeed_ = 1;
  prey_ = randomPos();
}

void SnakeGame::endGame(GamepadPtr gamePad) {
  if (gamePad) {
    gamePad->setRumble(0xc0 /* force */, 0xc0 /* duration */);
  }
  display_->off();
  delay(500);
  display_->turnOnHalf(true);

  delay(500);
  display_->off();
  display_->turnOnHalf(false);
  delay(500);
  display_->off();
}

void SnakeGame::paint(int grey) {
  for (const auto& pos : snake_) {
    display_->setPixel(pos, grey);
  }
}

Pos SnakeGame::randomPos() {
  Pos p{.c = distC_(gen_), .r = distR_(gen_)};
  bool onTheSnake = std::find(snake_.begin(), snake_.end(), p) != snake_.end();
  if (onTheSnake) {
    return randomPos();
  }

  display_->setPixel(p, 7);

  return p;
}