#include "honeywell_galaxy7_keypad.h"
#include "esphome/core/log.h"

namespace esphome {
namespace honeywell_galaxy7_keypad {

static const char *TAG = "honeywell_galaxy7_keypad.component";
static const uint8_t poll[] = {0x10, 0x00, 0x0E, 0xC8};

void HoneywellGalaxy7Keypad::setup() {
  ESP_LOGI(TAG, "Honeywell Galaxy keypad setup starting");

  this->write_array(poll, sizeof(poll));

  // Init handshake can be made real later – for now we just blast a byte
}

void HoneywellGalaxy7Keypad::loop() {
  // ---- TX: send poll every 400ms ----
  static uint32_t last = 0;
  uint32_t now = millis();

  if (now - last > 400) {  // every 400ms
    last = now;

    const uint8_t poll[] = {0x10, 0x00, 0x0E, 0xC8};
    ESP_LOGI(TAG, "Sending poll to keypad");
    this->write_array(poll, sizeof(poll));
  }

  // ---- RX: drain anything the keypad replied with ----
  while (this->available()) {
    uint8_t b;
    this->read_byte(&b);

    // For now just dump hex to logs or your sensor
    // ESP_LOGI(TAG, "RX: 0x%02X", b);

    if (this->rx_sens_) {
      static std::string rxbuf;

      if (b >= 0x20 && b <= 0x7E) {
        rxbuf.push_back((char) b);
      } else {
        char buf[5];
        sprintf(buf, "\\x%02X", b);
        rxbuf += buf;
      }

      this->rx_sens_->publish_state(rxbuf);
    }
  }
}

// uint8_t b;
// this->read_byte(&b);

// if (this->rx_sens_) {
//   static std::string rxbuf;

//   if (b >= 0x20 && b <= 0x7E) {
//     rxbuf.push_back((char) b);
//   } else {
//     char buf[5];
//     sprintf(buf, "\\x%02X", b);
//     rxbuf += buf;
//   }

//   // publish quickly (can be throttled later)
//   this->rx_sens_->publish_state(rxbuf);
// }

void HoneywellGalaxy7Keypad::api_write_rs485(const std::string &data) {
  // Add newline so it shows nice on USB dongle / terminal
  std::string out = data;
  out += "\n";

  this->write_array(reinterpret_cast<const uint8_t *>(out.data()), out.size());
  ESP_LOGI(TAG, "Wrote via API: %s", data.c_str());
}

void HoneywellGalaxy7Keypad::dump_config() { ESP_LOGCONFIG(TAG, "Honeywell Galaxy 7 Keypad"); }

}  // namespace honeywell_galaxy7_keypad
}  // namespace esphome
