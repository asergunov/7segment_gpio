# 7segment_gpio
7-segment GPIO display platform for esphome.

This component let you connect 7-segment display directly to controller using just some resistors.

# Futures
## 1 to 8 digits support
Just list as many `digit_pins` as you have.

## Resistors on any segment or digit side
Usually driver is turning on whole digit one by one so you need resistors for each segment pin to have the same brightness. So 8 of them.

But if you have 4 digits you can put resistors on the digit side. To achieve the same brightness we have to turn on not whole digit, but the same segments on all digits at the time. To switch to this mode just set `iterate_digits` to `false`.

## Degree mark
Some displays have degree mark right before the last digit. So you can have `22.4°C` or `72.3°F` reading. Use `degree_pin` and `set_degree_on(bool)` function. Pin becomes `HIGH` when turned on. Just invert pin if you like to have it `LOW` when on.

## Colon for clock
If display has `:` mark use `colon_pin` and `set_colon_on(bool)` function. Pin becomes `HIGH` when turned on. Just invert pin if you like to have it `LOW` when on.

## Common Anode or Cathode
Common Anode means segment of a digit turning on by `HIGH` signal on digit and `LOW` on the segment. Common cathode one is reverse.

This code has been written for common Anode display.

Once your display is common Cathode display you can invert all the pins.

# Usage Examples

## Common Anode display 4 digits
This example of wiring 4 digit 7-segment common anode display to [S2 mini](https://www.wemos.cc/en/latest/s2/s2_mini.html). 
### Wiring
* Display segment pins [`g`, `f`, `e`, `d`, `c`, `b`, `a`, `.`] connected directly to pins [21, 40, 14, 10, 36, 17, 34, 8]
* Display digit pins connected via 220Ohm resistors to pins [1, 2, 6, 4]
* Display colon anode connected to pin 38
* Display colon cathode connected to ground vai 200Ohm resistor
* Display degree anode connected to pin 13
* Display degree cathode connected to ground via 220Ohm resistor

```yaml
external_components:
  # shorthand
  source: github://asergunov/7segement_gpio

time:
- platform: sntp
  id: sntp_time

display:
- platform: 7segment_gpio
  id: lcd
  digit_pins: [1,2,6,4]
  iterate_digits: false
  segment_pins:
    - 21 # g
    - 40 # f
    - 14 # e
    - 10 # d
    - 36 # c
    - 17 # b
    - 34 # a
    - 8  # .
  colon_pin: 38
  degree_pin: 13 # degree mark on 
  update_interval: 1s
  lambda: |-
      switch((millis()/3000) % 3) {
      case 0:
        it.printf(0, "%4.1fC", 22.4);
        it.set_degree_on(true);
        it.set_colon_on(false);
        break;
      case 1:
        it.printf(0, "%4.1fF", 72,3);
        it.set_degree_on(true);
        it.set_colon_on(false);
        break;
      case 2:
        it.strftime(0, "%H%M", id(sntp_time).now());
        it.set_degree_on(false);
        it.set_colon_on((millis()/500) % 2);
        break;
      default:
        it.print("Unexpected to be here");
        break;
      }
```