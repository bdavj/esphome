#include "honeywell_galaxy7_keypad.h"
#include "esphome/core/log.h"

namespace esphome {
namespace honeywell_galaxy7_keypad {

static const char *TAG = "honeywell_galaxy7_keypad.component";

void HoneywellGalaxy7Keypad::setup() {
  // Serial/UART device initialization is typically done here.
  // Note that a number of read/write methods are available in the UARTDevice
  // class. See "uart/uart.h" for details.
  ESP_LOGI(TAG, "Honeywell Galaxy keypad setup starting");

  uint8_t initialize_cmd = 0x12;  // Example command to initialize the device
  this->write_byte(initialize_cmd);

  uint8_t response;
  /*   if (!this->read_byte(&response)) {
      ESP_LOGI(TAG, "Nothing on the bus!");
      this->mark_failed();  // Mark the component as failed if communication fails
      return;
    }
    if (response != 0) {  // Example check for a specific response
      ESP_LOGE(TAG, "Initialization failed; response: %d", response);
      this->mark_failed();  // Mark the component as failed if the response is not
                            // as expected
      return;
    } */
}

void HoneywellGalaxy7Keypad::loop() {
  static uint32_t last_log = 0;
  uint32_t now = millis();

  if (now - last_log > 5000) {
    last_log = now;

    ESP_LOGI(TAG, "Sending BEN test frame, millis=%u", now);

    // Create a string "BEN 123456"
    char msg[32];
    snprintf(msg, sizeof(msg), "BEN %u\n", now);

    // Write string to RS485 UART
    this->write_array((const uint8_t *) msg, strlen(msg));
  }
}

void HoneywellGalaxy7Keypad::dump_config() { ESP_LOGCONFIG(TAG, "Honeywell Galaxy 7 Keypad"); }

}  // namespace honeywell_galaxy7_keypad
}  // namespace esphome
