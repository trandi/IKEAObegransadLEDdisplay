# IKEA OBEGRÄNSAD LED wall lamp game

- controlled by a [TTGO T-Display](https://github.com/Xinyuan-LilyGO/TTGO-T-Display) ESP32 board
- using a Nintendo Switch JoyCon controller
  - uses [BluePad32](https://github.com/ricardoquesada/bluepad32) platform as gamepad "host" on the ESP32
    - this supports numerous other GamePads
- project uses [Espressif IoT Development Framework](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html) with several "components" (not in this repository)
  - [Arduino](https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/esp-idf_component.html)
  - BluePad32
  - BluePad32-Aduino
  - btstack
  - cmd_nvs
  - smc_system


To set up:
- install [ESP-IDF v4.4](https://dl.espressif.com/dl/esp-idf/?idf=4.4)
- export tools : `source esp-idf/export.sh`
- add the above **components** into `/components/` dir
- `idf.py fullclean`
- `idf.py build`
- `idf.py -p /dev/ttyUSB0 flash`


## More and ability to see it in action [details on my blog](https://trandi.wordpress.com/2022/12/24/game-on-ikea-obergransad-led-display/)