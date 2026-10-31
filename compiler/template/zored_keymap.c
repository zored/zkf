{{! Keymap template for all of my keymaps. Has a lot of common code. }}

#include QMK_KEYBOARD_H

#include "action_layer.h"
#include "version.h"

#ifdef STENO_ENABLE
 #include "keymap_steno.h"
#endif

enum custom_keycodes {
  ZKC_BTL = SAFE_RANGE,
  {{{layers.doKeys.names}}}
};

enum operating_systems {
  OS_MACOS = 1,
  OS_WINDOWS,
} zored_os = OS_WINDOWS;

{{#mappings}}
uint8_t mappingIndex = 0;
uint8_t mappingIndexMax = {{{mappings.maxIndex}}};
uint8_t map_code8_hash (uint8_t keycode) {
  {{{mappings.code}}}
  return keycode;
}
uint8_t map_code16_hash (uint16_t keycode) {
  {{{mappings.code}}}
  return 0;
}
{{/mappings}}

uint8_t map_mod (uint8_t mod) {
  switch (zored_os) {
    case OS_MACOS:
      switch (mod) {
        case MOD_LCTL:
          return MOD_LGUI;
        case MOD_LGUI:
          return MOD_LCTL;
      }
    case OS_WINDOWS:
      break;
  }

  return mod;
}

void do_one_shot (uint8_t keycode) {
  set_oneshot_mods(map_mod(keycode));
}

uint8_t map_code (uint8_t keycode) {
  switch (zored_os) {
    case OS_MACOS:
      switch (keycode) {
        case KC_LCTL:
          return KC_LGUI;
        case KC_RCTL:
          return KC_RGUI;
        case KC_LGUI:
          return KC_LCTL;
        case KC_RGUI:
          return KC_RCTL;
        case KC_MEDIA_PREV_TRACK:
          return KC_MEDIA_REWIND;
        case KC_MEDIA_NEXT_TRACK:
          return KC_MEDIA_FAST_FORWARD;
      }
    case OS_WINDOWS:
      break;
  }

  {{#mappings}}
  keycode = map_code8_hash(keycode);
  {{/mappings}}

  return keycode;
}

void code_down(uint8_t code) {
  register_code(map_code(code));
}

void code_up(uint8_t code) {
  unregister_code(map_code(code));
}

void code_hold_ms(uint8_t code, uint32_t timeout) {
  code_down(KC_CAPS);
  wait_ms(timeout);
  code_up(KC_CAPS);
}

// Helper functions:
{{functions}}

enum do_command {
  DO_FIND_BEGIN = 1,
  DO_FIND_END,
  DO_ENPASS,
  DO_MAIL,
  DO_LOGIN,
  DO_COPY, DO_PASTE,
  DO_TERMINAL,
  DO_SCREENSHOT,
  DO_BOOTLOADER,
  DO_CLOSE,
  DO_NEXT_LANGUAGE,
  DO_UNDERSCORE,
  DO_ARROW,
  DO_FAT_ARROW,
  DO_NOT_EQUALS,
  DO_EMOJI_PANEL,
  DO_AMPERSAND,
  DO_NEXT_MAPPING,
  DO_ONE_SHOT_CTRL,
  DO_ONE_SHOT_ALT,
  DO_ONE_SHOT_GUI,
  DO_ONE_SHOT_SHIFT,
{{#keyGroups.turbo}}
  DO_ONE_SHOT_TURBO,
{{/keyGroups.turbo}}
  DO_PAST, DO_FUTURE,
  DO_PREV_TAB, DO_NEXT_TAB,
{{#keyGroups.appSwitch}}
  DO_FUTURE_TAB, DO_PAST_TAB,
  DO_FUTURE_APP, DO_PAST_APP,
  DO_FUTURE_WINDOW, DO_PAST_WINDOW,
{{/keyGroups.appSwitch}}
  DO_MOUSE_SLOW, DO_MOUSE_FAST,
};


{{#keyGroups.turbo}}
typedef struct {
  bool waiting;
  uint16_t keycode;
  bool fingerPressing;
  bool signalActive;
  uint16_t signalTimer;
} Turbo;
Turbo turbo = {.waiting = false, .keycode = 0, .fingerPressing = false, .signalActive = false, .signalTimer = 0};
void turboWaitEnable(void) {
  turbo.waiting = true;
}
void turboSendSignal(void) {
  turbo.signalTimer = timer_read();
  if (turbo.signalActive) {
    register_code16(turbo.keycode);
    return;
  }
  unregister_code16(turbo.keycode);
}
void turboEnable(uint16_t code) {
  turbo.waiting = false;
  if (code == KC_DO_ONE_SHOT_TURBO) {
    turbo.keycode = 0;
    return;
  }
  turbo.keycode = code;
  turboSendSignal();
}
bool turboHandlePress(uint16_t code, bool pressed) {
  if (turbo.waiting && pressed) {
    turboEnable(code);
    return true;
  }
  if (code == turbo.keycode) {
    turbo.fingerPressing = pressed;
    turbo.signalActive = pressed;
    turboSendSignal();
    return true;
  }
  return false;
}
static inline void turboTick(void) {
  if (!turbo.fingerPressing || turbo.keycode == 0) {
    return;
  }
  uint16_t signalMs = timer_elapsed(turbo.signalTimer);
  uint16_t timeoutMs = TURBO_NO_SIGNAL_TIMEOUT;
  if (turbo.signalActive) {
    timeoutMs = TURBO_SIGNAL_TIMEOUT;
  }
  if (signalMs <= timeoutMs) {
    return;
  }
  turbo.signalActive = !turbo.signalActive;
  turboSendSignal();
}
{{/keyGroups.turbo}}


{{#keyGroups.appSwitch}}
typedef struct {
  bool active;
  uint16_t timer;
  uint8_t hold1;
  uint8_t hold2;
  uint8_t target;
} AppSwitch;
AppSwitch appSwitch = {.active = false, .timer = 0};
void appSwitchDeactivate(void) {
  if (!appSwitch.active) {
    return;
  }

  unregister_code(appSwitch.hold1);
  if (appSwitch.hold2 > 0) {
    unregister_code(appSwitch.hold2);
  }
  appSwitch.active = false;
}

enum target {
  TARGET_APP = 1,
  TARGET_WINDOW,
  TARGET_TAB
};

void appSwitchRun(bool past, uint8_t target) {
  // Deactivate on new switch type:
  if (appSwitch.active && appSwitch.target != target) {
    appSwitchDeactivate();
  }

  // Activate:
  if (!appSwitch.active) {
    appSwitch.hold2 = 0;
    switch (zored_os) {
      case OS_MACOS:
        switch (target) {
          case TARGET_APP:
          case TARGET_WINDOW:
            appSwitch.hold1 = KC_LGUI;
            break;
          case TARGET_TAB:
            appSwitch.hold1 = KC_LCTL;
            break;
        }
        break;
      default:
        switch (target) {
          case TARGET_APP:
          case TARGET_WINDOW:
            appSwitch.hold1 = KC_LALT;
            break;
          case TARGET_TAB:
            appSwitch.hold1 = KC_LCTL;
            break;
        }
        break;
    }
    register_code(appSwitch.hold1);
    if (appSwitch.hold2 > 0) {
      register_code(appSwitch.hold2);
    }
  }


  uint16_t tap = KC_TAB;
  switch (zored_os) {
    case OS_MACOS:
      switch (target) {
        case TARGET_WINDOW:
          tap = KC_TILD;
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  if (!past && tap != KC_RIGHT && tap != KC_LEFT) {
    tap = S(tap);
  }
  tap_code16(tap);

  appSwitch.timer = timer_read(); // - there may be more switches.
  appSwitch.active = true;
  appSwitch.target = target;
}
static inline void appSwitchTimeout(void) {
  if (!appSwitch.active) {
    return;
  }
  if (timer_elapsed(appSwitch.timer) <= APP_SWITCH_TIMEOUT) {
    return;
  }
  appSwitchDeactivate();
}
{{/keyGroups.appSwitch}}


uint8_t mouseSpeed = 1;
void setMouseSpeed(uint8_t newMouseSpeed) {
  // Turn off:
  if (mouseSpeed == newMouseSpeed || newMouseSpeed == 1) {
    mouseSpeed = 1;
    mousekey_off(MS_ACL2);
    mk_max_speed = MOUSEKEY_MAX_SPEED;  
    return;
  }

  mouseSpeed = newMouseSpeed;
  mousekey_on(MS_ACL2); // - make mk_max_speed the real mouse speed.

  switch (mouseSpeed) {
    case 0:
      mk_max_speed = MOUSEKEY_SLOW_SPEED;
      break;
    case 2:
      mk_max_speed = MOUSEKEY_FAST_SPEED;
      break;
  }
  mouseSpeed = newMouseSpeed;

}

// Advanced commands.
void run_advanced (uint8_t command) {
  switch (command) {
    case DO_FIND_BEGIN:
      switch (zored_os) {
        case OS_WINDOWS:
          tap_code(KC_LGUI);
          break;
        case OS_MACOS:
          tap_code16(G(KC_SPC));
          break;
      }
      break;
    case DO_FIND_END:
      tap_code(KC_ENTER);
      break;
    case DO_ONE_SHOT_CTRL:
      do_one_shot(MOD_LCTL);
      break;
    case DO_ONE_SHOT_ALT:
      do_one_shot(MOD_LALT);
      break;
    case DO_ONE_SHOT_GUI:
      do_one_shot(MOD_LGUI);
      break;
    case DO_ONE_SHOT_SHIFT:
      do_one_shot(MOD_LSFT);
      break;
    {{#keyGroups.turbo}}
    case DO_ONE_SHOT_TURBO:
      turboWaitEnable();
      break;
    {{/keyGroups.turbo}}
    {{#keyGroups.appSwitch}}
    case DO_PAST_APP:
      appSwitchRun(true, TARGET_APP);
      break;
    case DO_FUTURE_APP:
      appSwitchRun(false, TARGET_APP);
      break;
    case DO_PAST_WINDOW:
      appSwitchRun(true, TARGET_WINDOW);
      break;
    case DO_FUTURE_WINDOW:
      appSwitchRun(false, TARGET_WINDOW);
      break;
    case DO_FUTURE_TAB:
      appSwitchRun(false, TARGET_TAB);
      break;
    case DO_PAST_TAB:
      appSwitchRun(true, TARGET_TAB);
      break;
    case DO_NEXT_TAB:
      switch (zored_os) {
        case OS_MACOS:
          tap_code16(G(S(KC_RBRC)));
          break;
        case OS_WINDOWS:
          tap_code16(C(A(KC_RIGHT)));
          break;
      }
    case DO_PREV_TAB:
      switch (zored_os) {
        case OS_MACOS:
          tap_code16(G(S(KC_LBRC)));
          break;
        case OS_WINDOWS:
          tap_code16(C(A(KC_LEFT)));
          break;
      }
    {{/keyGroups.appSwitch}}
    case DO_FUTURE:
      switch (zored_os) {
        case OS_MACOS:
          tap_code16(G(KC_LBRC));
          break;
        case OS_WINDOWS:
          tap_code16(A(KC_LEFT));
          break;
      }
      break;
    case DO_PAST:
      switch (zored_os) {
        case OS_MACOS:
          tap_code16(G(KC_RBRC));
          break;
        case OS_WINDOWS:
          tap_code16(A(KC_RIGHT));
          break;
      }
      break;
    case DO_NEXT_MAPPING:
      run_advanced(DO_NEXT_LANGUAGE);  
{{#mappings}}
      mappingIndex++;
      if (mappingIndex > mappingIndexMax) {
        mappingIndex = 0;
      }
{{/mappings}}
      break;
    case DO_NEXT_LANGUAGE:
      ; // empty statement for declarations
      uint8_t hold = 0;
      uint8_t tap = 0;
      uint32_t timeout = 50;

      switch (zored_os) {
        case OS_WINDOWS:
          hold = KC_LALT;
          tap = KC_LSFT;
          break;
        case OS_MACOS:
          hold = KC_LGUI;
          tap = KC_SPC;
          break;
      }
      register_code(hold);
      wait_ms(timeout);
      tap_code(tap);
      wait_ms(timeout);
      unregister_code(hold);
      break;
    case DO_EMOJI_PANEL:
      switch (zored_os) {
        case OS_WINDOWS:
          tap_code16(G(KC_DOT));
          break;
        case OS_MACOS:
          tap_code16(G(C(KC_SPC)));
          break;
      }
      break;
    case DO_AMPERSAND:
      tap_code16(KC_AMPR);
      break;
    case DO_UNDERSCORE:
      SEND_STRING("_");
      break;
    case DO_ARROW:
      SEND_STRING("->");
      break;
    case DO_FAT_ARROW:
      SEND_STRING("=>");
      break;
    case DO_NOT_EQUALS:
      SEND_STRING("!=");
      break;
    case DO_ENPASS:
      run_advanced(DO_FIND_BEGIN);
      SEND_STRING("enpass");
      run_advanced(DO_FIND_END);
      break;
    case DO_TERMINAL:
      run_advanced(DO_FIND_BEGIN);
      switch (zored_os) {
        case OS_WINDOWS:
          SEND_STRING("conemu");
          break;
        case OS_MACOS:
          SEND_STRING("iterm");
          break;
      }
      run_advanced(DO_FIND_END);
      break;
    case DO_MAIL:
      SEND_STRING("zored.box@gmail.com");
      break;
    case DO_LOGIN:
      SEND_STRING("zored");
      break;
    case DO_SCREENSHOT:
      switch (zored_os) {
        case OS_MACOS:
          tap_code16(G(C(S(KC_4))));
          break;
        case OS_WINDOWS:
          tap_code16(G(S(KC_S)));
          break;
      }
      break;
    case DO_COPY:
      switch (zored_os) {
        case OS_MACOS:
          tap_code16(G(KC_C));
          break;
        case OS_WINDOWS:
          tap_code16(C(KC_C));
          break;
      }
      break;
    case DO_PASTE:
      switch (zored_os) {
        case OS_MACOS:
          tap_code16(G(KC_V));
          break;
        case OS_WINDOWS:
          tap_code16(C(KC_V));
          break;
      }
      break;
    case DO_BOOTLOADER:
      reset_keyboard();
      break;
    case DO_CLOSE:
      switch (zored_os) {
        case OS_WINDOWS:
          tap_code16(A(KC_F4));
          break;
        case OS_MACOS:
          tap_code16(G(KC_Q));
          break;
      }
      break;
    case DO_MOUSE_SLOW:
      setMouseSpeed(0);
      break;
    case DO_MOUSE_FAST:
      setMouseSpeed(2);
      break;
  }
}

{{#unicode}}
enum unicode_names {
  {{{unicode.names}}}
};

void tap_unicode(enum unicode_names name) {
  // Inspired by process_unicodemap().
  unicode_input_start();

  uint32_t code = pgm_read_dword(&unicode_map[name]);
  uint8_t input_mode = get_unicode_input_mode();

  if (code > 0x10FFFF || (code > 0xFFFF && input_mode == UNICODE_MODE_WINDOWS)) {
    unicode_input_cancel();
    return;
  }

  if (code > 0xFFFF && input_mode == UNICODE_MODE_MACOS) {
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
{{/unicode}}

{{{combos.definitions}}}

void process_combo_event(uint16_t combo_index, bool pressed) {
  if (!pressed) {
    return;
  }

  switch(combo_index) {
    {{{combos.declarations}}}
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

void on_dance_finished(tap_dance_state_t *state, void *user_data);
void on_dance_reset(tap_dance_state_t *state, void *user_data);

void dance_finish_as_tap(tap_dance_state_t *state) {
  state->pressed = false;
  on_dance_finished(state, NULL);
  on_dance_reset(state, NULL);
  state->finished = true;
}

void dance_flush_on_many_taps(tap_dance_state_t *state) {
  if (state->count == 0) {
    return;
  }
  uint16_t dance_key = state->index;

  switch (dance_key) {
    {{{dance.counts}}}
    default:
      return;
  }

  dance_finish_as_tap(state);
}

void dance_tap_on_enemy_hold(tap_dance_state_t *state) {
  if (state->count == 0) {
    return;
  }

  uint16_t dance_key = state->index;

  switch(dance_key) {
    {{{dance.enemies}}}
    default:
      return;
  }

  dance_finish_as_tap(state);
}

void on_dance_each_tap(tap_dance_state_t *state, void *user_data) {
  dance_tap_on_enemy_hold(state);
  dance_flush_on_many_taps(state);
}

void on_dance_finished(tap_dance_state_t *state, void *user_data) {
  if (state->count == 0) {
    return;
  }
  uint16_t dance_key = state->index;

  switch (dance_key) {
    {{{dance.onDance}}}
  }
}

void on_dance_reset(tap_dance_state_t *state, void *user_data) {
  uint16_t dance_key = state->index;
  switch (dance_key_states[dance_key]) {
    {{{dance.onDanceReset}}}
  }

  dance_key_states[dance_key] = 0;
}


#define DANCE_MODIFIER() ACTION_TAP_DANCE_FN_ADVANCED(on_dance_each_tap, on_dance_finished, on_dance_reset)

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
  if (IS_QK_TAP_DANCE(keycode)) {
    return TAPPING_TERM_TAP_DANCE;
  }
  return TAPPING_TERM;
}

tap_dance_action_t tap_dance_actions[] = {
    {{{dance.actions}}}
};
{{/dance}}

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
  #ifdef STENO_ENABLE
    steno_set_mode(STENO_MODE_GEMINI);
  #endif

  {{#ergodox}}
  ergodox_led_all_set(LED_BRIGHTNESS_LO);
  {{/ergodox}}
  {{#planck}}
  planck_ez_right_led_level(64);
  planck_ez_left_led_level(64);
  planck_ez_led_all_off();
  {{/planck}}
}

void matrix_scan_user(void) {
{{#keyGroups.turbo}}
  turboTick();
{{/keyGroups.turbo}}

{{#keyGroups.appSwitch}}
  appSwitchTimeout();
{{/keyGroups.appSwitch}}

}

{{^ymd09}}
void leader_end_user(void) {
  if (leader_sequence_one_key(KC_U)) {
    run_advanced(DO_LOGIN);
  } else if (leader_sequence_one_key(KC_P)) {
    run_advanced(DO_ENPASS);
  } else if (leader_sequence_one_key(KC_S)) {
    run_advanced(DO_SCREENSHOT);
  }
}
{{/ymd09}}

void keyboard_post_init_user(void) {
{{#unicode}}
  switch (get_unicode_input_mode()) {
    case UNICODE_MODE_MACOS:
      zored_os = OS_MACOS;
  }
{{/unicode}}
{{#planck}}
  clicky_off();
{{/planck}}
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  bool pressed = record->event.pressed;

{{#keyGroups.turbo}}
  if (turboHandlePress(keycode, pressed)) {
    return false;
  }
{{/keyGroups.turbo}}

{{#mappings}}
  uint16_t newKeycode = map_code16_hash(keycode);
  if (newKeycode > 0) {
    if (pressed) {
      register_code(newKeycode);
    } else {
      unregister_code(newKeycode);
    }
    return false;
  }
{{/mappings}}

  if (!pressed) {
    return true;
  }

{{#keyGroups.appSwitch}}
  switch (keycode) {
    case KC_DO_PAST_APP: break;
    case KC_DO_FUTURE_APP: break;
    case KC_DO_PAST_WINDOW: break;
    case KC_DO_FUTURE_WINDOW: break;
    case KC_DO_PAST_TAB: break;
    case KC_DO_FUTURE_TAB: break;
    default:
      appSwitchDeactivate();
      break;
  }
{{/keyGroups.appSwitch}}

  switch (keycode) {
    case UC_MAC:
      zored_os = OS_MACOS;
      return true;

    case UC_WINC:
      zored_os = OS_WINDOWS;
      return true;

    {{{layers.doKeys.cases}}}

    default:
      return true;
  }

  return false;
}


{{^ymd09}}
layer_state_t layer_state_set_user(layer_state_t state) {
  uint8_t layer = get_highest_layer(state);

{{#ergodox}}
  ergodox_led_all_off();
  switch (layer) {
    {{{ergodox.onLayerOn}}}
  }
{{/ergodox}}
{{#planck}}
  planck_ez_led_all_off();
  switch (layer) {
    {{{planck.onLayerOn}}}
  }
{{/planck}}

  return state;
};
{{/ymd09}}
