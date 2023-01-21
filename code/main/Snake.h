#pragma once

#include <Bluepad32.h>

#include <memory>
#include <random>
#include <vector>

#include "IGame.h"

class SnakeGame : public IGame {
  int gameSpeed_{1};
  std::vector<Pos> snake_;
  Direction dir_;
  Pos prey_;
  std::random_device rd_;
  std::mt19937 gen_{rd_()};
  std::uniform_int_distribution<> distC_;
  std::uniform_int_distribution<> distR_;

  void endGame(GamepadPtr gamePad);
  void paint(int grey);
  Pos randomPos();

 public:
  explicit SnakeGame(std::shared_ptr<IDisplay> display)
      : IGame{std::move(display)},
        distC_{0, display_->max().c - 1},
        distR_{0, display_->max().r - 1} {
    init();
  }

  void tick(Direction leftDir, Direction rightDir, GamepadPtr gamePadLeft,
            GamepadPtr gamePadRight) override;

  void init() override;

  int gameSpeed() override { return gameSpeed_; }
};