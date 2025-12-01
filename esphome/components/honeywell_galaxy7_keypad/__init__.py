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

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(HoneywellGalaxy7Keypad),
            cv.Optional("rs485_rx_id"): cv.use_id(text_sensor.TextSensor),
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

    # If you want an RX text_sensor, make sure YAML sets rs485_rx_id
    rx = config.get("rs485_rx_id")
    if rx is not None:
        sens = yield cg.get_variable(rx)
        cg.add(var.set_rx_text_sensor(sens))

    # Register API service
    # svc = cg.RawExpression("[] (std::string data) { /* placeholder */ }")
    cg.add_api_service("write_rs485", {"data": "string"}, var.api_write_rs485)
