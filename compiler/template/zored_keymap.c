{{! Keymap template for all of my keymaps. Has a lot of common code. }}

{{#ergodox}}
#include "ergodox_ez.h"
{{/ergodox}}

{{#planck}}
#include QMK_KEYBOARD_H
#include "muse.h"
#include "eeprom.h"
{{/planck}}

#include "action_layer.h"
#include "version.h"
#include "keymap_steno.h"

#define LCGS(code) LCTL(LGUI(LSFT(code)))
#define LCS(code) LCTL(LSFT(code))

#define SLT(kc) (QK_LSFT | QK_LALT | (kc))
#define ALT_TAB LALT(KC_TAB)
#define SLT_TAB SLT(KC_TAB)

enum operating_systems {
  OS_MACOS = 1,
  OS_WINDOWS,
};

// Get operating system. Windows is default.
uint8_t get_os (void) {
  switch (get_unicode_input_mode()) {
    case UC_OSX:
      return OS_MACOS;

    case UC_WIN:
    case UC_WINC:
      return OS_WINDOWS;
  }

  return OS_WINDOWS;
}

uint8_t map_windows_keycode (uint8_t windowsKeycode) {
  switch (get_os()) {
    case OS_MACOS:
      switch (windowsKeycode) {
        case KC_LCTRL:
          return KC_LGUI;
        case KC_RCTRL:
          return KC_RGUI;
        case KC_LGUI:
          return KC_LCTRL;
        case KC_RGUI:
          return KC_RCTRL;
      }
  }

  return windowsKeycode;
}

void register_win_code(uint8_t code) {
  register_code(map_windows_keycode(code));
}

void unregister_win_code(uint8_t code) {
  unregister_code(map_windows_keycode(code));
}

enum unicode_names {
  {{{unicode.names}}}
};

const uint32_t PROGMEM
unicode_map[] = {
  {{{unicode.map}}}
};

// Combos:
// - Unique only!
// - Don't forget to update COMBO_COUNT.
const uint16_t PROGMEM combo_esc[] = {KC_Y, KC_U, COMBO_END};
const uint16_t PROGMEM combo_right_arrow[] = {KC_N, KC_M, COMBO_END};
const uint16_t PROGMEM combo_fat_right_arrow[] = {KC_F, KC_G, COMBO_END};
const uint16_t PROGMEM combo_underscore[] = {KC_H, KC_J, COMBO_END};
const uint16_t PROGMEM combo_quit[] = {KC_I, KC_O, COMBO_END};
const uint16_t PROGMEM combo_backslash[] = {KC_K, KC_L, COMBO_END};

enum combo_names {
  CMB_ESC = 0,
  CMB_RAR,
  CMB_FRAR,
  CMB_UND,
  CMB_QUI,
  CMB_BSLS,
};

combo_t key_combos[COMBO_COUNT] = {
  [CMB_ESC] = COMBO_ACTION(combo_esc),
  [CMB_RAR] = COMBO_ACTION(combo_right_arrow),
  [CMB_FRAR] = COMBO_ACTION(combo_fat_right_arrow),
  [CMB_UND] = COMBO_ACTION(combo_underscore),
  [CMB_QUI] = COMBO_ACTION(combo_quit),
  [CMB_BSLS] = COMBO_ACTION(combo_backslash),
};

void process_combo_event(uint8_t combo_index, bool pressed) {
  if (!pressed) {
    return;
  }

  switch(combo_index) {
    case CMB_ESC:
      tap_code(KC_ESC);
      break;

    case CMB_FRAR:
      tap_code(KC_EQL);

      // >
      register_win_code(KC_LSHIFT);
      tap_code(KC_DOT);
      unregister_win_code(KC_LSHIFT);
      break;

    case CMB_RAR:
      tap_code(KC_MINUS);

      // >
      register_win_code(KC_LSHIFT);
      tap_code(KC_DOT);
      unregister_win_code(KC_LSHIFT);
      break;

    case CMB_UND:
      // _
      register_win_code(KC_LSHIFT);
      tap_code(KC_MINUS);
      unregister_win_code(KC_LSHIFT);
      break;

    case CMB_QUI:
      switch (get_os()) {
        case OS_WINDOWS:
          // alt+f4
          register_win_code(KC_LALT);
          tap_code(KC_F4);
          unregister_win_code(KC_LALT);
          break;

        case OS_MACOS:
          // cmd+q
          register_code(KC_LCMD);
          tap_code(KC_Q);
          unregister_code(KC_LCMD);
          break;
      }
      break;

    case CMB_BSLS:
      tap_code(KC_BSLS);
      break;
  }
};

enum layers {
{{{layers.names}}}
};

{{#dance}}
enum dance_keys {
  {{{dance.names}}}
};
enum dance_action_names {
  {{{dance.actionNames}}}
};

static int dance_key_states[{{{dance.count}}}] = {0};

void on_dance(qk_tap_dance_state_t *state, void *user_data) {
  uint16_t dance_key = state->keycode - QK_TAP_DANCE;

  if (state->count == 0) {
    return;
  }

  switch (dance_key) {
    {{{dance.onDance}}}
  }
}

void on_dance_reset(qk_tap_dance_state_t *state, void *user_data) {
  uint16_t dance_key = state->keycode - QK_TAP_DANCE;
  switch (dance_key_states[dance_key]) {
    {{{dance.onDanceReset}}}
  }
}


#define ACTION_TAP_DANCE_DOUBLE_TIME(kc1, kc2, tap_specific_tapping_term) { \
    .fn = { qk_tap_dance_pair_on_each_tap, qk_tap_dance_pair_finished, qk_tap_dance_pair_reset }, \
    .user_data = (void *)&((qk_tap_dance_pair_t) { kc1, kc2 }),  \
    .custom_tapping_term = tap_specific_tapping_term, \
  }
#define DANCE_MODIFIER() ACTION_TAP_DANCE_FN_ADVANCED_TIME(NULL, on_dance, on_dance_reset, TAPPING_TERM_TAP_DANCE)
#define DANCE_TWO(k1,k11) ACTION_TAP_DANCE_DOUBLE_TIME(k1, k11, TAPPING_TERM_TAP_DANCE)

qk_tap_dance_action_t tap_dance_actions[] = {
    {{{dance.actions}}}
};
{{/dance}}

enum custom_keycodes {
  ZKC_BTL = SAFE_RANGE,

  // At the end:
  DYNAMIC_MACRO_RANGE,
};

#include "dynamic_macro.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
{{{layers.keys}}}
};

void matrix_init_user(void) {
  steno_set_mode(STENO_MODE_GEMINI);
  {{#ergodox}}
  ergodox_led_all_set(LED_BRIGHTNESS_LO);
  {{/ergodox}}
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (!process_record_dynamic_macro(keycode, record)) {
    return false;
  }

  bool complete = false;

  switch (keycode) {
    case ZKC_BTL:
      if (record->event.pressed) {
        clear_keyboard();
        bootloader_jump();
        complete = true;
      }
      break;
  }

  return !complete;
};

uint32_t layer_state_set_user(uint32_t state) {
{{#ergodox}}
  ergodox_led_all_off();

  switch (biton32(state)) {
    {{{ergodox.lights}}}
  }
{{/ergodox}}

{{#planck}}
  palClearPad(GPIOB, 8);
  palClearPad(GPIOB, 9);
  uint8_t layer = biton32(state);
  switch (layer) {
    {{{planck.lights}}}
  }
{{/planck}}

  return state;
};