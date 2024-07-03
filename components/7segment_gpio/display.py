import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display
from esphome.const import CONF_ID, CONF_INTENSITY, CONF_LAMBDA
from esphome.pins import gpio_output_pin_schema
from esphome.const import __version__ as ESPHOME_VERSION

lcd_digits_ns = cg.esphome_ns.namespace("lcd_digits")
LcdDigitsComponent = lcd_digits_ns.class_(
    "LcdDigitsComponent", cg.PollingComponent
)
LcdDigitsComponentRef = LcdDigitsComponent.operator("ref")

# CONF_REVERSE_ENABLE = "reverse_enable"
CONF_DIGIT_PINS = "digit_pins"
CONF_SEGMENT_PINS = "segment_pins"
CONF_ISCOMMONVCC = "common_positive"
CONF_ITERATE_DIGITS = "iterate_digits"
CONF_COMPENSATE_BRIGHTNESS = "compensate_brightness"
CONF_COLON_PIN = "colon_pin"
CONF_DEGREE_PIN = "degree_pin"

CONFIG_SCHEMA = (
    display.BASIC_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(LcdDigitsComponent),
            cv.Optional(CONF_DIGIT_PINS): cv.ensure_list(
                            gpio_output_pin_schema
                        ),
            cv.Required(CONF_SEGMENT_PINS): cv.ensure_list(
                            gpio_output_pin_schema
                        ),
            cv.Optional(CONF_INTENSITY, default=15): cv.int_range(min=0, max=15),
            cv.Optional(CONF_ISCOMMONVCC, default=False): cv.boolean,
            cv.Optional(CONF_ITERATE_DIGITS, default=True): cv.boolean,
            cv.Optional(CONF_COMPENSATE_BRIGHTNESS, default=False): cv.boolean,
            cv.Optional(CONF_INTENSITY, default=15): cv.int_range(min=0, max=15),
            cv.Optional(CONF_COLON_PIN): gpio_output_pin_schema,
            cv.Optional(CONF_DEGREE_PIN): gpio_output_pin_schema,
        }
    )
    .extend(cv.polling_component_schema("1s"))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    if cv.Version.parse(ESPHOME_VERSION) < cv.Version.parse("2023.12.0"):
        await cg.register_component(var, config)
    await display.register_display(var, config)

    cg.add(var.set_intensity(config[CONF_INTENSITY]))
    # cg.add(var.set_reverse(config[CONF_REVERSE_ENABLE]))

    if CONF_DIGIT_PINS in config:
        digit_pins = []
        for pin_config in config[CONF_DIGIT_PINS]:
            digit_pins.append(await cg.gpio_pin_expression(pin_config))
        cg.add(var.set_digit_pins(digit_pins))

    segment_pins = []
    for pin_config in config[CONF_SEGMENT_PINS]:
        segment_pins.append(await cg.gpio_pin_expression(pin_config))
    cg.add(var.set_segment_pins(segment_pins))
    cg.add(var.set_common_vcc(config[CONF_ISCOMMONVCC]))
    cg.add(var.set_iterate_digits(config[CONF_ITERATE_DIGITS]))
    cg.add(var.set_compensate_brightness(config[CONF_COMPENSATE_BRIGHTNESS]))
    cg.add(var.set_intensity(config[CONF_INTENSITY]))

    if CONF_COLON_PIN in config:
        cg.add(var.set_colon_pin(await cg.gpio_pin_expression(config[CONF_COLON_PIN])));
    
    if CONF_DEGREE_PIN in config:
        cg.add(var.set_degree_pin(await cg.gpio_pin_expression(config[CONF_DEGREE_PIN])));

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(LcdDigitsComponentRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
