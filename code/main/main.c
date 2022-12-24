#include <btstack_port_esp32.h>
#include <btstack_run_loop.h>
#include <hci_dump.h>
#include <hci_dump_embedded_stdout.h>
#include <stddef.h>

#include "sdkconfig.h"
#include "uni_esp32.h"
#include "uni_main.h"

// **** Main entry point, required by ESP-IDF
// Also below BluePad32 functions are in C
int app_main(void) {
  uni_esp32_enable_uart_output(1);

  // Configure BTstack for ESP32 on VHCI Controller
  btstack_init();

  // Init Bluepad32, and keeps running not return.
  uni_main(0, NULL);

  return 0;
}
