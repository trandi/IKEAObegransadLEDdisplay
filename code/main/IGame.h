#pragma once

#include <Bluepad32.h>

#include "IDisplay.h"

struct IGame {
  virtual void tick(Direction joyLeft, Direction joyRight,
                    GamepadPtr gamePadLeft, GamepadPtr gamePadRight);
};