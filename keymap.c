// LEGACY FILE.
// GET RID OF IT.
// USE ./src/main.cpp

#include "ergodox_ez.h"
#include "debug.h"
#include "action_layer.h"
#include "version.h"
#include "keymap_steno.h"

enum {
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
  E_LOL = 0,
  E_JOY,
  E_THI,
  E_THU,
  E_BIC,
  E_SMI,
  E_FIR,
  E_HEA,
  E_CRY,
  E_SOB,
  E_DNC,
  RU_Q,
};

const uint32_t PROGMEM unicode_map[] = {
  [E_LOL] 0x1F60A, // 😊
  [E_JOY] 0x1F602, // 😂
  [E_THI] 0x1F914, // 🤔
  [E_THU] 0x1F44D, // 👍
  [E_BIC] 0x1F4AA, // 💪
  [E_SMI] 0x1F60F, // 😏
  [E_FIR] 0x1F525, // 🔥
  [E_HEA] 0x1F496, // 💖
  [E_CRY] 0x1F622, // 😢
  [E_SOB] 0x1F62D, // 😭
  [E_DNC] 0x1F57A, // 🕺
  [RU_Q] 0x430, // а
};

enum dance_state_values {
  DANCE_Z = 1,
  DANCE_X,
  DANCE_C,
  DANCE_SLSH,
  DANCE_DOT,
  DANCE_COMM,
  // Left:
  DANCE_LEFT_CTRL,
  DANCE_LEFT_CTRL_SHIFT,
  DANCE_LEFT_ALT,
  DANCE_LEFT_ALT_SHIFT,
  DANCE_LEFT_GUI,
  DANCE_LEFT_GUI_SHIFT,
  // Right:
  DANCE_RIGHT_CTRL,
  DANCE_RIGHT_CTRL_SHIFT,
  DANCE_RIGHT_ALT,
  DANCE_RIGHT_ALT_SHIFT,
  DANCE_RIGHT_GUI,
  DANCE_RIGHT_GUI_SHIFT,
};

enum dance_keys {
  DNC_Z = 0,
  DNC_X,
  DNC_C,
  DNC_SLSH,
  DNC_DOT,
  DNC_COMM,
  DNC_F1,
  DNC_F2,
  DNC_F3,
  DNC_F4,
  DNC_F5,
  DNC_F6,
  DNC_F7,
  DNC_F8,
  DNC_F9,
  DNC_F10,
  DNC_KF1,
  DNC_KF2,
  DNC_KF3,
  DNC_KF4,
  DNC_KF5,
  DNC_KF6,
  DNC_KF7,
  DNC_KF8,
  DNC_KF9,
  DNC_KF10,
  // Last:
  DNC_AMOUNT
};

static int dance_states[DNC_AMOUNT] = {0};

void dance_modifier_key(
  qk_tap_dance_state_t *state,
  int danceStateTap,
  int danceStateHold1,
  int danceStateHold2,
  uint8_t tapKeycode,
  uint8_t holdKeycode1,
  uint8_t holdKeycode2,
  uint16_t danceKey
) {
  // Not enough taps:
  if (state->count < 1) {
    return;
  }

  // Register modifiers:
  if (state->pressed) {
    // First modifier:
    register_win_code(holdKeycode1);
    dance_states[danceKey] = danceStateHold1;

    if (state->count == 1) {
      return;
    }

    // Second modifier:
    register_win_code(holdKeycode2);
    dance_states[danceKey] = danceStateHold2;
    return;
  }

  // Make previous taps (if needed):
  for (int i = 1; i < state->count; i++) {
    tap_code(tapKeycode);
  }

  // Last tap:
  register_win_code(tapKeycode);
  dance_states[danceKey] = danceStateTap;
}

void on_dance(qk_tap_dance_state_t *state, void *user_data) {
  uint16_t danceKey = state->keycode - QK_TAP_DANCE;

  switch (danceKey) {
    case DNC_Z:
      dance_modifier_key(
        state,
        DANCE_Z,
        DANCE_LEFT_CTRL,
        DANCE_LEFT_CTRL_SHIFT,
        KC_Z,
        KC_LCTRL,
        KC_LSHIFT,
        DNC_Z
      );
      break;

    case DNC_X:
      dance_modifier_key(
        state,
        DANCE_X,
        DANCE_LEFT_ALT,
        DANCE_LEFT_ALT_SHIFT,
        KC_X,
        KC_LALT,
        KC_LSHIFT,
        DNC_X
      );
      break;

    case DNC_C:
      dance_modifier_key(
        state,
        DANCE_C,
        DANCE_LEFT_GUI,
        DANCE_LEFT_GUI_SHIFT,
        KC_C,
        KC_LGUI,
        KC_LSHIFT,
        DNC_C
      );
      break;

    case DNC_SLSH:
      dance_modifier_key(
        state,
        DANCE_SLSH,
        DANCE_RIGHT_CTRL,
        DANCE_RIGHT_CTRL_SHIFT,
        KC_SLSH,
        KC_RCTRL,
        KC_RSHIFT,
        DNC_SLSH
      );
      break;

    case DNC_DOT:
      dance_modifier_key(
        state,
        DANCE_DOT,
        DANCE_RIGHT_ALT,
        DANCE_RIGHT_ALT_SHIFT,
        KC_DOT,
        KC_RALT,
        KC_RSHIFT,
        DNC_DOT
      );
      break;

    case DNC_COMM:
      dance_modifier_key(
        state,
        DANCE_COMM,
        DANCE_RIGHT_GUI,
        DANCE_RIGHT_GUI_SHIFT,
        KC_COMM,
        KC_RGUI,
        KC_RSHIFT,
        DNC_COMM
      );
      break;
  }
}

void on_dance_reset(qk_tap_dance_state_t *state, void *user_data) {
  uint16_t danceKey = state->keycode - QK_TAP_DANCE;
  switch (dance_states[danceKey]) {
    case DANCE_Z:
      unregister_win_code(KC_Z);
      break;

    case DANCE_X:
      unregister_win_code(KC_X);
      break;

    case DANCE_C:
      unregister_win_code(KC_C);
      break;

    case DANCE_SLSH:
      unregister_win_code(KC_SLSH);
      break;

    case DANCE_DOT:
      unregister_win_code(KC_DOT);
      break;

    case DANCE_COMM:
      unregister_win_code(KC_COMM);
      break;

    // Left:
    case DANCE_LEFT_CTRL:
      unregister_win_code(KC_LCTRL);
      break;

    case DANCE_LEFT_CTRL_SHIFT:
      unregister_win_code(KC_LSHIFT);
      unregister_win_code(KC_LCTRL);
      break;

    case DANCE_LEFT_ALT:
      unregister_win_code(KC_LALT);
      break;

    case DANCE_LEFT_ALT_SHIFT:
      unregister_win_code(KC_LSHIFT);
      unregister_win_code(KC_LALT);
      break;

    case DANCE_LEFT_GUI:
      unregister_win_code(KC_LGUI);
      break;

    case DANCE_LEFT_GUI_SHIFT:
      unregister_win_code(KC_LSHIFT);
      unregister_win_code(KC_LGUI);
      break;

    // Right:
    case DANCE_RIGHT_CTRL:
      unregister_win_code(KC_RCTRL);
      break;

    case DANCE_RIGHT_CTRL_SHIFT:
      unregister_win_code(KC_RSHIFT);
      unregister_win_code(KC_RCTRL);
      break;

    case DANCE_RIGHT_ALT:
      unregister_win_code(KC_RALT);
      break;

    case DANCE_RIGHT_ALT_SHIFT:
      unregister_win_code(KC_RSHIFT);
      unregister_win_code(KC_RALT);
      break;

    case DANCE_RIGHT_GUI:
      unregister_win_code(KC_RGUI);
      break;

    case DANCE_RIGHT_GUI_SHIFT:
      unregister_win_code(KC_RSHIFT);
      unregister_win_code(KC_RGUI);
      break;
  }
  dance_states[danceKey] = 0;
}

#define DANCE_MODIFIER() ACTION_TAP_DANCE_FN_ADVANCED_TIME(NULL, on_dance, on_dance_reset, TAPPING_TERM_TAP_DANCE)
#define DANCE_TWO(k1,k11) ACTION_TAP_DANCE_DOUBLE_TIME(k1, k11, TAPPING_TERM_TAP_DANCE)

qk_tap_dance_action_t tap_dance_actions[] = {
    // Left (ctrl - alt - gui)
    [DNC_Z] = DANCE_MODIFIER(),
    [DNC_X] = DANCE_MODIFIER(),
    [DNC_C] = DANCE_MODIFIER(),

    // Right: (ctrl - alt - gui)
    [DNC_SLSH] = DANCE_MODIFIER(),
    [DNC_DOT] = DANCE_MODIFIER(),
    [DNC_COMM] = DANCE_MODIFIER(),

    // F-dance:
    [DNC_F1] = DANCE_TWO(KC_F1, KC_F11),
    [DNC_F2] = DANCE_TWO(KC_F2, KC_F12),
    [DNC_F3] = DANCE_TWO(KC_F3, KC_F13),
    [DNC_F4] = DANCE_TWO(KC_F4, KC_F14),
    [DNC_F5] = DANCE_TWO(KC_F5, KC_F15),
    [DNC_F6] = DANCE_TWO(KC_F6, KC_F16),
    [DNC_F7] = DANCE_TWO(KC_F7, KC_F17),
    [DNC_F8] = DANCE_TWO(KC_F8, KC_F18),
    [DNC_F9] = DANCE_TWO(KC_F9, KC_F19),
    [DNC_F10]= DANCE_TWO(KC_F10,KC_F20),

    // Number F-dance:
    [DNC_KF1] = DANCE_TWO(KC_1, KC_F11),
    [DNC_KF2] = DANCE_TWO(KC_2, KC_F12),
    [DNC_KF3] = DANCE_TWO(KC_3, KC_F13),
    [DNC_KF4] = DANCE_TWO(KC_4, KC_F14),
    [DNC_KF5] = DANCE_TWO(KC_5, KC_F15),
    [DNC_KF6] = DANCE_TWO(KC_6, KC_F16),
    [DNC_KF7] = DANCE_TWO(KC_7, KC_F17),
    [DNC_KF8] = DANCE_TWO(KC_8, KC_F18),
    [DNC_KF9] = DANCE_TWO(KC_9, KC_F19),
    [DNC_KF10]= DANCE_TWO(KC_0, KC_F10),
};

enum custom_keycodes {
  ZKC_BTL = SAFE_RANGE,

  // At the end:
  DYNAMIC_MACRO_RANGE,
};

#include "dynamic_macro.h"


enum layers {
  L_DEF = 0,
  L_PLO,
  L_SYM,
  L_NAV,
  L_EMO,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [L_DEF] = LAYOUT_ergodox(
    KC_ESC,           KC_1,          KC_2,          KC_3,           KC_4,           KC_5,           _______,
    KC_TAB,           KC_Q,          KC_W,          KC_E,           KC_R,           KC_T,           _______,
    LT(L_SYM,KC_CLCK),KC_A,          KC_S,          KC_D,           KC_F,           KC_G,
    KC_LSPO,          TD(DNC_Z),     TD(DNC_X),     TD(DNC_C),      KC_V,           KC_B,           KC_MEH,
    KC_LCTRL,         KC_LALT,       KC_DLR,        KC_LEFT,        KC_RGHT,
                                                                                    KC_ESC,         _______,
                                                                                                    KC_HOME,
                                                                    KC_SPC,         KC_BSPC,        KC_END,
        OSL(L_SYM),     KC_6,           KC_7,           KC_8,           KC_9,           KC_0,             KC_RBRC,
        _______,        KC_Y,           KC_U,           KC_I,           KC_O,           LT(L_NAV,KC_P),   KC_LBRC,
                        KC_H,           KC_J,           KC_K,           KC_L,           LT(L_NAV,KC_SCLN),LT(L_EMO, KC_QUOT),
        KC_HYPR,        KC_N,           KC_M,           TD(DNC_COMM),   TD(DNC_DOT),    TD(DNC_SLSH),     KC_RSPC,
        KC_DOWN,        KC_UP,          KC_BSLS,        KC_GRV,         TG(L_PLO),
        KC_ESC,         _______,
        KC_PGUP,
        KC_PGDN,        KC_TAB,         KC_ENT
    ),

  [L_SYM] = LAYOUT_ergodox(
    _______,        TD(DNC_F1),     TD(DNC_F2),     TD(DNC_F3),     TD(DNC_F4),     TD(DNC_F5),     _______,
    _______,        KC_EXLM,        KC_AT,          KC_LCBR,        KC_RCBR,        KC_BSLS,        _______,
    _______,        KC_HASH,        KC_DLR,         KC_LPRN,        KC_RPRN,        KC_GRV,
    _______,        KC_PERC,        KC_CIRC,        KC_LBRC,        KC_RBRC,        KC_TILD,        _______,
    _______,        _______,        _______,        _______,        _______,
                                                                                    _______,        _______,
                                                                                                    _______,
                                                                    _______,        KC_DEL,         _______,
        _______,        _______,        _______,        _______,        _______,        _______,        _______,
        _______,        KC_PPLS,        KC_7,           KC_8,           KC_9,           _______,        KC_PIPE,
                        KC_EQL,         KC_4,           KC_5,           KC_6,           KC_PAST,        _______,
        _______,        KC_MINS,        KC_1,           KC_2,           KC_3,           KC_AMPR,        _______,
        KC_EQL,         KC_0,           KC_DOT,         _______,        _______,
        _______,        _______,
        _______,
        _______,        _______,        _______
    ),

  [L_NAV] = LAYOUT_ergodox(
    _______,        TD(DNC_F6),     TD(DNC_F7),     TD(DNC_F8),     TD(DNC_F9),     TD(DNC_F10),    _______,
    _______,        KC_MENU,        KC_BTN2,        KC_MS_U,        KC_BTN1,        _______,        _______,
    _______,        _______,        KC_MS_L,        KC_MS_D,        KC_MS_R,        _______,
    _______,        _______,        _______,        _______,        _______,        _______,        _______,
    _______,        _______,        _______,        KC_BTN1,        KC_BTN2,
                                                                                    _______,        _______,
                                                                                                    _______,
                                                                    _______,        _______,        _______,
        _______,        KC_PWR,         KC_SLEP,        UC_M_WC,        UC_M_OS,        _______,        _______,
        _______,        _______,        KC_BRID,        KC_BRIU,        _______,        _______,        _______,
                        KC_LEFT,        KC_DOWN,        KC_UP,          KC_RGHT,        _______,        _______,
        _______,        KC_WH_L,        KC_WH_D,        KC_WH_U,        KC_WH_R,        _______,        _______,
        KC_VOLD,        KC_VOLU,        KC_MUTE,        _______,        _______,
        _______,        _______,
        _______,
        _______,        _______,        _______
    ),

  [L_PLO] = LAYOUT_ergodox(
    _______,        _______,        _______,        _______,        _______,        _______,        _______,
    _______,        STN_N1,         STN_N2,         STN_N3,         STN_N4,         STN_N5,         _______,
    _______,        STN_S1,         STN_TL,         STN_PL,         STN_HL,         STN_ST1,
    _______,        STN_S2,         STN_KL,         STN_WL,         STN_RL,         STN_ST2,           _______,
    _______,        _______,        _______,        STN_A,          STN_O,
                                                                                    _______,        _______,
                                                                                                    _______,
                                                                    STN_A,           STN_O,           _______,
        _______,        _______,        _______,        _______,        _______,        _______,         _______,
        _______,        STN_N6,         STN_N7,         STN_N8,         STN_N9,         STN_NA,          STN_NB,
                        STN_ST3,        STN_FR,         STN_PR,         STN_LR,         STN_TR,          STN_DR,
        _______,        STN_ST4,        STN_RR,         STN_BR,         STN_GR,         STN_SR,          STN_ZR,
        STN_E,          STN_U,          _______,        _______,        _______,
        _______,        _______,
        _______,
        _______,        STN_E,           STN_U
    ),

  [L_EMO] = LAYOUT_ergodox(
    _______,        _______,        _______,        _______,        _______,        _______,        _______,
    _______,        X(E_LOL),       X(E_JOY),       X(E_THI),       X(E_THU),       X(E_BIC),       _______,
    _______,        X(E_SMI),       X(E_FIR),       X(E_HEA),       X(E_CRY),       X(E_SOB),
    _______,        _______,        _______,        _______,        _______,        _______,        _______,
    _______,        _______,        _______,        _______,        _______,
                                                                                    _______,        _______,
                                                                                                    _______,
                                                                    _______,        _______,        _______,
        ZKC_BTL,        DYN_REC_START1, DYN_REC_START2, DYN_REC_STOP,   DYN_MACRO_PLAY1,DYN_MACRO_PLAY2,_______,
        _______,        X(RU_Q),        TD(DNC_F7),     TD(DNC_F8),     TD(DNC_F9),     _______,    _______,
                        _______,        TD(DNC_F4),     TD(DNC_F5),     TD(DNC_F6),     _______,        _______,
        _______,        _______,        TD(DNC_F1),     TD(DNC_F2),     TD(DNC_F3),     _______,        _______,
        _______,        _______,        _______,        TD(DNC_F10),    _______,
        _______,        _______,
        _______,
        _______,        _______,        _______
    ),

  /*
  [4] = LAYOUT_ergodox(
    _______,        _______,        _______,        _______,        _______,        _______,        _______,
    _______,        _______,        _______,        _______,        _______,        _______,        _______,
    _______,        _______,        _______,        _______,        _______,        _______,
    _______,        _______,        _______,        _______,        _______,        _______,        _______,
    _______,        _______,        _______,        _______,        _______,
                                                                                    _______,        _______,
                                                                                                    _______,
                                                                    _______,        _______,        _______,
        _______,        _______,        _______,        _______,        _______,        _______,        _______,
        _______,        _______,        _______,        _______,        _______,        _______,        _______,
                        _______,        _______,        _______,        _______,        _______,        _______,
        _______,        _______,        _______,        _______,        _______,        _______,        _______,
        _______,        _______,        _______,        _______,        _______,
        _______,        _______,
        _______,
        _______,        _______,        _______
    ),
   */

};

void matrix_init_user(void) {
  steno_set_mode(STENO_MODE_GEMINI);
}

void dim_leds(void) {
  ergodox_board_led_off();
  ergodox_right_led_1_off();
  ergodox_right_led_2_off();
  ergodox_right_led_3_off();
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
  dim_leds();

  ergodox_led_all_set(LED_BRIGHTNESS_LO);
  switch (biton32(state)) {
    case L_NAV:
      ergodox_right_led_on(1);
      break;
    case L_SYM:
      ergodox_right_led_on(2);
      break;
    case L_PLO:
      ergodox_right_led_on(3);
      break;
    case L_EMO:
      ergodox_right_led_on(1);
      ergodox_right_led_on(3);
      break;
  }

  return state;
};
