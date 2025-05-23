#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <cstdint>
struct Controller {
  union Input {
    uint8_t reg;
    struct {
      uint8_t a_button: 1;
      uint8_t b_button: 1;
      uint8_t select: 1;
      uint8_t start: 1;
      uint8_t up: 1;
      uint8_t down: 1;
      uint8_t left: 1;
      uint8_t right: 1;
    };
  } input;
  int shifted_count = 0;
  bool prev_strobe = false;

  void detect_input();
  void detect_input_keyboard();
};

#endif
