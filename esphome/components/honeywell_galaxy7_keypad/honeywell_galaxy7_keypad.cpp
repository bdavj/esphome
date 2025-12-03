#include "honeywell_galaxy7_keypad.h"
#include "esphome/core/log.h"

namespace esphome {
namespace honeywell_galaxy7_keypad {

static const char *TAG = "honeywell_galaxy7_keypad.component";
static const uint8_t poll[] = {0x10, 0x00, 0x0E, 0xC8};

uint8_t HoneywellGalaxy7Keypad::galaxy_checksum(const std::vector<uint8_t> &data) {
  uint32_t temp = 0xAA;
  for (auto b : data) {
    temp += b;
  }
  return ((temp >> 24) & 0xFF) + ((temp >> 16) & 0xFF) + ((temp >> 8) & 0xFF) + (temp & 0xFF);
}

void HoneywellGalaxy7Keypad::send_frame(const std::vector<uint8_t> &payload) {
  // Copy + append checksum
  std::vector<uint8_t> frame(payload);
  frame.push_back(galaxy_checksum(payload));
  this->write_array(frame.data(), frame.size());
}

void HoneywellGalaxy7Keypad::setup() {
  ESP_LOGI(TAG, "Honeywell Galaxy keypad setup starting");

  this->write_array(poll, sizeof(poll));

  // Init handshake can be made real later – for now we just blast a byte
}

void HoneywellGalaxy7Keypad::loop() {
  static uint32_t last_init_poll = 0;
  static uint32_t last_activity_poll = 0;
  uint32_t now = millis();
  const uint8_t device_id = 0x20;  // or 0x10 / whatever your keypad is set to

  // 1) Occasional "init" / status poll (00)
  if (now - last_init_poll > 1000) {  // every 1s
    last_init_poll = now;
    // Your real panel used: 20 00 0F D9
    this->send_frame({device_id, 0x00, 0x0F});
  }

  // 2) Frequent activity poll (19) to catch keypresses
  if (now - last_activity_poll > 50) {  // ~20 Hz
    last_activity_poll = now;
    // Burton example: 10 19 01 D4 → so {id, 0x19, 0x01}
    this->send_frame({device_id, 0x19, 0x01});
  }

  // ---- RX: grab whatever we got this cycle ----
  std::vector<uint8_t> bytes;
  while (this->available()) {
    uint8_t b;
    this->read_byte(&b);
    bytes.push_back(b);
  }

  if (bytes.empty())
    return;

  // We don't want to spam logs with every poll reply, so keep this very light
  // ESP_LOGD(TAG, "Raw frame len=%d", (int)bytes.size());

  // All keypad → panel frames start with 0x11 in your captures
  if (bytes[0] != 0x11) {
    // Not a keypad frame we care about
    return;
  }

  if (bytes.size() >= 2) {
    uint8_t type = bytes[1];

    // 11 FF 08 00 64 28  → initial/status response to 0x00 poll
    if (type == 0xFF && bytes.size() == 6) {
      // Known-good "I'm alive" frame – ignore for HA
      // ESP_LOGD(TAG, "Status frame (00 poll), ignoring");
      return;
    }

    // 11 FE BA → activity poll (19) with NO key pressed
    if (type == 0xFE && bytes.size() == 3) {
      // No key pressed – also ignore
      // ESP_LOGD(TAG, "Activity no-key frame, ignoring");
      return;
    }

    // 11 F4 <key> <csum> → activity poll (19) WITH a key
    if (type == 0xF4 && bytes.size() == 4) {
      uint8_t key_code = bytes[2];
      uint8_t cs = bytes[3];

      // Optional: verify checksum of [11, F4, key_code] matches cs
      std::vector<uint8_t> chk_bytes = {0x11, 0xF4, key_code};
      uint8_t expected = this->galaxy_checksum(chk_bytes);
      if (expected != cs) {
        ESP_LOGW(TAG, "Key frame bad checksum: got %02X, expected %02X", cs, expected);
        return;
      }

      ESP_LOGI(TAG, "Keypress frame: key_code=0x%02X", key_code);

      // Build short hex string for HA
      char buf[16];
      snprintf(buf, sizeof(buf), "\\x11\\xF4\\x%02X\\x%02X", key_code, cs);

      if (this->rx_sens_ != nullptr) {
        this->rx_sens_->publish_state(buf);
      }

      return;
    }
  }

  // Anything else: ignore by default
  // ESP_LOGD(TAG, "Unhandled frame type, ignoring");
}

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

// uint8_t payload[] = { device_id, cmd, arg };  // whatever bytes, no checksum
// uint8_t cs = galaxy_checksum(payload, sizeof(payload));
// uint8_t frame[sizeof(payload) + 1];
// memcpy(frame, payload, sizeof(payload));
// frame[sizeof(payload)] = cs;
// this->write_array(frame, sizeof(frame));
