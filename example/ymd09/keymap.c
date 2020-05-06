


#include QMK_KEYBOARD_H

#include "action_layer.h"
#include "version.h"

#ifdef STENO_ENABLE
 #include "keymap_steno.h"
#endif

#define LCGS(code) LCTL(LGUI(LSFT(code)))
#define LCS(code) LCTL(LSFT(code))

#define SLT(kc) (QK_LSFT | QK_LALT | (kc))
#define ALT_TAB LALT(KC_TAB)
#define SLT_TAB SLT(KC_TAB)

enum operating_systems {
  OS_MACOS = 1,
  OS_WINDOWS,
} zored_os = OS_WINDOWS;

uint8_t map_windows_keycode (uint8_t windowsKeycode) {
  switch (zored_os) {
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

// Helper functions:


enum do_command {
  DO_FIND_BEGIN = 1,
  DO_FIND_END,
  DO_ENPASS,
  DO_MAIL,
  DO_LOGIN,
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
};

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
    case DO_NEXT_LANGUAGE:
      ; // empty statement.
      uint8_t hold = 0;
      uint8_t tap = 0;
      uint32_t timeout = 40;

      switch (zored_os) {
        case OS_WINDOWS:
          hold = KC_LALT;
          tap = KC_LSHIFT;
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
      tap_code16(KC_AMPERSAND);
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
    case DO_BOOTLOADER:
      clear_keyboard();
      bootloader_jump();
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
  }
}




void process_combo_event(uint8_t combo_index, bool pressed) {
  if (!pressed) {
    return;
  }

  switch(combo_index) {
    
  }
};

enum layers {
LAYER_DEFAULT = 0
};


enum custom_keycodes {
  ZKC_BTL = SAFE_RANGE,

  // At the end:
  DYNAMIC_MACRO_RANGE,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[LAYER_DEFAULT] = LAYOUT(
  
/* 0 */ KC_1,KC_2,KC_3,
/* 1 */ KC_4,KC_5,KC_6,
/* 2 */ KC_7,KC_8,ZKC_BTL
)
  
};



void matrix_init_user(void) {
  #ifdef STENO_ENABLE
    steno_set_mode(STENO_MODE_GEMINI);
  #endif

}



bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  bool complete = false;

  switch (keycode) {
    case ZKC_BTL:
      if (record->event.pressed) {
        run_advanced(DO_BOOTLOADER);
        complete = true;
      }
      break;

    case UC_M_OS:
      zored_os = OS_MACOS;
      break;

    case UC_M_WC:
      zored_os = OS_WINDOWS;
      break;
  }

  return !complete;
};


