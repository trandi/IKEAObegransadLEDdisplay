#include "sdkconfig.h"
#ifndef CONFIG_BLUEPAD32_PLATFORM_ARDUINO
#error "Must only be compiled when using Bluepad32 Arduino platform"
#endif  // !CONFIG_BLUEPAD32_PLATFORM_ARDUINO

#include <Arduino.h>
#include <Bluepad32.h>

#include <memory>

/********************/
#include "Display.h"

struct Ball {
  explicit Ball(std::shared_ptr<Display> disp) : display(std::move(disp)) {
    display->setPixel(x, y, colour);
  }

  void move(int deltaX, int deltaY) {
    display->setPixel(x, y, 0);
    x = constrain(x + deltaX, 0, Display::COLS - 1);
    y = constrain(y + deltaY, 0, Display::ROWS - 1);
    display->setPixel(x, y, colour);
  }

  void darken() {
    colour = min(colour + 1, Display::MAX_GREY_LEVEL);
    display->setPixel(x, y, colour);
  }

  void lighten() {
    colour = max(colour - 1, 0);
    display->setPixel(x, y, colour);
  }

 private:
  int x{0};
  int y{0};
  int colour{Display::MAX_GREY_LEVEL};
  std::shared_ptr<Display> display;
};

GamepadPtr myGamepads[BP32_MAX_GAMEPADS];
auto display{std::make_shared<Display>()};
TaskHandle_t dispRefreshHandle;
Ball ball(display);
int BUTTON_A_MASK{0x01};
int BUTTON_B_MASK{0x04};

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
}

// ****** Arduino loop function. Will run on CPU 1
void loop() {
  // This call fetches all the gamepad info from the NINA (ESP32) module.
  // Just call this function in your main loop.
  // The gamepads pointer (the ones received in the callbacks) gets updated
  // automatically.
  BP32.update();

  GamepadPtr myGamepad = myGamepads[0];
  if (myGamepad && myGamepad->isConnected()) {
    auto x = myGamepad->axisX();
    auto y = myGamepad->axisY();

    auto deltaX = x > 100 ? -1 : (x < -100 ? 1 : 0);
    auto deltaY = y > 100 ? 1 : (y < -100 ? -1 : 0);

    if (deltaX != 0 || deltaY != 0) {
      Console.printf("~~~~~ MOVE: (%d, %d)\n", deltaX, deltaY);
      ball.move(deltaX, deltaY);
    }

    if (myGamepad->buttons() & BUTTON_A_MASK) {
      ball.darken();
    }
    if (myGamepad->buttons() & BUTTON_B_MASK) {
      ball.lighten();
    }
  }

  // // It is safe to always do this before using the gamepad API.
  // // This guarantees that the gamepad is valid and connected.
  // for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
  //     GamepadPtr myGamepad = myGamepads[i];

  //     if (myGamepad && myGamepad->isConnected()) {
  //         // There are different ways to query whether a button is pressed.
  //         // By query each button individually:
  //         //  a(), b(), x(), y(), l1(), etc...
  //         if (myGamepad->a()) {
  //             static int colorIdx = 0;
  //             // Some gamepads like DS4 and DualSense support changing the
  //             color LED.
  //             // It is possible to change it by calling:
  //             switch (colorIdx % 3) {
  //                 case 0:
  //                     // Red
  //                     myGamepad->setColorLED(255, 0, 0);
  //                     break;
  //                 case 1:
  //                     // Green
  //                     myGamepad->setColorLED(0, 255, 0);
  //                     break;
  //                 case 2:
  //                     // Blue
  //                     myGamepad->setColorLED(0, 0, 255);
  //                     break;
  //             }
  //             colorIdx++;
  //         }

  //         if (myGamepad->b()) {
  //             // Turn on the 4 LED. Each bit represents one LED.
  //             static int led = 0;
  //             led++;
  //             // Some gamepads like the DS3, DualSense, Nintendo Wii,
  //             Nintendo Switch
  //             // support changing the "Player LEDs": those 4 LEDs that
  //             usually indicate
  //             // the "gamepad seat".
  //             // It is possible to change them by calling:
  //             myGamepad->setPlayerLEDs(led & 0x0f);
  //         }

  //         if (myGamepad->x()) {
  //             // Duration: 255 is ~2 seconds
  //             // force: intensity
  //             // Some gamepads like DS3, DS4, DualSense, Switch, Xbox One S
  //             support
  //             // rumble.
  //             // It is possible to set it by calling:
  //             myGamepad->setRumble(0xc0 /* force */, 0xc0 /* duration */);
  //         }

  //         // Another way to query the buttons, is by calling buttons(), or
  //         // miscButtons() which return a bitmask.
  //         // Some gamepads also have DPAD, axis and more.
  //         Console.printf(
  //             "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis
  //             R: %4d, "
  //             "%4d, brake: %4d, throttle: %4d, misc: 0x%02x\n",
  //             i,                        // Gamepad Index
  //             myGamepad->dpad(),        // DPAD
  //             myGamepad->buttons(),     // bitmask of pressed buttons
  //             myGamepad->axisX(),       // (-511 - 512) left X Axis
  //             myGamepad->axisY(),       // (-511 - 512) left Y axis
  //             myGamepad->axisRX(),      // (-511 - 512) right X axis
  //             myGamepad->axisRY(),      // (-511 - 512) right Y axis
  //             myGamepad->brake(),       // (0 - 1023): brake button
  //             myGamepad->throttle(),    // (0 - 1023): throttle (AKA gas)
  //             button myGamepad->miscButtons()  // bitmak of pressed "misc"
  //             buttons
  //         );

  //         // You can query the axis and other properties as well. See
  //         Gamepad.h
  //         // For all the available functions.
  //     }
  // }

  delay(100);
}