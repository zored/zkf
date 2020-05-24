#ifndef DISABLE_RGB_MATRIX_ZORED_EFFECT
RGB_MATRIX_EFFECT(zored_effect)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#include "keymap_zored.h"

static bool zored_effect(effect_params_t* params) {
  RGB_MATRIX_USE_LIMITS(led_min, led_max);

  uint8_t red = 0x00;
  uint8_t green = 0x00;
  uint8_t blue = 0x00;

  switch (zored_os) {
    case OS_WINDOWS:
      blue = 0x00;
      break;
    case OS_MACOS:
      red = 0xff;
      break;
  }

  rgb_matrix_set_color_all(red, green, blue);

  return led_max < DRIVER_LED_TOTAL;
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif // DISABLE_RGB_MATRIX_ZORED_EFFECT
