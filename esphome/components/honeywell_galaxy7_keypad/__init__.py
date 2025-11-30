import esphome.codegen as cg
from esphome.components import uart
import esphome.config_validation as cv
from esphome.const import CONF_ID

DEPENDENCIES = ["uart"]

galaxy_ns = cg.esphome_ns.namespace("honeywell_galaxy7_keypad")
HoneywellGalaxy7Keypad = galaxy_ns.class_(
    "HoneywellGalaxy7Keypad", uart.UARTDevice, cg.Component
)

CONFIG_SCHEMA = (
    cv.Schema({cv.GenerateID(): cv.declare_id(HoneywellGalaxy7Keypad)})
    .extend(uart.UART_DEVICE_SCHEMA)
    .extend(cv.COMPONENT_SCHEMA)
)

FINAL_VALIDATE_SCHEMA = uart.final_validate_device_schema(
    "honeywell_galaxy7_keypad", require_tx=True, require_rx=True
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
