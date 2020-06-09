

#include QMK_KEYBOARD_H
#include "muse.h"
#include "eeprom.h"


#include "action_layer.h"
#include "version.h"

#ifdef STENO_ENABLE
 #include "keymap_steno.h"
#endif

enum operating_systems {
  OS_MACOS = 1,
  OS_WINDOWS,
} zored_os = OS_WINDOWS;


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


  return keycode;
}

void code_down(uint8_t code) {
  register_code(map_code(code));
}

void code_up(uint8_t code) {
  unregister_code(map_code(code));
}

void code_hold_ms(uint8_t code, uint32_t timeout) {
  code_down(KC_CAPSLOCK);
  wait_ms(timeout);
  code_up(KC_CAPSLOCK);
}

// Helper functions:

void code_down_2(uint8_t code1, uint8_t code2) {
  register_code(map_code(code1));
  register_code(map_code(code2));
}

void code_up_2(uint8_t code1, uint8_t code2) {
  unregister_code(map_code(code1));
  unregister_code(map_code(code2));
}

void code_down_3(uint8_t code1, uint8_t code2, uint8_t code3) {
  register_code(map_code(code1));
  register_code(map_code(code2));
  register_code(map_code(code3));
}

void code_up_3(uint8_t code1, uint8_t code2, uint8_t code3) {
  unregister_code(map_code(code1));
  unregister_code(map_code(code2));
  unregister_code(map_code(code3));
}


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
  DO_NEXT_MAPPING,
  DO_ONE_SHOT_CTRL,
  DO_ONE_SHOT_ALT,
  DO_ONE_SHOT_GUI,
  DO_ONE_SHOT_SHIFT,
  DO_PAST, DO_FUTURE,
  DO_PREV_TAB, DO_NEXT_TAB,
  DO_FUTURE_TAB, DO_PAST_TAB,
  DO_FUTURE_APP, DO_PAST_APP,
  DO_FUTURE_WINDOW, DO_PAST_WINDOW,
  DO_MOUSE_SLOW, DO_MOUSE_FAST,
};


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
            appSwitch.hold1 = KC_LCTRL;
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
            appSwitch.hold1 = KC_LCTRL;
            appSwitch.hold2 = KC_LALT;
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
          tap = KC_TILDE;
          break;
        default:
          break;
      }
      break;
    default:
      switch (target) {
        case TARGET_TAB:
          if (past) {
            tap = KC_RIGHT;
          } else {
            tap = KC_LEFT;
          }
          break;
        default:
          break;
      }
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
void appSwitchTimeout(void) {
  if (!appSwitch.active) {
    return;
  }
  if (timer_elapsed(appSwitch.timer) <= APP_SWITCH_TIMEOUT) {
    return;
  }
  appSwitchDeactivate();
}


extern uint8_t mk_max_speed;
void mousekey_on(uint8_t code);
void mousekey_off(uint8_t code);
uint8_t mouseSpeed = 1;
void setMouseSpeed(uint8_t newMouseSpeed) {
  // Turn off:
  if (mouseSpeed == newMouseSpeed || newMouseSpeed == 1) {
    mouseSpeed = 1;
    mousekey_off(KC_MS_ACCEL2);
    mk_max_speed = MOUSEKEY_MAX_SPEED;  
    return;
  }

  mouseSpeed = newMouseSpeed;
  mousekey_on(KC_MS_ACCEL2); // - make mk_max_speed the real mouse speed.

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
          tap_code16(G(S(KC_RBRACKET)));
          break;
        case OS_WINDOWS:
          tap_code16(A(KC_RIGHT));
          break;
      }
    case DO_PREV_TAB:
      switch (zored_os) {
        case OS_MACOS:
          tap_code16(G(S(KC_LBRACKET)));
          break;
        case OS_WINDOWS:
          tap_code16(A(KC_LEFT));
          break;
      }
    case DO_FUTURE:
      switch (zored_os) {
        case OS_MACOS:
          register_code(KC_LGUI);
          tap_code(KC_LBRACKET);
          unregister_code(KC_LGUI);
          break;
        case OS_WINDOWS:
          tap_code16(A(KC_LEFT));
          break;
      }
      break;
    case DO_PAST:
      switch (zored_os) {
        case OS_MACOS:
          register_code(KC_LGUI);
          tap_code(KC_RBRACKET);
          unregister_code(KC_LGUI);
          break;
        case OS_WINDOWS:
          tap_code16(A(KC_LEFT));
          break;
      }
      break;
    case DO_NEXT_MAPPING:
      run_advanced(DO_NEXT_LANGUAGE);  
      break;
    case DO_NEXT_LANGUAGE:
      ; // empty statement for declarations
      uint8_t hold = 0;
      uint8_t tap = 0;
      uint32_t timeout = 50;

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
    case DO_MOUSE_SLOW:
      setMouseSpeed(0);
      break;
    case DO_MOUSE_FAST:
      setMouseSpeed(2);
      break;
  }
}

enum unicode_names {
  EMOJI_1F60A = 0,
  EMOJI_1F601,
  EMOJI_1F604,
  EMOJI_1F602,
  EMOJI_1F61E,
  EMOJI_1F622,
  EMOJI_1F62D,
  EMOJI_1F914,
  EMOJI_1F928,
  EMOJI_1F631,
  EMOJI_1F60F,
  EMOJI_1F60E,
  EMOJI_1F4AA,
  EMOJI_1F525,
  EMOJI_1F44D,
  EMOJI_1F44C,
  EMOJI_1F197,
  EMOJI_1F61A,
  EMOJI_1F60D,
  EMOJI_1F618,
  EMOJI_1F496
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
  [EMOJI_1F60A] 0x1F60A, // 😊
[EMOJI_1F601] 0x1F601, // 😁
[EMOJI_1F604] 0x1F604, // 😄
[EMOJI_1F602] 0x1F602, // 😂
[EMOJI_1F61E] 0x1F61E, // 😞
[EMOJI_1F622] 0x1F622, // 😢
[EMOJI_1F62D] 0x1F62D, // 😭
[EMOJI_1F914] 0x1F914, // 🤔
[EMOJI_1F928] 0x1F928, // 🤨
[EMOJI_1F631] 0x1F631, // 😱
[EMOJI_1F60F] 0x1F60F, // 😏
[EMOJI_1F60E] 0x1F60E, // 😎
[EMOJI_1F4AA] 0x1F4AA, // 💪
[EMOJI_1F525] 0x1F525, // 🔥
[EMOJI_1F44D] 0x1F44D, // 👍
[EMOJI_1F44C] 0x1F44C, // 👌
[EMOJI_1F197] 0x1F197, // 🆗
[EMOJI_1F61A] 0x1F61A, // 😚
[EMOJI_1F60D] 0x1F60D, // 😍
[EMOJI_1F618] 0x1F618, // 😘
[EMOJI_1F496] 0x1F496, // 💖
};


const uint16_t PROGMEM combo_seq__w__e[] = {KC_W, KC_E, COMBO_END};
const uint16_t PROGMEM combo_seq__r__t[] = {KC_R, KC_T, COMBO_END};
const uint16_t PROGMEM combo_seq__y__u[] = {KC_Y, KC_U, COMBO_END};
const uint16_t PROGMEM combo_seq__i__o[] = {KC_I, KC_O, COMBO_END};
const uint16_t PROGMEM combo_seq__s__d[] = {KC_S, KC_D, COMBO_END};
const uint16_t PROGMEM combo_seq__f__g[] = {KC_F, KC_G, COMBO_END};
const uint16_t PROGMEM combo_seq__h__j[] = {KC_H, KC_J, COMBO_END};
const uint16_t PROGMEM combo_seq__k__l[] = {KC_K, KC_L, COMBO_END};
const uint16_t PROGMEM combo_seq__n__m[] = {KC_N, KC_M, COMBO_END};

enum combo_names {
  CMB_SEQ__W__E = 0,
  CMB_SEQ__R__T,
  CMB_SEQ__Y__U,
  CMB_SEQ__I__O,
  CMB_SEQ__S__D,
  CMB_SEQ__F__G,
  CMB_SEQ__H__J,
  CMB_SEQ__K__L,
  CMB_SEQ__N__M
};

combo_t key_combos[COMBO_COUNT] = {
  
    [CMB_SEQ__W__E] = COMBO_ACTION(combo_seq__w__e),
  
    [CMB_SEQ__R__T] = COMBO_ACTION(combo_seq__r__t),
  
    [CMB_SEQ__Y__U] = COMBO_ACTION(combo_seq__y__u),
  
    [CMB_SEQ__I__O] = COMBO_ACTION(combo_seq__i__o),
  
    [CMB_SEQ__S__D] = COMBO_ACTION(combo_seq__s__d),
  
    [CMB_SEQ__F__G] = COMBO_ACTION(combo_seq__f__g),
  
    [CMB_SEQ__H__J] = COMBO_ACTION(combo_seq__h__j),
  
    [CMB_SEQ__K__L] = COMBO_ACTION(combo_seq__k__l),
  
    [CMB_SEQ__N__M] = COMBO_ACTION(combo_seq__n__m),
  
};


void process_combo_event(uint8_t combo_index, bool pressed) {
  if (!pressed) {
    return;
  }

  switch(combo_index) {
    
    case CMB_SEQ__W__E:
      run_advanced(DO_TERMINAL);
      break; 

    case CMB_SEQ__R__T:
      run_advanced(DO_NOT_EQUALS);
      break; 

    case CMB_SEQ__Y__U:
      run_advanced(DO_SCREENSHOT);
      break; 

    case CMB_SEQ__I__O:
      run_advanced(DO_CLOSE);
      break; 

    case CMB_SEQ__S__D:
      run_advanced(DO_EMOJI_PANEL);
      break; 

    case CMB_SEQ__F__G:
      tap_code(KC_PAST);
      break; 

    case CMB_SEQ__H__J:
      run_advanced(DO_UNDERSCORE);
      break; 

    case CMB_SEQ__K__L:
      run_advanced(DO_AMPERSAND);
      break; 

    case CMB_SEQ__N__M:
      run_advanced(DO_ARROW);
      break; 

  }
};

enum layers {
LAYER_DEFAULT = 0,
  LAYER_SYMBOL,
  LAYER_NAVIGATION,
  LAYER_NAVIGATION2,
  LAYER_EMOJI,
  LAYER_HISTORY,
  LAYER_GAME
};

enum dance_keys {
  DANCE_KC_CAPSDANCE = 1,
  DANCE_KC_ADANCE,
  DANCE_KC_SEMICOLONDANCE,
  DANCE_KC_QUOTEDANCE,
  DANCE_KC_ZDANCE,
  DANCE_KC_XDANCE,
  DANCE_KC_CDANCE,
  DANCE_KC_COMMADANCE,
  DANCE_KC_DOTDANCE,
  DANCE_KC_SLASHDANCE,
  DANCE_KC_TABDANCE,
  DANCE_KC_PERCENTDANCE,
  DANCE_KC_CIRCUMFLEXDANCE,
  DANCE_KC_LEFTSQUAREBRACKETDANCE,
  DANCE_KC_HAPPYDANCE,
  DANCE_KC_SADDANCE,
  DANCE_KC_STRANGEDANCE,
  DANCE_KC_F7DANCE,
  DANCE_KC_F8DANCE,
  DANCE_KC_F9DANCE,
  DANCE_KC_COOLDANCE,
  DANCE_KC_OKDANCE,
  DANCE_KC_LOVEDANCE,
  DANCE_KC_F4DANCE,
  DANCE_KC_F5DANCE,
  DANCE_KC_F6DANCE,
  DANCE_KC_F1DANCE,
  DANCE_KC_F2DANCE,
  DANCE_KC_F3DANCE,
  DANCE_KC_F10DANCE
};
enum dance_action_names {
  ACTION_SEQ__DO_NEXT_LANGUAGE_1 = 1,
  ACTION_SEQ__HOLD_LAYER_SYMBOL_3,
  ACTION_SEQ__HOLD_LAYER_SYMBOL__LCTRL_4,
  ACTION_SEQ__A_6,
  ACTION_SEQ__HOLD_LAYER_HISTORY_8,
  ACTION_SEQ__SCOLON_10,
  ACTION_SEQ__HOLD_LAYER_NAVIGATION_12,
  ACTION_SEQ__HOLD_LAYER_NAVIGATION2_13,
  ACTION_SEQ__TOGGLE_LAYER_NAVIGATION_14,
  ACTION_SEQ__QUOT_16,
  ACTION_SEQ__HOLD_LAYER_EMOJI_18,
  ACTION_SEQ__HOLD_LAYER_EMOJI__LCTRL_19,
  ACTION_SEQ__TOGGLE_LAYER_EMOJI_20,
  ACTION_SEQ__Z_22,
  ACTION_SEQ__LCTRL_24,
  ACTION_SEQ__LCTRL__LSHIFT_25,
  ACTION_SEQ__LCTRL__LALT__LGUI_26,
  ACTION_SEQ__X_28,
  ACTION_SEQ__LALT_30,
  ACTION_SEQ__LALT__LSHIFT_31,
  ACTION_SEQ__C_33,
  ACTION_SEQ__LGUI_35,
  ACTION_SEQ__LGUI__LSHIFT_36,
  ACTION_SEQ__COMM_38,
  ACTION_SEQ__RGUI_40,
  ACTION_SEQ__RGUI__RSHIFT_41,
  ACTION_SEQ__DOT_43,
  ACTION_SEQ__RALT_45,
  ACTION_SEQ__RALT__RSHIFT_46,
  ACTION_SEQ__SLSH_48,
  ACTION_SEQ__RCTRL_50,
  ACTION_SEQ__RCTRL__RSHIFT_51,
  ACTION_SEQ__TAB_53,
  ACTION_SEQ__HOLD_LAYER_SYMBOL_55,
  ACTION_SEQ__LSHIFT__5_57,
  ACTION_SEQ__LCTRL_59,
  ACTION_SEQ__LCTRL__LSHIFT_60,
  ACTION_SEQ__LCTRL__LALT__LGUI_61,
  ACTION_SEQ__LSHIFT__6_63,
  ACTION_SEQ__LALT_65,
  ACTION_SEQ__LALT__LSHIFT_66,
  ACTION_SEQ__LBRC_68,
  ACTION_SEQ__LGUI_70,
  ACTION_SEQ__LGUI__LSHIFT_71,
  ACTION_SEQ__1F60A_73,
  ACTION_SEQ__1F601_74,
  ACTION_SEQ__1F604_75,
  ACTION_SEQ__1F602_76,
  ACTION_SEQ__1F61E_79,
  ACTION_SEQ__1F622_80,
  ACTION_SEQ__1F62D_81,
  ACTION_SEQ__1F914_84,
  ACTION_SEQ__1F928_85,
  ACTION_SEQ__1F631_86,
  ACTION_SEQ__F7_89,
  ACTION_SEQ__F17_90,
  ACTION_SEQ__F8_93,
  ACTION_SEQ__F18_94,
  ACTION_SEQ__F9_97,
  ACTION_SEQ__F19_98,
  ACTION_SEQ__1F60F_101,
  ACTION_SEQ__1F60E_102,
  ACTION_SEQ__1F4AA_103,
  ACTION_SEQ__1F525_104,
  ACTION_SEQ__1F44D_107,
  ACTION_SEQ__1F44C_108,
  ACTION_SEQ__1F197_109,
  ACTION_SEQ__1F61A_112,
  ACTION_SEQ__1F60D_113,
  ACTION_SEQ__1F618_114,
  ACTION_SEQ__1F496_115,
  ACTION_SEQ__F4_118,
  ACTION_SEQ__F14_119,
  ACTION_SEQ__F5_122,
  ACTION_SEQ__F15_123,
  ACTION_SEQ__F6_126,
  ACTION_SEQ__F16_127,
  ACTION_SEQ__F1_130,
  ACTION_SEQ__F11_131,
  ACTION_SEQ__F2_134,
  ACTION_SEQ__F12_135,
  ACTION_SEQ__F3_138,
  ACTION_SEQ__F13_139,
  ACTION_SEQ__F10_142,
  ACTION_SEQ__F20_143
};

static int dance_key_states[85] = {0};

void dance_flush_on_many_taps(qk_tap_dance_state_t *state) {
  if (state->count == 0) {
    return;
  }
  uint16_t dance_key = state->keycode - QK_TAP_DANCE;

  switch (dance_key) {
    
    case DANCE_KC_CAPSDANCE:
      if (state->count <= 2) {
        return;
      }
      break;
 

    case DANCE_KC_ADANCE:
      if (state->count <= 1) {
        return;
      }
      break;
 

    case DANCE_KC_SEMICOLONDANCE:
      if (state->count <= 3) {
        return;
      }
      break;
 

    case DANCE_KC_QUOTEDANCE:
      if (state->count <= 3) {
        return;
      }
      break;
 

    case DANCE_KC_ZDANCE:
      if (state->count <= 3) {
        return;
      }
      break;
 

    case DANCE_KC_XDANCE:
      if (state->count <= 2) {
        return;
      }
      break;
 

    case DANCE_KC_CDANCE:
      if (state->count <= 2) {
        return;
      }
      break;
 

    case DANCE_KC_COMMADANCE:
      if (state->count <= 2) {
        return;
      }
      break;
 

    case DANCE_KC_DOTDANCE:
      if (state->count <= 2) {
        return;
      }
      break;
 

    case DANCE_KC_SLASHDANCE:
      if (state->count <= 2) {
        return;
      }
      break;
 

    case DANCE_KC_TABDANCE:
      if (state->count <= 1) {
        return;
      }
      break;
 

    case DANCE_KC_PERCENTDANCE:
      if (state->count <= 3) {
        return;
      }
      break;
 

    case DANCE_KC_CIRCUMFLEXDANCE:
      if (state->count <= 2) {
        return;
      }
      break;
 

    case DANCE_KC_LEFTSQUAREBRACKETDANCE:
      if (state->count <= 2) {
        return;
      }
      break;
 

    case DANCE_KC_HAPPYDANCE:
      if (state->count <= 4) {
        return;
      }
      break;
 

    case DANCE_KC_SADDANCE:
      if (state->count <= 3) {
        return;
      }
      break;
 

    case DANCE_KC_STRANGEDANCE:
      if (state->count <= 3) {
        return;
      }
      break;
 

    case DANCE_KC_F7DANCE:
      if (state->count <= 2) {
        return;
      }
      break;
 

    case DANCE_KC_F8DANCE:
      if (state->count <= 2) {
        return;
      }
      break;
 

    case DANCE_KC_F9DANCE:
      if (state->count <= 2) {
        return;
      }
      break;
 

    case DANCE_KC_COOLDANCE:
      if (state->count <= 4) {
        return;
      }
      break;
 

    case DANCE_KC_OKDANCE:
      if (state->count <= 3) {
        return;
      }
      break;
 

    case DANCE_KC_LOVEDANCE:
      if (state->count <= 4) {
        return;
      }
      break;
 

    case DANCE_KC_F4DANCE:
      if (state->count <= 2) {
        return;
      }
      break;
 

    case DANCE_KC_F5DANCE:
      if (state->count <= 2) {
        return;
      }
      break;
 

    case DANCE_KC_F6DANCE:
      if (state->count <= 2) {
        return;
      }
      break;
 

    case DANCE_KC_F1DANCE:
      if (state->count <= 2) {
        return;
      }
      break;
 

    case DANCE_KC_F2DANCE:
      if (state->count <= 2) {
        return;
      }
      break;
 

    case DANCE_KC_F3DANCE:
      if (state->count <= 2) {
        return;
      }
      break;
 

    case DANCE_KC_F10DANCE:
      if (state->count <= 2) {
        return;
      }
      break;
 
    default:
      return;
  }

  state->timer = 0;
  state->pressed = false;
}

void dance_tap_on_enemy_hold(qk_tap_dance_state_t *state) {
  if (state->count == 0) {
    return;
  }

  uint16_t dance_key = state->keycode - QK_TAP_DANCE;

  switch(dance_key) {
    
// Enemies 0 #0
case DANCE_KC_ZDANCE:
case DANCE_KC_XDANCE:
case DANCE_KC_CDANCE:
    if (
dance_key_states[DANCE_KC_COMMADANCE] == 0 && 
dance_key_states[DANCE_KC_DOTDANCE] == 0 && 
dance_key_states[DANCE_KC_SLASHDANCE] == 0
    ) {
      return;
    }
    break;


// Enemies 0 #1
case DANCE_KC_COMMADANCE:
case DANCE_KC_DOTDANCE:
case DANCE_KC_SLASHDANCE:
    if (
dance_key_states[DANCE_KC_ZDANCE] == 0 && 
dance_key_states[DANCE_KC_XDANCE] == 0 && 
dance_key_states[DANCE_KC_CDANCE] == 0
    ) {
      return;
    }
    break;

    default:
      return;
  }

  state->timer = 0;
  state->pressed = false;
}

void on_dance_each_tap(qk_tap_dance_state_t *state, void *user_data) {
  dance_tap_on_enemy_hold(state);
  dance_flush_on_many_taps(state);
}

void on_dance_finished(qk_tap_dance_state_t *state, void *user_data) {
  if (state->count == 0) {
    return;
  }
  uint16_t dance_key = state->keycode - QK_TAP_DANCE;

  switch (dance_key) {
    
    case DANCE_KC_CAPSDANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            layer_on(LAYER_SYMBOL);
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_SYMBOL_3;
            return;
      
          case 2:
            layer_on(LAYER_SYMBOL);
    code_down(KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_SYMBOL__LCTRL_4;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            dance_key_states[dance_key] = ACTION_SEQ__DO_NEXT_LANGUAGE_1;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              dance_key_states[dance_key] = ACTION_SEQ__DO_NEXT_LANGUAGE_1;
              run_advanced(DO_NEXT_LANGUAGE);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_ADANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            layer_on(LAYER_HISTORY);
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_HISTORY_8;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_A);
            dance_key_states[dance_key] = ACTION_SEQ__A_6;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_A);
            dance_key_states[dance_key] = ACTION_SEQ__A_6;
              code_up(KC_A);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_SEMICOLONDANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            layer_on(LAYER_NAVIGATION);
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_NAVIGATION_12;
            return;
      
          case 2:
            layer_on(LAYER_NAVIGATION2);
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_NAVIGATION2_13;
            return;
      
          case 3:
            layer_invert(LAYER_NAVIGATION);
            dance_key_states[dance_key] = ACTION_SEQ__TOGGLE_LAYER_NAVIGATION_14;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_SCOLON);
            dance_key_states[dance_key] = ACTION_SEQ__SCOLON_10;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_SCOLON);
            dance_key_states[dance_key] = ACTION_SEQ__SCOLON_10;
              code_up(KC_SCOLON);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_QUOTEDANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            layer_on(LAYER_EMOJI);
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_EMOJI_18;
            return;
      
          case 2:
            layer_on(LAYER_EMOJI);
    code_down(KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_EMOJI__LCTRL_19;
            return;
      
          case 3:
            layer_invert(LAYER_EMOJI);
            dance_key_states[dance_key] = ACTION_SEQ__TOGGLE_LAYER_EMOJI_20;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_QUOT);
            dance_key_states[dance_key] = ACTION_SEQ__QUOT_16;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_QUOT);
            dance_key_states[dance_key] = ACTION_SEQ__QUOT_16;
              code_up(KC_QUOT);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_ZDANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            code_down(KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL_24;
            return;
      
          case 2:
            code_down_2(KC_LCTRL, KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL__LSHIFT_25;
            return;
      
          case 3:
            code_down_3(KC_LCTRL, KC_LALT, KC_LGUI);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL__LALT__LGUI_26;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_Z);
            dance_key_states[dance_key] = ACTION_SEQ__Z_22;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_Z);
            dance_key_states[dance_key] = ACTION_SEQ__Z_22;
              code_up(KC_Z);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_XDANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            code_down(KC_LALT);
            dance_key_states[dance_key] = ACTION_SEQ__LALT_30;
            return;
      
          case 2:
            code_down_2(KC_LALT, KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LALT__LSHIFT_31;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_X);
            dance_key_states[dance_key] = ACTION_SEQ__X_28;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_X);
            dance_key_states[dance_key] = ACTION_SEQ__X_28;
              code_up(KC_X);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_CDANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            code_down(KC_LGUI);
            dance_key_states[dance_key] = ACTION_SEQ__LGUI_35;
            return;
      
          case 2:
            code_down_2(KC_LGUI, KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LGUI__LSHIFT_36;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_C);
            dance_key_states[dance_key] = ACTION_SEQ__C_33;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_C);
            dance_key_states[dance_key] = ACTION_SEQ__C_33;
              code_up(KC_C);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_COMMADANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            code_down(KC_RGUI);
            dance_key_states[dance_key] = ACTION_SEQ__RGUI_40;
            return;
      
          case 2:
            code_down_2(KC_RGUI, KC_RSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__RGUI__RSHIFT_41;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_COMM);
            dance_key_states[dance_key] = ACTION_SEQ__COMM_38;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_COMM);
            dance_key_states[dance_key] = ACTION_SEQ__COMM_38;
              code_up(KC_COMM);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_DOTDANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            code_down(KC_RALT);
            dance_key_states[dance_key] = ACTION_SEQ__RALT_45;
            return;
      
          case 2:
            code_down_2(KC_RALT, KC_RSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__RALT__RSHIFT_46;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_DOT);
            dance_key_states[dance_key] = ACTION_SEQ__DOT_43;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_DOT);
            dance_key_states[dance_key] = ACTION_SEQ__DOT_43;
              code_up(KC_DOT);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_SLASHDANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            code_down(KC_RCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__RCTRL_50;
            return;
      
          case 2:
            code_down_2(KC_RCTRL, KC_RSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__RCTRL__RSHIFT_51;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_SLSH);
            dance_key_states[dance_key] = ACTION_SEQ__SLSH_48;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_SLSH);
            dance_key_states[dance_key] = ACTION_SEQ__SLSH_48;
              code_up(KC_SLSH);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_TABDANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            layer_on(LAYER_SYMBOL);
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_SYMBOL_55;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_TAB);
            dance_key_states[dance_key] = ACTION_SEQ__TAB_53;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_TAB);
            dance_key_states[dance_key] = ACTION_SEQ__TAB_53;
              code_up(KC_TAB);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_PERCENTDANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            code_down(KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL_59;
            return;
      
          case 2:
            code_down_2(KC_LCTRL, KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL__LSHIFT_60;
            return;
      
          case 3:
            code_down_3(KC_LCTRL, KC_LALT, KC_LGUI);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL__LALT__LGUI_61;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down_2(KC_LSHIFT, KC_5);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__5_57;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down_2(KC_LSHIFT, KC_5);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__5_57;
              code_up_2(KC_LSHIFT, KC_5);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_CIRCUMFLEXDANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            code_down(KC_LALT);
            dance_key_states[dance_key] = ACTION_SEQ__LALT_65;
            return;
      
          case 2:
            code_down_2(KC_LALT, KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LALT__LSHIFT_66;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down_2(KC_LSHIFT, KC_6);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__6_63;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down_2(KC_LSHIFT, KC_6);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__6_63;
              code_up_2(KC_LSHIFT, KC_6);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_LEFTSQUAREBRACKETDANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            code_down(KC_LGUI);
            dance_key_states[dance_key] = ACTION_SEQ__LGUI_70;
            return;
      
          case 2:
            code_down_2(KC_LGUI, KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LGUI__LSHIFT_71;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_LBRC);
            dance_key_states[dance_key] = ACTION_SEQ__LBRC_68;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_LBRC);
            dance_key_states[dance_key] = ACTION_SEQ__LBRC_68;
              code_up(KC_LBRC);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_HAPPYDANCE:
      if (state->pressed) {
        // Hold actions:
        return;
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            tap_unicode(EMOJI_1F60A);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F60A_73;
            return;
      
          case 2:
            tap_unicode(EMOJI_1F601);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F601_74;
            return;
      
          case 3:
            tap_unicode(EMOJI_1F604);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F604_75;
            return;
      
          case 4:
            tap_unicode(EMOJI_1F602);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F602_76;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              tap_unicode(EMOJI_1F60A);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F60A_73;
              
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_SADDANCE:
      if (state->pressed) {
        // Hold actions:
        return;
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            tap_unicode(EMOJI_1F61E);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F61E_79;
            return;
      
          case 2:
            tap_unicode(EMOJI_1F622);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F622_80;
            return;
      
          case 3:
            tap_unicode(EMOJI_1F62D);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F62D_81;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              tap_unicode(EMOJI_1F61E);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F61E_79;
              
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_STRANGEDANCE:
      if (state->pressed) {
        // Hold actions:
        return;
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            tap_unicode(EMOJI_1F914);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F914_84;
            return;
      
          case 2:
            tap_unicode(EMOJI_1F928);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F928_85;
            return;
      
          case 3:
            tap_unicode(EMOJI_1F631);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F631_86;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              tap_unicode(EMOJI_1F914);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F914_84;
              
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_F7DANCE:
      if (state->pressed) {
        // Hold actions:
        return;
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_F7);
            dance_key_states[dance_key] = ACTION_SEQ__F7_89;
            return;
      
          case 2:
            code_down(KC_F17);
            dance_key_states[dance_key] = ACTION_SEQ__F17_90;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F7);
            dance_key_states[dance_key] = ACTION_SEQ__F7_89;
              code_up(KC_F7);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_F8DANCE:
      if (state->pressed) {
        // Hold actions:
        return;
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_F8);
            dance_key_states[dance_key] = ACTION_SEQ__F8_93;
            return;
      
          case 2:
            code_down(KC_F18);
            dance_key_states[dance_key] = ACTION_SEQ__F18_94;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F8);
            dance_key_states[dance_key] = ACTION_SEQ__F8_93;
              code_up(KC_F8);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_F9DANCE:
      if (state->pressed) {
        // Hold actions:
        return;
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_F9);
            dance_key_states[dance_key] = ACTION_SEQ__F9_97;
            return;
      
          case 2:
            code_down(KC_F19);
            dance_key_states[dance_key] = ACTION_SEQ__F19_98;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F9);
            dance_key_states[dance_key] = ACTION_SEQ__F9_97;
              code_up(KC_F9);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_COOLDANCE:
      if (state->pressed) {
        // Hold actions:
        return;
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            tap_unicode(EMOJI_1F60F);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F60F_101;
            return;
      
          case 2:
            tap_unicode(EMOJI_1F60E);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F60E_102;
            return;
      
          case 3:
            tap_unicode(EMOJI_1F4AA);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F4AA_103;
            return;
      
          case 4:
            tap_unicode(EMOJI_1F525);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F525_104;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              tap_unicode(EMOJI_1F60F);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F60F_101;
              
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_OKDANCE:
      if (state->pressed) {
        // Hold actions:
        return;
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            tap_unicode(EMOJI_1F44D);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F44D_107;
            return;
      
          case 2:
            tap_unicode(EMOJI_1F44C);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F44C_108;
            return;
      
          case 3:
            tap_unicode(EMOJI_1F197);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F197_109;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              tap_unicode(EMOJI_1F44D);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F44D_107;
              
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_LOVEDANCE:
      if (state->pressed) {
        // Hold actions:
        return;
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            tap_unicode(EMOJI_1F61A);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F61A_112;
            return;
      
          case 2:
            tap_unicode(EMOJI_1F60D);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F60D_113;
            return;
      
          case 3:
            tap_unicode(EMOJI_1F618);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F618_114;
            return;
      
          case 4:
            tap_unicode(EMOJI_1F496);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F496_115;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              tap_unicode(EMOJI_1F61A);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F61A_112;
              
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_F4DANCE:
      if (state->pressed) {
        // Hold actions:
        return;
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_F4);
            dance_key_states[dance_key] = ACTION_SEQ__F4_118;
            return;
      
          case 2:
            code_down(KC_F14);
            dance_key_states[dance_key] = ACTION_SEQ__F14_119;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F4);
            dance_key_states[dance_key] = ACTION_SEQ__F4_118;
              code_up(KC_F4);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_F5DANCE:
      if (state->pressed) {
        // Hold actions:
        return;
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_F5);
            dance_key_states[dance_key] = ACTION_SEQ__F5_122;
            return;
      
          case 2:
            code_down(KC_F15);
            dance_key_states[dance_key] = ACTION_SEQ__F15_123;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F5);
            dance_key_states[dance_key] = ACTION_SEQ__F5_122;
              code_up(KC_F5);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_F6DANCE:
      if (state->pressed) {
        // Hold actions:
        return;
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_F6);
            dance_key_states[dance_key] = ACTION_SEQ__F6_126;
            return;
      
          case 2:
            code_down(KC_F16);
            dance_key_states[dance_key] = ACTION_SEQ__F16_127;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F6);
            dance_key_states[dance_key] = ACTION_SEQ__F6_126;
              code_up(KC_F6);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_F1DANCE:
      if (state->pressed) {
        // Hold actions:
        return;
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_F1);
            dance_key_states[dance_key] = ACTION_SEQ__F1_130;
            return;
      
          case 2:
            code_down(KC_F11);
            dance_key_states[dance_key] = ACTION_SEQ__F11_131;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F1);
            dance_key_states[dance_key] = ACTION_SEQ__F1_130;
              code_up(KC_F1);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_F2DANCE:
      if (state->pressed) {
        // Hold actions:
        return;
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_F2);
            dance_key_states[dance_key] = ACTION_SEQ__F2_134;
            return;
      
          case 2:
            code_down(KC_F12);
            dance_key_states[dance_key] = ACTION_SEQ__F12_135;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F2);
            dance_key_states[dance_key] = ACTION_SEQ__F2_134;
              code_up(KC_F2);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_F3DANCE:
      if (state->pressed) {
        // Hold actions:
        return;
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_F3);
            dance_key_states[dance_key] = ACTION_SEQ__F3_138;
            return;
      
          case 2:
            code_down(KC_F13);
            dance_key_states[dance_key] = ACTION_SEQ__F13_139;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F3);
            dance_key_states[dance_key] = ACTION_SEQ__F3_138;
              code_up(KC_F3);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_F10DANCE:
      if (state->pressed) {
        // Hold actions:
        return;
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_F10);
            dance_key_states[dance_key] = ACTION_SEQ__F10_142;
            return;
      
          case 2:
            code_down(KC_F20);
            dance_key_states[dance_key] = ACTION_SEQ__F20_143;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F10);
            dance_key_states[dance_key] = ACTION_SEQ__F10_142;
              code_up(KC_F10);
            }
            return;
        }
      }
      break;
    
  }
}

void on_dance_reset(qk_tap_dance_state_t *state, void *user_data) {
  uint16_t dance_key = state->keycode - QK_TAP_DANCE;
  switch (dance_key_states[dance_key]) {
    case ACTION_SEQ__HOLD_LAYER_SYMBOL_3:
            layer_off(LAYER_SYMBOL);
            break;
    
          case ACTION_SEQ__HOLD_LAYER_SYMBOL__LCTRL_4:
            layer_off(LAYER_SYMBOL);code_up(KC_LCTRL);
            break;
        case ACTION_SEQ__DO_NEXT_LANGUAGE_1:
            run_advanced(DO_NEXT_LANGUAGE);
            break;
    
        case ACTION_SEQ__HOLD_LAYER_HISTORY_8:
            layer_off(LAYER_HISTORY);
            break;
        case ACTION_SEQ__A_6:
            code_up(KC_A);
            break;
    
        case ACTION_SEQ__HOLD_LAYER_NAVIGATION_12:
            layer_off(LAYER_NAVIGATION);
            break;
    
          case ACTION_SEQ__HOLD_LAYER_NAVIGATION2_13:
            layer_off(LAYER_NAVIGATION2);
            break;
        case ACTION_SEQ__SCOLON_10:
            code_up(KC_SCOLON);
            break;
    
        case ACTION_SEQ__HOLD_LAYER_EMOJI_18:
            layer_off(LAYER_EMOJI);
            break;
    
          case ACTION_SEQ__HOLD_LAYER_EMOJI__LCTRL_19:
            layer_off(LAYER_EMOJI);code_up(KC_LCTRL);
            break;
        case ACTION_SEQ__QUOT_16:
            code_up(KC_QUOT);
            break;
    
        case ACTION_SEQ__LCTRL_24:
            code_up(KC_LCTRL);
            break;
    
          case ACTION_SEQ__LCTRL__LSHIFT_25:
            code_up_2(KC_LCTRL, KC_LSHIFT);
            break;
    
          case ACTION_SEQ__LCTRL__LALT__LGUI_26:
            code_up_3(KC_LCTRL, KC_LALT, KC_LGUI);
            break;
        case ACTION_SEQ__Z_22:
            code_up(KC_Z);
            break;
    
        case ACTION_SEQ__LALT_30:
            code_up(KC_LALT);
            break;
    
          case ACTION_SEQ__LALT__LSHIFT_31:
            code_up_2(KC_LALT, KC_LSHIFT);
            break;
        case ACTION_SEQ__X_28:
            code_up(KC_X);
            break;
    
        case ACTION_SEQ__LGUI_35:
            code_up(KC_LGUI);
            break;
    
          case ACTION_SEQ__LGUI__LSHIFT_36:
            code_up_2(KC_LGUI, KC_LSHIFT);
            break;
        case ACTION_SEQ__C_33:
            code_up(KC_C);
            break;
    
        case ACTION_SEQ__RGUI_40:
            code_up(KC_RGUI);
            break;
    
          case ACTION_SEQ__RGUI__RSHIFT_41:
            code_up_2(KC_RGUI, KC_RSHIFT);
            break;
        case ACTION_SEQ__COMM_38:
            code_up(KC_COMM);
            break;
    
        case ACTION_SEQ__RALT_45:
            code_up(KC_RALT);
            break;
    
          case ACTION_SEQ__RALT__RSHIFT_46:
            code_up_2(KC_RALT, KC_RSHIFT);
            break;
        case ACTION_SEQ__DOT_43:
            code_up(KC_DOT);
            break;
    
        case ACTION_SEQ__RCTRL_50:
            code_up(KC_RCTRL);
            break;
    
          case ACTION_SEQ__RCTRL__RSHIFT_51:
            code_up_2(KC_RCTRL, KC_RSHIFT);
            break;
        case ACTION_SEQ__SLSH_48:
            code_up(KC_SLSH);
            break;
    
        case ACTION_SEQ__HOLD_LAYER_SYMBOL_55:
            layer_off(LAYER_SYMBOL);
            break;
        case ACTION_SEQ__TAB_53:
            code_up(KC_TAB);
            break;
    
        case ACTION_SEQ__LCTRL_59:
            code_up(KC_LCTRL);
            break;
    
          case ACTION_SEQ__LCTRL__LSHIFT_60:
            code_up_2(KC_LCTRL, KC_LSHIFT);
            break;
    
          case ACTION_SEQ__LCTRL__LALT__LGUI_61:
            code_up_3(KC_LCTRL, KC_LALT, KC_LGUI);
            break;
        case ACTION_SEQ__LSHIFT__5_57:
            code_up_2(KC_LSHIFT, KC_5);
            break;
    
        case ACTION_SEQ__LALT_65:
            code_up(KC_LALT);
            break;
    
          case ACTION_SEQ__LALT__LSHIFT_66:
            code_up_2(KC_LALT, KC_LSHIFT);
            break;
        case ACTION_SEQ__LSHIFT__6_63:
            code_up_2(KC_LSHIFT, KC_6);
            break;
    
        case ACTION_SEQ__LGUI_70:
            code_up(KC_LGUI);
            break;
    
          case ACTION_SEQ__LGUI__LSHIFT_71:
            code_up_2(KC_LGUI, KC_LSHIFT);
            break;
        case ACTION_SEQ__LBRC_68:
            code_up(KC_LBRC);
            break;
    
        
        case ACTION_SEQ__F7_89:
            code_up(KC_F7);
            break;
    
          case ACTION_SEQ__F17_90:
            code_up(KC_F17);
            break;
    
        
        case ACTION_SEQ__F8_93:
            code_up(KC_F8);
            break;
    
          case ACTION_SEQ__F18_94:
            code_up(KC_F18);
            break;
    
        
        case ACTION_SEQ__F9_97:
            code_up(KC_F9);
            break;
    
          case ACTION_SEQ__F19_98:
            code_up(KC_F19);
            break;
    
        
        case ACTION_SEQ__F4_118:
            code_up(KC_F4);
            break;
    
          case ACTION_SEQ__F14_119:
            code_up(KC_F14);
            break;
    
        
        case ACTION_SEQ__F5_122:
            code_up(KC_F5);
            break;
    
          case ACTION_SEQ__F15_123:
            code_up(KC_F15);
            break;
    
        
        case ACTION_SEQ__F6_126:
            code_up(KC_F6);
            break;
    
          case ACTION_SEQ__F16_127:
            code_up(KC_F16);
            break;
    
        
        case ACTION_SEQ__F1_130:
            code_up(KC_F1);
            break;
    
          case ACTION_SEQ__F11_131:
            code_up(KC_F11);
            break;
    
        
        case ACTION_SEQ__F2_134:
            code_up(KC_F2);
            break;
    
          case ACTION_SEQ__F12_135:
            code_up(KC_F12);
            break;
    
        
        case ACTION_SEQ__F3_138:
            code_up(KC_F3);
            break;
    
          case ACTION_SEQ__F13_139:
            code_up(KC_F13);
            break;
    
        
        case ACTION_SEQ__F10_142:
            code_up(KC_F10);
            break;
    
          case ACTION_SEQ__F20_143:
            code_up(KC_F20);
            break;
    
  }

  dance_key_states[dance_key] = 0;
}


#define ACTION_TAP_DANCE_DOUBLE_TIME(kc1, kc2, tap_specific_tapping_term) { \
    .fn = { qk_tap_dance_pair_on_each_tap, qk_tap_dance_pair_finished, qk_tap_dance_pair_reset }, \
    .user_data = (void *)&((qk_tap_dance_pair_t) { kc1, kc2 }),  \
    .custom_tapping_term = tap_specific_tapping_term, \
  }
#define DANCE_MODIFIER() ACTION_TAP_DANCE_FN_ADVANCED_TIME(on_dance_each_tap, on_dance_finished, on_dance_reset, TAPPING_TERM_TAP_DANCE)
#define DANCE_TWO(k1,k11) ACTION_TAP_DANCE_DOUBLE_TIME(k1, k11, TAPPING_TERM_TAP_DANCE)

qk_tap_dance_action_t tap_dance_actions[] = {
    [DANCE_KC_CAPSDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_ADANCE] = DANCE_MODIFIER(),
  [DANCE_KC_SEMICOLONDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_QUOTEDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_ZDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_XDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_CDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_COMMADANCE] = DANCE_MODIFIER(),
  [DANCE_KC_DOTDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_SLASHDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_TABDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_PERCENTDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_CIRCUMFLEXDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_LEFTSQUAREBRACKETDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_HAPPYDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_SADDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_STRANGEDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F7DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F8DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F9DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_COOLDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_OKDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_LOVEDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F4DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F5DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F6DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F1DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F2DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F3DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F10DANCE] = DANCE_MODIFIER()
};

enum custom_keycodes {
  ZKC_BTL = SAFE_RANGE,
  KC_DO_NEXT_LANGUAGE,
KC_DO_BOOTLOADER,
KC_DO_FUTURE_APP,
KC_DO_FUTURE_TAB,
KC_DO_PAST_TAB,
KC_DO_PAST_APP,
KC_DO_FUTURE_WINDOW,
KC_DO_FUTURE,
KC_DO_PAST,
KC_DO_PAST_WINDOW,


  // At the end:
  DYNAMIC_MACRO_RANGE,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[LAYER_DEFAULT] = LAYOUT_planck_grid(
  
/* 0 */ KC_ESC,KC_Q,KC_W,KC_E,KC_R,KC_T,KC_Y,KC_U,KC_I,KC_O,KC_P,KC_BSLS,
/* 1 */ TD(DANCE_KC_CAPSDANCE),TD(DANCE_KC_ADANCE),KC_S,KC_D,KC_F,KC_G,KC_H,KC_J,KC_K,KC_L,TD(DANCE_KC_SEMICOLONDANCE),TD(DANCE_KC_QUOTEDANCE),
/* 2 */ KC_LSPO,TD(DANCE_KC_ZDANCE),TD(DANCE_KC_XDANCE),TD(DANCE_KC_CDANCE),KC_V,KC_B,KC_N,KC_M,TD(DANCE_KC_COMMADANCE),TD(DANCE_KC_DOTDANCE),TD(DANCE_KC_SLASHDANCE),KC_RSPC,
/* 3 */ KC_LEAD,TG(LAYER_NAVIGATION),_______,KC_DELETE,KC_BSPC,KC_SPC,_______,TD(DANCE_KC_TABDANCE),KC_ENT,KC_LBRC,KC_RBRC,TG(LAYER_GAME)
)
  ,

[LAYER_SYMBOL] = LAYOUT_planck_grid(
  
/* 0 */ _______,KC_EXLM,KC_HASH,KC_LCBR,KC_RCBR,KC_SLSH,KC_EQL,KC_7,KC_8,KC_9,KC_AMPR,KC_PIPE,
/* 1 */ _______,KC_AT,KC_DLR,KC_LPRN,KC_RPRN,KC_GRV,KC_PPLS,KC_4,KC_5,KC_6,KC_PAST,_______,
/* 2 */ _______,TD(DANCE_KC_PERCENTDANCE),TD(DANCE_KC_CIRCUMFLEXDANCE),TD(DANCE_KC_LEFTSQUAREBRACKETDANCE),KC_RBRC,KC_TILD,KC_MINS,KC_1,KC_2,KC_3,KC_SLSH,_______,
/* 3 */ _______,_______,_______,_______,_______,_______,_______,_______,KC_0,KC_DOT,_______,_______
)
  ,

[LAYER_NAVIGATION] = LAYOUT_planck_grid(
  
/* 0 */ _______,_______,KC_BTN2,KC_MS_U,KC_BTN1,_______,_______,KC_BRID,KC_BRIU,_______,_______,_______,
/* 1 */ _______,_______,KC_MS_L,KC_MS_D,KC_MS_R,_______,KC_LEFT,KC_DOWN,KC_UP,KC_RGHT,_______,_______,
/* 2 */ _______,_______,_______,_______,_______,_______,KC_WH_L,KC_WH_D,KC_WH_U,KC_WH_R,_______,_______,
/* 3 */ _______,_______,_______,_______,_______,_______,_______,KC_VOLD,KC_VOLU,KC_MUTE,_______,_______
)
  ,

[LAYER_NAVIGATION2] = LAYOUT_planck_grid(
  
/* 0 */ _______,RGB_VAD,RGB_VAI,RGB_SAD,RGB_SAI,_______,KC_SYSTEM_POWER,KC_SYSTEM_SLEEP,UC_M_WC,UC_M_OS,_______,_______,
/* 1 */ _______,CK_TOGG,CK_UP,CK_DOWN,_______,_______,KC_HOME,KC_PGDN,KC_PGUP,KC_END,_______,_______,
/* 2 */ _______,MU_TOG,MU_MOD,_______,_______,_______,_______,KC_MEDIA_PREV_TRACK,_______,KC_MEDIA_PLAY_PAUSE,KC_MEDIA_NEXT_TRACK,_______,
/* 3 */ _______,_______,_______,_______,_______,_______,_______,RGB_HUD,RGB_HUI,RGB_TOG,RGB_MOD,_______
)
  ,

[LAYER_EMOJI] = LAYOUT_planck_grid(
  
/* 0 */ _______,TD(DANCE_KC_HAPPYDANCE),TD(DANCE_KC_SADDANCE),TD(DANCE_KC_STRANGEDANCE),_______,_______,_______,TD(DANCE_KC_F7DANCE),TD(DANCE_KC_F8DANCE),TD(DANCE_KC_F9DANCE),_______,_______,
/* 1 */ _______,TD(DANCE_KC_COOLDANCE),TD(DANCE_KC_OKDANCE),TD(DANCE_KC_LOVEDANCE),_______,_______,_______,TD(DANCE_KC_F4DANCE),TD(DANCE_KC_F5DANCE),TD(DANCE_KC_F6DANCE),_______,_______,
/* 2 */ _______,_______,_______,_______,_______,_______,_______,TD(DANCE_KC_F1DANCE),TD(DANCE_KC_F2DANCE),TD(DANCE_KC_F3DANCE),_______,_______,
/* 3 */ KC_DO_BOOTLOADER,_______,_______,_______,_______,_______,_______,_______,TD(DANCE_KC_F10DANCE),_______,_______,_______
)
  ,

[LAYER_HISTORY] = LAYOUT_planck_grid(
  
/* 0 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 1 */ _______,_______,KC_HOME,KC_PGDN,KC_PGUP,KC_END,KC_DO_FUTURE_APP,KC_DO_FUTURE_TAB,KC_DO_PAST_TAB,KC_DO_PAST_APP,_______,_______,
/* 2 */ _______,_______,_______,_______,_______,_______,KC_DO_FUTURE_WINDOW,KC_DO_FUTURE,KC_DO_PAST,KC_DO_PAST_WINDOW,_______,_______,
/* 3 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______
)
  ,

[LAYER_GAME] = LAYOUT_planck_grid(
  
/* 0 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,KC_UP,_______,_______,
/* 1 */ KC_TAB,_______,_______,_______,_______,_______,_______,_______,KC_LEFT,KC_DOWN,KC_RGHT,_______,
/* 2 */ KC_LSHIFT,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 3 */ KC_LCTRL,KC_LALT,KC_LGUI,_______,_______,_______,_______,_______,KC_RGUI,KC_RALT,KC_RCTRL,_______
)
  
};


void planck_ez_led_all_off(void) {
  planck_ez_left_led_off();
  planck_ez_right_led_off();
}

void matrix_init_user(void) {
  #ifdef STENO_ENABLE
    steno_set_mode(STENO_MODE_GEMINI);
  #endif

  planck_ez_right_led_level(64);
  planck_ez_left_led_level(64);
  planck_ez_led_all_off();
  clicky_off(); // TODO: maybe there is some ENV for that?
}

LEADER_EXTERNS();
void matrix_scan_user(void) {
  appSwitchTimeout();
  LEADER_DICTIONARY() {
    leading = false;
    leader_end();
    SEQ_ONE_KEY(KC_U) {
      run_advanced(DO_LOGIN);
    }
    SEQ_ONE_KEY(KC_P) {
      run_advanced(DO_ENPASS);
    }
    SEQ_ONE_KEY(KC_S) {
      run_advanced(DO_SCREENSHOT);
    }
  }
}

void keyboard_post_init_user(void) {
  switch (get_unicode_input_mode()) {
    case UC_OSX:
      zored_os = OS_MACOS;
  }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  bool pressed = record->event.pressed;


  if (!pressed) {
    return true;
  }

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

  switch (keycode) {
    case UC_M_OS:
      zored_os = OS_MACOS;
      break;

    case UC_M_WC:
      zored_os = OS_WINDOWS;
      break;

    // Do keys:

      case KC_DO_NEXT_LANGUAGE:
        run_advanced(DO_NEXT_LANGUAGE);
        break;
  
      case KC_DO_BOOTLOADER:
        run_advanced(DO_BOOTLOADER);
        break;
  
      case KC_DO_FUTURE_APP:
        run_advanced(DO_FUTURE_APP);
        break;
  
      case KC_DO_FUTURE_TAB:
        run_advanced(DO_FUTURE_TAB);
        break;
  
      case KC_DO_PAST_TAB:
        run_advanced(DO_PAST_TAB);
        break;
  
      case KC_DO_PAST_APP:
        run_advanced(DO_PAST_APP);
        break;
  
      case KC_DO_FUTURE_WINDOW:
        run_advanced(DO_FUTURE_WINDOW);
        break;
  
      case KC_DO_FUTURE:
        run_advanced(DO_FUTURE);
        break;
  
      case KC_DO_PAST:
        run_advanced(DO_PAST);
        break;
  
      case KC_DO_PAST_WINDOW:
        run_advanced(DO_PAST_WINDOW);
        break;
  

    default:
      return true;
  }

  return false;
}


uint32_t layer_state_set_user(uint32_t state) {
  uint8_t layer = biton32(state);

  planck_ez_led_all_off();
  switch (layer) {
    
      case LAYER_DEFAULT:
        combo_enable(); 
        break;
    
      case LAYER_SYMBOL:
        combo_disable(); planck_ez_left_led_on();
        break;
    
      case LAYER_NAVIGATION:
        combo_disable(); planck_ez_right_led_on();
        break;
    
      case LAYER_NAVIGATION2:
        combo_disable(); planck_ez_left_led_on();
        break;
    
      case LAYER_EMOJI:
        combo_disable(); planck_ez_left_led_on(); planck_ez_right_led_on();
        break;
    
      case LAYER_HISTORY:
        combo_disable(); planck_ez_left_led_on(); planck_ez_right_led_on();
        break;
    
      case LAYER_GAME:
        combo_disable(); planck_ez_right_led_on();
        break;
    
  }

  return state;
};
