#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace honeywell_galaxy7_keypad {

class HoneywellGalaxy7Keypad : public uart::UARTDevice, public Component {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
};

}  // namespace honeywell_galaxy7_keypad
}  // namespace esphome
