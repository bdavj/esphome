import esphome.codegen as cg
from esphome.components import text_sensor, uart
import esphome.config_validation as cv
from esphome.const import CONF_ID

# Honeywell Galaxy 7 Keypad - bdavj

DEPENDENCIES = ["uart"]

galaxy_ns = cg.esphome_ns.namespace("honeywell_galaxy7_keypad")
HoneywellGalaxy7Keypad = galaxy_ns.class_(
    "HoneywellGalaxy7Keypad", uart.UARTDevice, cg.Component
)

CONF_RS485_RX_ID = "rs485_rx_id"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(HoneywellGalaxy7Keypad),
            cv.Optional(CONF_RS485_RX_ID): cv.use_id(text_sensor.TextSensor),
        }
    )
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

    rx = config.get(CONF_RS485_RX_ID)
    if rx is not None:
        sens = await cg.get_variable(rx)
        cg.add(var.set_rx_text_sensor(sens))

    # ❌ NO cg.add_api_service HERE
    # The API service itself is declared in YAML and calls var.api_write_rs485()
