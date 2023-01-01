#pragma once

#include <Bluepad32.h>

#include <memory>
#include <vector>

#include "IGame.h"

class SnakeGame : public IGame {
  std::vector<Pos> snake_;
  Direction dir_;

  void endGame(GamepadPtr gamePad);
  void paint(int grey);

 public:
  explicit SnakeGame(std::shared_ptr<IDisplay> display)
      : IGame{std::move(display)} {
    init();
  }

  void tick(Direction leftDir, Direction rightDir, GamepadPtr gamePadLeft,
            GamepadPtr gamePadRight) override;

  void init() override;
};