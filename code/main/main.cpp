#include "sdkconfig.h"
#ifndef CONFIG_BLUEPAD32_PLATFORM_ARDUINO
#error "Must only be compiled when using Bluepad32 Arduino platform"
#endif  // !CONFIG_BLUEPAD32_PLATFORM_ARDUINO

#include <Arduino.h>
#include <Bluepad32.h>

#include <memory>

/********************/
#include "ObegransadDisplay.h"
#include "PingPong.h"
#include "Snake.h"

TaskHandle_t dispRefreshHandle;

GamepadPtr myGamepads[BP32_MAX_GAMEPADS];
std::shared_ptr<IDisplay> display{std::make_shared<ObegransadDisplay>()};
std::vector<std::shared_ptr<IGame>> games{
    std::make_shared<PingPongGame>(display),
    std::make_shared<SnakeGame>(display)};
int gameIdx{0};
time_t lastGameChange{time(nullptr)};

void dispRefresh(void* arg) {
  Console.printf("~~~~~ Display refresh running on core: %d\n",
                 xPortGetCoreID());

  while (true) {
    display->refresh();
    vTaskDelay(1);
  }
}

// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedGamepad(GamepadPtr gp) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myGamepads[i] == nullptr) {
      Console.printf("CALLBACK: Gamepad is connected, index=%d\n", i);
      // Additionally, you can get certain gamepad properties like:
      // Model, VID, PID, BTAddr, flags, etc.
      GamepadProperties properties = gp->getProperties();
      Console.printf("Gamepad model: %s, VID=0x%04x, PID=0x%04x\n",
                     gp->getModelName(), properties.vendor_id,
                     properties.product_id);
      myGamepads[i] = gp;
      foundEmptySlot = true;
      break;
    }
  }
  if (!foundEmptySlot) {
    Console.println(
        "CALLBACK: Gamepad connected, but could not found empty slot");
  }
}

void onDisconnectedGamepad(GamepadPtr gp) {
  bool foundGamepad = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myGamepads[i] == gp) {
      Console.printf("CALLBACK: Gamepad is disconnected from index=%d\n", i);
      myGamepads[i] = nullptr;
      foundGamepad = true;
      break;
    }
  }

  if (!foundGamepad) {
    Console.println(
        "CALLBACK: Gamepad disconnected, but not found in myGamepads");
  }
}

namespace {

Direction dPadDirection(GamepadPtr gamePad) {
  if (gamePad && gamePad->isConnected()) {
    if (gamePad->dpad() & DPAD_UP || gamePad->x()) {
      return Direction::UP;
    }
    if (gamePad->dpad() & DPAD_DOWN || gamePad->b()) {
      return Direction::DOWN;
    }

    if (gamePad->dpad() & DPAD_LEFT || gamePad->y()) {
      return Direction::LEFT;
    }
    if (gamePad->dpad() & DPAD_RIGHT || gamePad->a()) {
      return Direction::RIGHT;
    }
  }

  return Direction::NONE;
}

}  // namespace

/*******************************************************************************/

// ****** Arduino setup function. Will run on CPU 1
void setup() {
  Console.printf("Firmware: %s\n", BP32.firmwareVersion());

  // Setup the Bluepad32 callbacks
  BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);

  // "forgetBluetoothKeys()" should be called when the user performs
  // a "device factory reset", or similar.
  // Calling "forgetBluetoothKeys" in setup() just as an example.
  // Forgetting Bluetooth keys prevents "paired" gamepads to reconnect.
  // But might also fix some connection / re-connection issues.
  BP32.forgetBluetoothKeys();

  xTaskCreatePinnedToCore(dispRefresh, /* Function to implement the task */
                          "dispRefreshTask",  /* Name of the task */
                          10000,              /* Stack size in words */
                          NULL,               /* Task input parameter */
                          10,                 /* Priority of the task */
                          &dispRefreshHandle, /* Task handle. */
                          1); /* Core where the task should run */

  games[gameIdx]->init();
}

// ****** Arduino loop function. Will run on CPU 1
void loop() {
  // The gamepads pointer (the ones received in the callbacks) gets updated
  // automatically.
  BP32.update();

  GamepadPtr gamePadLeft = myGamepads[0];
  GamepadPtr gamePadRight = myGamepads[1];

  // change game ?
  if (gamePadLeft && gamePadLeft->isConnected()) {
    // at least 3 secs since last game change
    if (gamePadLeft->miscSystem() && (time(nullptr) > (lastGameChange + 3))) {
      gameIdx = (gameIdx + 1) % games.size();
      games[gameIdx]->init();

      lastGameChange = time(nullptr);
    }
  }

  games[gameIdx]->tick(dPadDirection(gamePadLeft), dPadDirection(gamePadRight),
                       gamePadLeft, gamePadRight);

  delay(100);
}
