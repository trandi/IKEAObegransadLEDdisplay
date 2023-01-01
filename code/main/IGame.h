#pragma once

#include <Bluepad32.h>

#include "IDisplay.h"

struct IGame {
  virtual void tick(Direction joyLeft, Direction joyRight,
                    GamepadPtr gamePadLeft, GamepadPtr gamePadRight) = 0;
  virtual void init() = 0;

 protected:
  std::shared_ptr<IDisplay> display_;

  explicit IGame(std::shared_ptr<IDisplay> display)
      : display_{std::move(display)} {}
};