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

// TODO: better cache it.
uint8_t zored_os(void) {
  switch (get_unicode_input_mode()) {
    case UC_OSX:
      return OS_MACOS;
    default:
      return OS_WINDOWS;
  }
}

uint8_t map_windows_keycode (uint8_t windowsKeycode) {
  switch (zored_os()) {
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
        case KC_MEDIA_PREV_TRACK:
          return KC_MEDIA_REWIND;
        case KC_MEDIA_NEXT_TRACK:
          return KC_MEDIA_FAST_FORWARD;
      }
    case OS_WINDOWS:
      break;
  }

  return windowsKeycode;
}

void code_down(uint8_t code) {
  register_code(map_windows_keycode(code));
}

void code_up(uint8_t code) {
  unregister_code(map_windows_keycode(code));
}

void close_app(void) {
  switch (zored_os()) {
    case OS_WINDOWS:
      // alt+f4
      code_down(KC_LALT);
      tap_code(KC_F4);
      code_up(KC_LALT);
      break;

    case OS_MACOS:
      // cmd+q
      register_code(KC_LCMD);
      tap_code(KC_Q);
      unregister_code(KC_LCMD);
      break;
  }
}

enum unicode_names {
  {{{unicode.names}}}
};

void tap_unicode(enum unicode_names name) {
  // Inspired by process_unicodemap().
  unicode_input_start();

  uint32_t code = pgm_read_dword(&unicode_map[name]);
  uint8_t input_mode = get_unicode_input_mode();

  if (code > 0x10FFFF || (code > 0xFFFF && input_mode == UC_WIN)) {
    unicode_input_cancel();
    return;
  }

  if (code > 0xFFFF && input_mode == UC_OSX) {
    code -= 0x10000;
    uint32_t lo = code & 0x3FF, hi = (code & 0xFFC00) >> 10;
    register_hex32(hi + 0xD800);
    register_hex32(lo + 0xDC00);
    unicode_input_finish();
    return;
  }

  register_hex32(code);
  unicode_input_finish();
}

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
      code_down(KC_LSHIFT);
      tap_code(KC_DOT);
      code_up(KC_LSHIFT);
      break;

    case CMB_RAR:
      tap_code(KC_MINUS);

      // >
      code_down(KC_LSHIFT);
      tap_code(KC_DOT);
      code_up(KC_LSHIFT);
      break;

    case CMB_UND:
      // _
      code_down(KC_LSHIFT);
      tap_code(KC_MINUS);
      code_up(KC_LSHIFT);
      break;

    case CMB_QUI:
      close_app();
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


{{#planck}}
void planck_ez_led_all_off(void) {
  planck_ez_left_led_off();
  planck_ez_right_led_off();
}
{{/planck}}

void matrix_init_user(void) {
  steno_set_mode(STENO_MODE_GEMINI);
  {{#ergodox}}
  ergodox_led_all_set(LED_BRIGHTNESS_LO);
  {{/ergodox}}
  {{#planck}}
  planck_ez_right_led_level(64);
  planck_ez_left_led_level(64);
  planck_ez_led_all_off();
  clicky_off(); // TODO: maybe there is some ENV for that?
  {{/planck}}
}

void spotlight_start(void) {
  register_code(KC_LGUI);
  switch (zored_os()) {
    case OS_WINDOWS:
      tap_code(KC_R);
      break;

    case OS_MACOS:
      tap_code(KC_SPC);
      break;
  }
  unregister_code(KC_LGUI);
}

void spotlight_finish(void) {
  tap_code(KC_ENTER);
}

LEADER_EXTERNS();
void matrix_scan_user(void) {
  LEADER_DICTIONARY() {
    leading = false;
    leader_end();
    SEQ_ONE_KEY(KC_U) {
      SEND_STRING("zored");
    }
    SEQ_ONE_KEY(KC_E) {
      SEND_STRING("zored.box@gmail.com");
    }
    SEQ_ONE_KEY(KC_P) {
      spotlight_start();
      SEND_STRING("enpass");
      spotlight_finish();
    }
    SEQ_ONE_KEY(KC_S) {
      // Make screenshot:
      switch (zored_os()) {
        case OS_MACOS:
          register_code(KC_LGUI);
          register_code(KC_LCTRL);
          register_code(KC_LSHIFT);
          tap_code(KC_4);
          unregister_code(KC_LGUI);
          unregister_code(KC_LCTRL);
          unregister_code(KC_LSHIFT);
          break;
        case OS_WINDOWS:
          register_code(KC_LGUI);
          register_code(KC_LSHIFT);
          tap_code(KC_S);
          unregister_code(KC_LGUI);
          unregister_code(KC_LSHIFT);
          break;
      }
    }
  }
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
{{!
    case UC_M_OS:
      zored_os = OS_MACOS;
      break;

    case UC_M_WC:
      zored_os = OS_WINDOWS;
      break;
}}
  }

  return !complete;
};

uint32_t layer_state_set_user(uint32_t state) {
  uint8_t layer = biton32(state);

{{#ergodox}}
  ergodox_led_all_off();
  switch (layer) {
    {{{ergodox.lights}}}
  }
{{/ergodox}}
{{#planck}}
  planck_ez_led_all_off();
  switch (layer) {
    {{{planck.lights}}}
  }
{{/planck}}

  return state;
};
