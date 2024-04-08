#include "7segment_gpio.h"
#include <assert.h>

namespace esphome {
namespace lcd_digits {

namespace {
constexpr uint8_t UNKNOWN_CHAR = 0xff;
/**
 * @brief symbol map
 *
 */
constexpr uint8_t ASCII_TO_RAW[95] = {
    0b00000000,   // ' ', ord 0x20
    0b10110000,   // '!', ord 0x21
    0b00100010,   // '"', ord 0x22
    UNKNOWN_CHAR, // '#', ord 0x23
    UNKNOWN_CHAR, // '$', ord 0x24
    0b01001001,   // '%', ord 0x25
    UNKNOWN_CHAR, // '&', ord 0x26
    0b00000010,   // ''', ord 0x27
    0b01001110,   // '(', ord 0x28
    0b01111000,   // ')', ord 0x29
    0b01000000,   // '*', ord 0x2A
    UNKNOWN_CHAR, // '+', ord 0x2B
    0b00010000,   // ',', ord 0x2C
    0b00000001,   // '-', ord 0x2D
    0b10000000,   // '.', ord 0x2E
    UNKNOWN_CHAR, // '/', ord 0x2F
    0b01111110,   // '0', ord 0x30
    0b00110000,   // '1', ord 0x31
    0b01101101,   // '2', ord 0x32
    0b01111001,   // '3', ord 0x33
    0b00110011,   // '4', ord 0x34
    0b01011011,   // '5', ord 0x35
    0b01011111,   // '6', ord 0x36
    0b01110000,   // '7', ord 0x37
    0b01111111,   // '8', ord 0x38
    0b01111011,   // '9', ord 0x39
    0b01001000,   // ':', ord 0x3A
    0b01011000,   // ';', ord 0x3B
    0b01000011,   // '<', ord 0x3C
    0b00001001,   // '=', ord 0x3D
    0b01100001,   // '>', ord 0x3E
    0b01100101,   // '?', ord 0x3F
    0b01101111,   // '@', ord 0x40
    0b01110111,   // 'A', ord 0x41
    0b01111111,   // 'B', ord 0x42
    0b01001110,   // 'C', ord 0x43
    0b01111001,   // 'D', ord 0x44
    0b01001111,   // 'E', ord 0x45
    0b01000111,   // 'F', ord 0x46
    0b01011110,   // 'G', ord 0x47
    0b00110111,   // 'H', ord 0x48
    0b00000110,   // 'I', ord 0x49
    0b00111000,   // 'J', ord 0x4A
    0b01010111,   // 'K', ord 0x4B
    0b00001110,   // 'L', ord 0x4C
    0b01101011,   // 'M', ord 0x4D
    0b01110110,   // 'N', ord 0x4E
    0b01111110,   // 'O', ord 0x4F
    0b01100111,   // 'P', ord 0x50
    0b01101011,   // 'Q', ord 0x51
    0b01101111,   // 'R', ord 0x52
    0b01011011,   // 'S', ord 0x53
    0b01000110,   // 'T', ord 0x54
    0b00111110,   // 'U', ord 0x55
    0b00111010,   // 'V', ord 0x56
    0b01011100,   // 'W', ord 0x57
    0b01001001,   // 'X', ord 0x58
    0b00101011,   // 'Y', ord 0x59
    0b01101101,   // 'Z', ord 0x5A
    0b01001110,   // '[', ord 0x5B
    UNKNOWN_CHAR, // '\', ord 0x5C
    0b01111000,   // ']', ord 0x5D
    UNKNOWN_CHAR, // '^', ord 0x5E
    0b00001000,   // '_', ord 0x5F
    0b00100000,   // '`', ord 0x60
    0b00011001,   // 'a', ord 0x61
    0b00011111,   // 'b', ord 0x62
    0b00001101,   // 'c', ord 0x63
    0b00111101,   // 'd', ord 0x64
    0b00001100,   // 'e', ord 0x65
    0b00000111,   // 'f', ord 0x66
    0b01001101,   // 'g', ord 0x67
    0b00010111,   // 'h', ord 0x68
    0b01001100,   // 'i', ord 0x69
    0b01011000,   // 'j', ord 0x6A
    0b01011000,   // 'k', ord 0x6B
    0b00000110,   // 'l', ord 0x6C
    0b01010101,   // 'm', ord 0x6D
    0b00010101,   // 'n', ord 0x6E
    0b00011101,   // 'o', ord 0x6F
    0b01100111,   // 'p', ord 0x70
    0b01110011,   // 'q', ord 0x71
    0b00000101,   // 'r', ord 0x72
    0b00011000,   // 's', ord 0x73
    0b00001111,   // 't', ord 0x74
    0b00011100,   // 'u', ord 0x75
    0b00011000,   // 'v', ord 0x76
    0b00101010,   // 'w', ord 0x77
    0b00001001,   // 'x', ord 0x78
    0b00111011,   // 'y', ord 0x79
    0b00001001,   // 'z', ord 0x7A
    0b00110001,   // '{', ord 0x7B
    0b00000110,   // '|', ord 0x7C
    0b00000111,   // '}', ord 0x7D
    0b01100011,   // '~', ord 0x7E (degree symbol)
};

/// Starts from 0x0410
constexpr uint8_t CYRILLIC_TO_RAW[] = {
    0b01110111,   // `А` 0x0410
    0b01011111,   // `Б` 0x0411
    0b01111111,   // `В` 0x0412
    0b01000110,   // `Г` 0x0413
    UNKNOWN_CHAR, // `Д` 0x0414
    0b01001111,   // `Е` 0x0415
    UNKNOWN_CHAR, // `Ж` 0x0416
    0b01111001,   // `З` 0x0417
    UNKNOWN_CHAR, // `И` 0x0418
    UNKNOWN_CHAR, // `Й` 0x0419
    UNKNOWN_CHAR, // `К` 0x041a
    UNKNOWN_CHAR, // `Л` 0x041b
    UNKNOWN_CHAR, // `М` 0x041c
    0b00110111,   // `Н` 0x041d
    0b01111110,   // `О` 0x041e
    0b01110110,   // `П` 0x041f
    0b01100111,   // `Р` 0x0420
    0b01001110,   // `С` 0x0421
    UNKNOWN_CHAR, // `Т` 0x0422
    0b00111011,   // `У` 0x0423
    UNKNOWN_CHAR, // `Ф` 0x0424
    UNKNOWN_CHAR, // `Х` 0x0425
    UNKNOWN_CHAR, // `Ц` 0x0426
    0b00110011,   // `Ч` 0x0427
    UNKNOWN_CHAR, // `Ш` 0x0428
    UNKNOWN_CHAR, // `Щ` 0x0429
    UNKNOWN_CHAR, // `Ъ` 0x042a
    UNKNOWN_CHAR, // `Ы` 0x042b
    0b00011111,   // `Ь` 0x042c
    0b01111001,   // `Э` 0x042d
    UNKNOWN_CHAR, // `Ю` 0x042e
    UNKNOWN_CHAR, // `Я` 0x042f

    UNKNOWN_CHAR, // `а` 0x0430
    UNKNOWN_CHAR, // `б` 0x0431
    UNKNOWN_CHAR, // `в` 0x0432
    0b00000011,   // `г` 0x0433
    0b01111101,   // `д` 0x0434
    UNKNOWN_CHAR, // `е` 0x0435
    UNKNOWN_CHAR, // `ж` 0x0436
    UNKNOWN_CHAR, // `з` 0x0437
    0b00011100,   // `и` 0x0438
    0b01011100,   // `й` 0x0439
    UNKNOWN_CHAR, // `к` 0x043a
    UNKNOWN_CHAR, // `л` 0x043b
    UNKNOWN_CHAR, // `м` 0x043c
    UNKNOWN_CHAR, // `н` 0x043d
    UNKNOWN_CHAR, // `о` 0x043e
    UNKNOWN_CHAR, // `п` 0x043f
    UNKNOWN_CHAR, // `р` 0x0440
    UNKNOWN_CHAR, // `с` 0x0441
    UNKNOWN_CHAR, // `т` 0x0442
    UNKNOWN_CHAR, // `у` 0x0443
    UNKNOWN_CHAR, // `ф` 0x0444
    UNKNOWN_CHAR, // `х` 0x0445
    UNKNOWN_CHAR, // `ц` 0x0446
    UNKNOWN_CHAR, // `ч` 0x0447
    UNKNOWN_CHAR, // `ш` 0x0448
    UNKNOWN_CHAR, // `щ` 0x0449
    UNKNOWN_CHAR, // `ъ` 0x044a
    UNKNOWN_CHAR, // `ы` 0x044b
    UNKNOWN_CHAR, // `ь` 0x044c
    UNKNOWN_CHAR, // `э` 0x044d
    UNKNOWN_CHAR, // `ю` 0x044e
    UNKNOWN_CHAR, // `я` 0x044f

    // UNKNOWN_CHAR, // `Ё` 0x0401
    // UNKNOWN_CHAR, // `ё` 0x0451
};

LcdDigitsData *g_interrupt_data = nullptr;
static void IRAM_ATTR HOT s_timer_intr() {
  g_interrupt_data->timer_interrupt();
}
} // namespace

void IRAM_ATTR HOT LcdDigitsData::timer_interrupt() {
  if (cycles_to_skip > 0) {
    cycles_to_skip--;
    return;
  }

  // run at least with 1kHz
  const uint32_t min_dt_us = 1000;
  const uint32_t now = micros();

  uint8_t bit_count = 0;
  if (iterate_digits) {

    // turn off digit
    digit_pins[current_frame]->digital_write(true);

    // switch to hext digit
    current_frame = (current_frame + 1) % digit_pins.size();

    auto raw_digit = buffer_[current_frame];
    for (const auto &segment_pin : segment_pins) {
      const bool segment_on = raw_digit & 0x01;
      raw_digit >>= 1;
      bit_count += segment_on ? 1 : 0;
      segment_pin->digital_write(segment_on);
    }

    //
    digit_pins[current_frame]->digital_write(false);
  } else {
    segment_pins[current_frame]->digital_write(false);

    // switch to hext segment
    current_frame = (current_frame + 1) % segment_pins.size();

    auto const *raw_digit = buffer_;
    for (const auto &digit_pin : digit_pins) {
      const bool digit_on = (*raw_digit) & (0x01 << current_frame);
      bit_count += digit_on ? 1 : 0;
      digit_pin->digital_write(!digit_on);
      raw_digit++;
    }
    segment_pins[current_frame]->digital_write(true);
  }

  if (colon_pin)
    colon_pin->digital_write(colon_on && current_frame == 0);

  if (degree_pin)
    degree_pin->digital_write(degree_on && current_frame == 0);

  cycles_to_skip = intensity_delay + (compensate_brightness ? bit_count : 0);
}

void LcdDigitsComponent::set_segment_pins(std::vector<GPIOPin *> segment_pins) {
  ESP_LOGV(TAG, "Setting up segment pins");
  assert(timer == nullptr);
  interrupt_data_.segment_pins = std::move(segment_pins);
  interrupt_data_.segment_pins.resize(
      std::min(size_t(8), interrupt_data_.segment_pins.size()));
}

void LcdDigitsComponent::set_degree_pin(GPIOPin *arg) {
  ESP_LOGV(TAG, "Setting up degree pin");
  assert(timer == nullptr);
  interrupt_data_.degree_pin = arg;
}

void LcdDigitsComponent::set_colon_pin(GPIOPin *arg) {
  ESP_LOGV(TAG, "Setting up colon");
  assert(timer == nullptr);
  interrupt_data_.colon_pin = arg;
}

void LcdDigitsComponent::set_digit_pins(std::vector<GPIOPin *> digit_pins) {
  ESP_LOGV(TAG, "Setting up digit pins");
  assert(timer == nullptr);
  interrupt_data_.digit_pins = std::move(digit_pins);
  interrupt_data_.digit_pins.resize(
      std::min(size_t(max_digit_count), interrupt_data_.digit_pins.size()));
}

void LcdDigitsComponent::set_writer(lcd_digits_writer_t &&writer) {
  ESP_LOGV(TAG, "Setting up writer");
  assert(timer == nullptr);
  writer_ = std::move(writer);
}
void LcdDigitsComponent::set_compensate_brightness(bool arg) {
  ESP_LOGV(TAG, "Setting up brightness to %d", arg);
  InterruptLock lock;
  interrupt_data_.compensate_brightness = arg;
}
void LcdDigitsComponent::set_iterate_digits(bool arg) {
  ESP_LOGV(TAG, "Setting up iterate digits to %d", arg);
  InterruptLock lock;
  interrupt_data_.iterate_digits = arg;
}
void LcdDigitsComponent::set_intensity(uint8_t arg) {
  ESP_LOGV(TAG, "Setting up intensity to %d", arg);
  InterruptLock lock;
  interrupt_data_.intensity_delay = (15 - arg);
};

void LcdDigitsComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "LCD Digits:");
  for (auto *pin : interrupt_data_.digit_pins) {
    LOG_PIN("  Digit Pin: ", pin);
  }
  for (auto *pin : interrupt_data_.segment_pins) {
    LOG_PIN("  Segment Pin: ", pin);
  }
  LOG_PIN("  Semicolon Pin: ", interrupt_data_.colon_pin);
  LOG_PIN("  Semicolon Pin: ", interrupt_data_.degree_pin);
}

void LcdDigitsComponent::update() {
  if (writer_.has_value())
    (*writer_)(*this);
  ESP_LOGV(TAG, "Updating interrupt data");
  InterruptLock lock;
  static_cast<LcdData &>(interrupt_data_) = display_data_;
}

uint8_t LcdDigitsComponent::print(const char *str) {
  ESP_LOGV(TAG, "Printing %s", str);
  return print(0, str);
}

void LcdDigitsComponent::set_degree_on(bool arg) {
  ESP_LOGV(TAG, "Setting degree on %d", arg);
  display_data_.degree_on = arg;
}
void LcdDigitsComponent::set_colon_on(bool arg) {
  ESP_LOGV(TAG, "Setting colon on %d", arg);
  display_data_.colon_on = arg;
}

void LcdDigitsComponent::set_mode(LcdDigitsComponent::Mode mode) {
  if (mode_ == mode)
    return;

  switch (mode) {
  case BufferMode:
    timerAlarmEnable(timer);
    break;
  case ProgressMode:
    timerAlarmDisable(timer);
    break;
  }
  mode_ = mode;
}

void LcdDigitsComponent::set_progress(float progress) {
  set_mode(ProgressMode);

  const uint8_t total_digits = interrupt_data_.digit_pins.size();
  const uint8_t total_steps = 6*total_digits;
  const uint8_t current_step = total_steps*progress;
  const uint8_t current_digit = current_step/6;
  const uint8_t current_segment = 1 + current_step%6;
  
  // off all digits
  for(auto* pin: interrupt_data_.digit_pins)
    pin->digital_write(true);
  // off all segments
  for(auto* pin: interrupt_data_.segment_pins)
    pin->digital_write(false);
  interrupt_data_.segment_pins[current_segment]->digital_write(true);
  interrupt_data_.digit_pins[current_digit]->digital_write(false);
}

void LcdDigitsComponent::strftime(uint8_t pos, const char *format,
                                  ESPTime time) {
  ESP_LOGV(TAG, "Setting strftme pos: %ud, format: %s", pos, format);
  char buffer[8];
  size_t ret = time.strftime(buffer, sizeof(buffer), format);
  if (ret > 0)
    print(pos, buffer);
}

void LcdDigitsComponent::setup() {
  ESP_LOGV(TAG, "Setting up");
  g_interrupt_data = &interrupt_data_;

  const auto setup_output_pin = [&](GPIOPin *pin, bool initial) {
    pin->setup();
    pin->pin_mode(esphome::gpio::Flags::FLAG_OUTPUT);
    pin->digital_write(initial);
  };

  for (const auto &pin : interrupt_data_.digit_pins)
    setup_output_pin(pin, true);

  for (const auto &pin : interrupt_data_.segment_pins)
    setup_output_pin(pin, false);

  if (interrupt_data_.colon_pin)
    setup_output_pin(interrupt_data_.colon_pin, false);

  if (interrupt_data_.degree_pin)
    setup_output_pin(interrupt_data_.degree_pin, false);

  // see https://esphome.io/api/ac__dimmer_8cpp_source
  assert(timer == nullptr);
  timer = timerBegin(0, 80, true);
  if (timer) {
    timerAttachInterrupt(timer, &s_timer_intr, true);
    // For ESP32, we can't use dynamic interval calculation because the timerX
    // functions are not callable from ISR (placed in flash storage). Here we
    // just use an interrupt firing every 50 µs.
    timerAlarmWrite(timer, 50, true);
    timerAlarmEnable(timer);
  } else {
    ESP_LOGE(TAG, "Can't initialize timer");
  }
}

uint8_t LcdDigitsComponent::print(uint8_t start_pos, const char *in_str) {
  ESP_LOGV(TAG, "Printing pos: %d %s", start_pos, in_str);
  const auto &digits_count = interrupt_data_.digit_pins.size();
  if (start_pos >= digits_count)
    return 0;

  uint8_t pos = start_pos;
  auto &&buffer = display_data_.buffer_;
  for (auto str = in_str; *str != '\0'; str++) {
    uint8_t data = UNKNOWN_CHAR;
    if (*str >= ' ' && *str <= '~')
      data = ASCII_TO_RAW[*str - ' '];

    if (data == UNKNOWN_CHAR) {
      ESP_LOGW(TAG,
               "Encountered character '%c' with no representation while "
               "translating string!",
               *str);
    }

    if (*str == '.') {
      if (pos != start_pos)
        pos--;
      buffer[digits_count - pos - 1] |= 0b10000000;
    } else {
      if (pos > digits_count) {
        ESP_LOGE(TAG, "String '%s' is too long for the display!", in_str);
        break;
      }
      buffer[digits_count - pos - 1] = data;
    }
    pos++;
  }

  return pos - start_pos;
}

uint8_t LcdDigitsComponent::printf(uint8_t pos, const char *format, ...) {
  ESP_LOGV(TAG, "printf pos: %d %s", pos, format);
  va_list arg;
  va_start(arg, format);
  char buffer[8];
  int ret = vsnprintf(buffer, sizeof(buffer), format, arg);
  va_end(arg);
  if (ret > 0)
    return print(pos, buffer);
  return 0;
}

void LcdDigitsComponent::set_raw(uint64_t raw) {
  ESP_LOGV(TAG, "Setting raw %ud", raw);
  for (auto &d : display_data_.buffer_) {
    d = raw & 0xff;
    raw >>= 8;
  }
}

} // namespace lcd_digits

} // namespace esphome
