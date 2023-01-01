#pragma once

#include <Bluepad32.h>

#include <memory>
#include <vector>

#include "IGame.h"

class SnakeGame : public IGame {
  std::vector<Pos> snake_;
  std::shared_ptr<IDisplay> display_;
  Direction dir_;

  void init();
  void paint(int grey);

 public:
  explicit SnakeGame(std::shared_ptr<IDisplay> display)
      : display_{std::move(display)} {
    init();
  }

  void tick(Direction leftDir, Direction rightDir, GamepadPtr gamePadLeft,
            GamepadPtr gamePadRight) override;
};