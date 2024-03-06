/**
 * @file lcd_digits.h
 * @author Anton Sergunov
 * @brief
 * @version 0.1
 * @date 2023-11-10
 *
 * @copyright Anton Sergunov (c) 2023
 *
 * Based on ac_dimmer for timers and max7219 display code
 */

#pragma once

#include "esphome.h"
#include <functional>

#ifdef USE_ESP32_FRAMEWORK_ARDUINO
#include <esp32-hal-timer.h>
#endif

namespace esphome {
namespace lcd_digits {
class LcdDigitsComponent;

using lcd_digits_writer_t = std::function<void(LcdDigitsComponent &)>;
constexpr uint8_t max_digit_count = 4;

struct LcdData {
  uint8_t buffer_[max_digit_count] = {};
  bool colon_on = false;
  bool degree_on = false;
};

struct LcdDigitsData : LcdData {
  std::vector<GPIOPin *> digit_pins;
  std::vector<GPIOPin *> segment_pins;
  GPIOPin *colon_pin = nullptr;
  GPIOPin *degree_pin = nullptr;
  uint8_t cycles_to_skip = 0;
  uint8_t current_frame = 0;
  /**
   * @brief Increase on time proptional to lighting items
   * Usable if you have resistros on digit pins and swithcing the digits
   */
  bool compensate_brightness = false;

  /**
   * @brief Iterate digits or segments
   *
   * If you have resistors on digits you can iterate segemnts to keep brightness
   * the same
   */
  bool iterate_digits = true;

  uint8_t intensity_delay = 0;

  void IRAM_ATTR HOT timer_interrupt();
};

//   a
// f   b
//   g
// e   c
//   d  .
class LcdDigitsComponent : public PollingComponent {
public:
  enum Mode {
    BufferMode, ProgressMode, DisabledMode
  };

  void set_degree_pin(GPIOPin *arg);
  void set_colon_pin(GPIOPin *arg);
  void set_segment_pins(std::vector<GPIOPin *> segment_pins);
  void set_digit_pins(std::vector<GPIOPin *> digit_pins);
  void set_writer(lcd_digits_writer_t &&writer);
  void set_compensate_brightness(bool arg);
  void set_iterate_digits(bool arg);
  void set_intensity(uint8_t arg);
  
  void setup() override;
  void update() override;
  void dump_config() override;
  
  uint8_t print(uint8_t start_pos, const char *str);
  uint8_t print(const char *str);
  uint8_t printf(uint8_t pos, const char *format, ...);
  /**
   * @brief Set the raw value
   * 
   * \a raw are 8 bytes low byte becomes the right most one.
   * Each byte is in format `0b.abcdefg`
   * @param raw 
   */
  void set_raw(uint64_t raw);
  void strftime(uint8_t pos, const char *format, ESPTime time);
  void set_degree_on(bool arg = true);
  void set_colon_on(bool arg = true);

  void set_mode(Mode mode);
  void set_progress(float progress);

private:
  static constexpr auto TAG = "lcd_digits";
  hw_timer_t *timer = nullptr;
  optional<lcd_digits_writer_t> writer_{};
  LcdDigitsData interrupt_data_;
  LcdData display_data_;
  Mode mode_ = DisabledMode;
};

} // namespace lcd_digits
} // namespace esphome