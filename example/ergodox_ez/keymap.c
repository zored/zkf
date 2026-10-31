
#include QMK_KEYBOARD_H

#include "action_layer.h"
#include "version.h"

#ifdef STENO_ENABLE
 #include "keymap_steno.h"
#endif

enum custom_keycodes {
  ZKC_FIRST = SAFE_RANGE,
  KC_DO_LAYOUT_EN,
KC_DO_LAYOUT_RU,
KC_DO_FUTURE_APP,
KC_DO_COPY,
KC_DO_PASTE,
KC_DO_ONE_SHOT_TURBO,
KC_DO_PAST_APP,
KC_DO_PAST_WINDOW,
KC_DO_FUTURE_WINDOW,
KC_DO_PAST,
KC_DO_FUTURE,
KC_DO_BOOTLOADER,
KC_DO_FUTURE_TAB,
KC_DO_PAST_TAB,

};

enum operating_systems {
  ZOS_MACOS = 1,
  ZOS_WINDOWS,
} zored_os = ZOS_WINDOWS;


bool process_detected_host_os_user(os_variant_t detected_os) {
  switch (detected_os) {
    case OS_MACOS:
    case OS_IOS:
      zored_os = ZOS_MACOS;
      break;
    case OS_WINDOWS:
    case OS_LINUX:
      zored_os = ZOS_WINDOWS;
      break;
    default:
      return true;
  }
#ifdef UNICODE_COMMON_ENABLE
  set_unicode_input_mode(zored_os == ZOS_MACOS ? UNICODE_MODE_MACOS : UNICODE_MODE_WINCOMPOSE);
#endif
  return true;
}


uint8_t map_mod (uint8_t mod) {
  switch (zored_os) {
    case ZOS_MACOS:
      switch (mod) {
        case MOD_LCTL:
          return MOD_LGUI;
        case MOD_LGUI:
          return MOD_LCTL;
      }
    case ZOS_WINDOWS:
      break;
  }

  return mod;
}

void do_one_shot (uint8_t keycode) {
  set_oneshot_mods(map_mod(keycode));
}

uint8_t map_code (uint8_t keycode) {
  switch (zored_os) {
    case ZOS_MACOS:
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
    case ZOS_WINDOWS:
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
  DO_COPY, DO_PASTE,
  DO_TERMINAL,
  DO_SCREENSHOT,
  DO_BOOTLOADER,
  DO_CLOSE,
  DO_NEXT_LANGUAGE,
  DO_LAYOUT_EN,
  DO_LAYOUT_RU,
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
  DO_ONE_SHOT_TURBO,
  DO_PAST, DO_FUTURE,
  DO_PREV_TAB, DO_NEXT_TAB,
  DO_FUTURE_TAB, DO_PAST_TAB,
  DO_FUTURE_APP, DO_PAST_APP,
  DO_FUTURE_WINDOW, DO_PAST_WINDOW,
  DO_MOUSE_SLOW, DO_MOUSE_FAST,
};


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
      case ZOS_MACOS:
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
    case ZOS_MACOS:
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
        case ZOS_WINDOWS:
          tap_code(KC_LGUI);
          break;
        case ZOS_MACOS:
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
    case DO_ONE_SHOT_TURBO:
      turboWaitEnable();
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
        case ZOS_MACOS:
          tap_code16(G(S(KC_RBRC)));
          break;
        case ZOS_WINDOWS:
          tap_code16(C(A(KC_RIGHT)));
          break;
      }
      break;
    case DO_PREV_TAB:
      switch (zored_os) {
        case ZOS_MACOS:
          tap_code16(G(S(KC_LBRC)));
          break;
        case ZOS_WINDOWS:
          tap_code16(C(A(KC_LEFT)));
          break;
      }
      break;
    case DO_FUTURE:
      switch (zored_os) {
        case ZOS_MACOS:
          tap_code16(G(KC_LBRC));
          break;
        case ZOS_WINDOWS:
          tap_code16(A(KC_LEFT));
          break;
      }
      break;
    case DO_PAST:
      switch (zored_os) {
        case ZOS_MACOS:
          tap_code16(G(KC_RBRC));
          break;
        case ZOS_WINDOWS:
          tap_code16(A(KC_RIGHT));
          break;
      }
      break;
    case DO_NEXT_MAPPING:
      run_advanced(DO_NEXT_LANGUAGE);  
      break;
    case DO_LAYOUT_EN:
    case DO_LAYOUT_RU:
      run_advanced(DO_NEXT_LANGUAGE);
      break;
    case DO_NEXT_LANGUAGE:
      ; // empty statement for declarations
      uint8_t hold = 0;
      uint8_t tap = 0;
      uint32_t timeout = 50;

      switch (zored_os) {
        case ZOS_WINDOWS:
          hold = KC_LALT;
          tap = KC_LSFT;
          break;
        case ZOS_MACOS:
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
        case ZOS_WINDOWS:
          tap_code16(G(KC_DOT));
          break;
        case ZOS_MACOS:
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
        case ZOS_WINDOWS:
          SEND_STRING("conemu");
          break;
        case ZOS_MACOS:
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
        case ZOS_MACOS:
          tap_code16(G(C(S(KC_4))));
          break;
        case ZOS_WINDOWS:
          tap_code16(G(S(KC_S)));
          break;
      }
      break;
    case DO_COPY:
      switch (zored_os) {
        case ZOS_MACOS:
          tap_code16(G(KC_C));
          break;
        case ZOS_WINDOWS:
          tap_code16(C(KC_C));
          break;
      }
      break;
    case DO_PASTE:
      switch (zored_os) {
        case ZOS_MACOS:
          tap_code16(G(KC_V));
          break;
        case ZOS_WINDOWS:
          tap_code16(C(KC_V));
          break;
      }
      break;
    case DO_BOOTLOADER:
      reset_keyboard();
      break;
    case DO_CLOSE:
      switch (zored_os) {
        case ZOS_WINDOWS:
          tap_code16(A(KC_F4));
          break;
        case ZOS_MACOS:
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

combo_t key_combos[] = {
  
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


void process_combo_event(uint16_t combo_index, bool pressed) {
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
  LAYER_GAME,
  LAYER_GAMENUMBERS,
  LAYER_GAMEMOUSE,
  LAYER_GAMEMIRROR,
  LAYER_SYMBOL,
  LAYER_NAVIGATION,
  LAYER_EMOJI,
  LAYER_NAVIGATION2,
  LAYER_HISTORY,
  LAYER_STENO
};

enum dance_keys {
  DANCE_KC_CAPSDANCE = 1,
  DANCE_KC_ADANCE,
  DANCE_KC_ZDANCE,
  DANCE_KC_XDANCE,
  DANCE_KC_CDANCE,
  DANCE_KC_SEMICOLONDANCE,
  DANCE_KC_QUOTEDANCE,
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
  DANCE_KC_COOLDANCE,
  DANCE_KC_OKDANCE,
  DANCE_KC_LOVEDANCE,
  DANCE_KC_F7DANCE,
  DANCE_KC_F8DANCE,
  DANCE_KC_F9DANCE,
  DANCE_KC_F4DANCE,
  DANCE_KC_F5DANCE,
  DANCE_KC_F6DANCE,
  DANCE_KC_F1DANCE,
  DANCE_KC_F2DANCE,
  DANCE_KC_F3DANCE,
  DANCE_KC_F10DANCE
};
enum dance_action_names {
  ACTION_SEQ__DO_LAYOUT_EN_1 = 1,
  ACTION_SEQ__DO_LAYOUT_RU_2,
  ACTION_SEQ__HOLD_LAYER_SYMBOL_4,
  ACTION_SEQ__HOLD_LAYER_SYMBOL__LCTL_5,
  ACTION_SEQ__A_7,
  ACTION_SEQ__HOLD_LAYER_HISTORY_9,
  ACTION_SEQ__Z_11,
  ACTION_SEQ__LCTL_13,
  ACTION_SEQ__LCTL__LSFT_14,
  ACTION_SEQ__LCTL__LALT__LGUI_15,
  ACTION_SEQ__X_17,
  ACTION_SEQ__LALT_19,
  ACTION_SEQ__LALT__LSFT_20,
  ACTION_SEQ__C_22,
  ACTION_SEQ__LGUI_24,
  ACTION_SEQ__LGUI__LSFT_25,
  ACTION_SEQ__SCLN_27,
  ACTION_SEQ__HOLD_LAYER_NAVIGATION_29,
  ACTION_SEQ__HOLD_LAYER_NAVIGATION2_30,
  ACTION_SEQ__TOGGLE_LAYER_NAVIGATION_31,
  ACTION_SEQ__QUOT_33,
  ACTION_SEQ__HOLD_LAYER_EMOJI_35,
  ACTION_SEQ__HOLD_LAYER_EMOJI__LCTL_36,
  ACTION_SEQ__TOGGLE_LAYER_EMOJI_37,
  ACTION_SEQ__COMM_39,
  ACTION_SEQ__RGUI_41,
  ACTION_SEQ__RGUI__RSFT_42,
  ACTION_SEQ__DOT_44,
  ACTION_SEQ__RALT_46,
  ACTION_SEQ__RALT__RSFT_47,
  ACTION_SEQ__SLSH_49,
  ACTION_SEQ__RCTL_51,
  ACTION_SEQ__RCTL__RSFT_52,
  ACTION_SEQ__TAB_54,
  ACTION_SEQ__HOLD_LAYER_SYMBOL_56,
  ACTION_SEQ__LSFT__5_58,
  ACTION_SEQ__LCTL_60,
  ACTION_SEQ__LCTL__LSFT_61,
  ACTION_SEQ__LCTL__LALT__LGUI_62,
  ACTION_SEQ__LSFT__6_64,
  ACTION_SEQ__LALT_66,
  ACTION_SEQ__LALT__LSFT_67,
  ACTION_SEQ__LBRC_69,
  ACTION_SEQ__LGUI_71,
  ACTION_SEQ__LGUI__LSFT_72,
  ACTION_SEQ__1F60A_74,
  ACTION_SEQ__1F601_75,
  ACTION_SEQ__1F604_76,
  ACTION_SEQ__1F602_77,
  ACTION_SEQ__1F61E_80,
  ACTION_SEQ__1F622_81,
  ACTION_SEQ__1F62D_82,
  ACTION_SEQ__1F914_85,
  ACTION_SEQ__1F928_86,
  ACTION_SEQ__1F631_87,
  ACTION_SEQ__1F60F_90,
  ACTION_SEQ__1F60E_91,
  ACTION_SEQ__1F4AA_92,
  ACTION_SEQ__1F525_93,
  ACTION_SEQ__1F44D_96,
  ACTION_SEQ__1F44C_97,
  ACTION_SEQ__1F197_98,
  ACTION_SEQ__1F61A_101,
  ACTION_SEQ__1F60D_102,
  ACTION_SEQ__1F618_103,
  ACTION_SEQ__1F496_104,
  ACTION_SEQ__F7_107,
  ACTION_SEQ__F17_108,
  ACTION_SEQ__F8_111,
  ACTION_SEQ__F18_112,
  ACTION_SEQ__F9_115,
  ACTION_SEQ__F19_116,
  ACTION_SEQ__F4_119,
  ACTION_SEQ__F14_120,
  ACTION_SEQ__F5_123,
  ACTION_SEQ__F15_124,
  ACTION_SEQ__F6_127,
  ACTION_SEQ__F16_128,
  ACTION_SEQ__F1_131,
  ACTION_SEQ__F11_132,
  ACTION_SEQ__F2_135,
  ACTION_SEQ__F12_136,
  ACTION_SEQ__F3_139,
  ACTION_SEQ__F13_140,
  ACTION_SEQ__F10_143,
  ACTION_SEQ__F20_144
};

static int dance_key_states[86] = {0};

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

  dance_finish_as_tap(state);
}

void dance_tap_on_enemy_hold(tap_dance_state_t *state) {
  if (state->count == 0) {
    return;
  }

  uint16_t dance_key = state->index;

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

  dance_finish_as_tap(state);
}

void on_dance_each_tap(tap_dance_state_t *state, void *user_data) {
  dance_tap_on_enemy_hold(state);
  dance_flush_on_many_taps(state);
}

void on_dance_each_release(tap_dance_state_t *state, void *user_data) {
  uint16_t dance_key = state->index;

  switch (dance_key) {
    
    case DANCE_KC_CAPSDANCE:
      if (state->count < 2) {
        return;
      }
      break;
 

    case DANCE_KC_ADANCE:
      if (state->count < 1) {
        return;
      }
      break;
 

    case DANCE_KC_ZDANCE:
      if (state->count < 3) {
        return;
      }
      break;
 

    case DANCE_KC_XDANCE:
      if (state->count < 2) {
        return;
      }
      break;
 

    case DANCE_KC_CDANCE:
      if (state->count < 2) {
        return;
      }
      break;
 

    case DANCE_KC_SEMICOLONDANCE:
      if (state->count < 3) {
        return;
      }
      break;
 

    case DANCE_KC_QUOTEDANCE:
      if (state->count < 3) {
        return;
      }
      break;
 

    case DANCE_KC_COMMADANCE:
      if (state->count < 2) {
        return;
      }
      break;
 

    case DANCE_KC_DOTDANCE:
      if (state->count < 2) {
        return;
      }
      break;
 

    case DANCE_KC_SLASHDANCE:
      if (state->count < 2) {
        return;
      }
      break;
 

    case DANCE_KC_TABDANCE:
      if (state->count < 1) {
        return;
      }
      break;
 

    case DANCE_KC_PERCENTDANCE:
      if (state->count < 3) {
        return;
      }
      break;
 

    case DANCE_KC_CIRCUMFLEXDANCE:
      if (state->count < 2) {
        return;
      }
      break;
 

    case DANCE_KC_LEFTSQUAREBRACKETDANCE:
      if (state->count < 2) {
        return;
      }
      break;
 

    case DANCE_KC_HAPPYDANCE:
      if (state->count < 4) {
        return;
      }
      break;
 

    case DANCE_KC_SADDANCE:
      if (state->count < 3) {
        return;
      }
      break;
 

    case DANCE_KC_STRANGEDANCE:
      if (state->count < 3) {
        return;
      }
      break;
 

    case DANCE_KC_COOLDANCE:
      if (state->count < 4) {
        return;
      }
      break;
 

    case DANCE_KC_OKDANCE:
      if (state->count < 3) {
        return;
      }
      break;
 

    case DANCE_KC_LOVEDANCE:
      if (state->count < 4) {
        return;
      }
      break;
 

    case DANCE_KC_F7DANCE:
      if (state->count < 2) {
        return;
      }
      break;
 

    case DANCE_KC_F8DANCE:
      if (state->count < 2) {
        return;
      }
      break;
 

    case DANCE_KC_F9DANCE:
      if (state->count < 2) {
        return;
      }
      break;
 

    case DANCE_KC_F4DANCE:
      if (state->count < 2) {
        return;
      }
      break;
 

    case DANCE_KC_F5DANCE:
      if (state->count < 2) {
        return;
      }
      break;
 

    case DANCE_KC_F6DANCE:
      if (state->count < 2) {
        return;
      }
      break;
 

    case DANCE_KC_F1DANCE:
      if (state->count < 2) {
        return;
      }
      break;
 

    case DANCE_KC_F2DANCE:
      if (state->count < 2) {
        return;
      }
      break;
 

    case DANCE_KC_F3DANCE:
      if (state->count < 2) {
        return;
      }
      break;
 

    case DANCE_KC_F10DANCE:
      if (state->count < 2) {
        return;
      }
      break;
 
    default:
      return;
  }

  dance_finish_as_tap(state);
}

void on_dance_finished(tap_dance_state_t *state, void *user_data) {
  if (state->count == 0) {
    return;
  }
  uint16_t dance_key = state->index;

  switch (dance_key) {
    
    case DANCE_KC_CAPSDANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            layer_on(LAYER_SYMBOL);
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_SYMBOL_4;
            return;
      
          case 2:
            layer_on(LAYER_SYMBOL);
    code_down(KC_LCTL);
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_SYMBOL__LCTL_5;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            dance_key_states[dance_key] = ACTION_SEQ__DO_LAYOUT_EN_1;
            return;
      
          case 2:
            dance_key_states[dance_key] = ACTION_SEQ__DO_LAYOUT_RU_2;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              dance_key_states[dance_key] = ACTION_SEQ__DO_LAYOUT_EN_1;
              run_advanced(DO_LAYOUT_EN);
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
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_HISTORY_9;
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
            dance_key_states[dance_key] = ACTION_SEQ__A_7;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_A);
            dance_key_states[dance_key] = ACTION_SEQ__A_7;
              code_up(KC_A);
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
            code_down(KC_LCTL);
            dance_key_states[dance_key] = ACTION_SEQ__LCTL_13;
            return;
      
          case 2:
            code_down_2(KC_LCTL, KC_LSFT);
            dance_key_states[dance_key] = ACTION_SEQ__LCTL__LSFT_14;
            return;
      
          case 3:
            code_down_3(KC_LCTL, KC_LALT, KC_LGUI);
            dance_key_states[dance_key] = ACTION_SEQ__LCTL__LALT__LGUI_15;
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
            dance_key_states[dance_key] = ACTION_SEQ__Z_11;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_Z);
            dance_key_states[dance_key] = ACTION_SEQ__Z_11;
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
            dance_key_states[dance_key] = ACTION_SEQ__LALT_19;
            return;
      
          case 2:
            code_down_2(KC_LALT, KC_LSFT);
            dance_key_states[dance_key] = ACTION_SEQ__LALT__LSFT_20;
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
            dance_key_states[dance_key] = ACTION_SEQ__X_17;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_X);
            dance_key_states[dance_key] = ACTION_SEQ__X_17;
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
            dance_key_states[dance_key] = ACTION_SEQ__LGUI_24;
            return;
      
          case 2:
            code_down_2(KC_LGUI, KC_LSFT);
            dance_key_states[dance_key] = ACTION_SEQ__LGUI__LSFT_25;
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
            dance_key_states[dance_key] = ACTION_SEQ__C_22;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_C);
            dance_key_states[dance_key] = ACTION_SEQ__C_22;
              code_up(KC_C);
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
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_NAVIGATION_29;
            return;
      
          case 2:
            layer_on(LAYER_NAVIGATION2);
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_NAVIGATION2_30;
            return;
      
          case 3:
            layer_invert(LAYER_NAVIGATION);
            dance_key_states[dance_key] = ACTION_SEQ__TOGGLE_LAYER_NAVIGATION_31;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_SCLN);
            dance_key_states[dance_key] = ACTION_SEQ__SCLN_27;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_SCLN);
            dance_key_states[dance_key] = ACTION_SEQ__SCLN_27;
              code_up(KC_SCLN);
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
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_EMOJI_35;
            return;
      
          case 2:
            layer_on(LAYER_EMOJI);
    code_down(KC_LCTL);
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_EMOJI__LCTL_36;
            return;
      
          case 3:
            layer_invert(LAYER_EMOJI);
            dance_key_states[dance_key] = ACTION_SEQ__TOGGLE_LAYER_EMOJI_37;
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
            dance_key_states[dance_key] = ACTION_SEQ__QUOT_33;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_QUOT);
            dance_key_states[dance_key] = ACTION_SEQ__QUOT_33;
              code_up(KC_QUOT);
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
            dance_key_states[dance_key] = ACTION_SEQ__RGUI_41;
            return;
      
          case 2:
            code_down_2(KC_RGUI, KC_RSFT);
            dance_key_states[dance_key] = ACTION_SEQ__RGUI__RSFT_42;
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
            dance_key_states[dance_key] = ACTION_SEQ__COMM_39;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_COMM);
            dance_key_states[dance_key] = ACTION_SEQ__COMM_39;
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
            dance_key_states[dance_key] = ACTION_SEQ__RALT_46;
            return;
      
          case 2:
            code_down_2(KC_RALT, KC_RSFT);
            dance_key_states[dance_key] = ACTION_SEQ__RALT__RSFT_47;
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
            dance_key_states[dance_key] = ACTION_SEQ__DOT_44;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_DOT);
            dance_key_states[dance_key] = ACTION_SEQ__DOT_44;
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
            code_down(KC_RCTL);
            dance_key_states[dance_key] = ACTION_SEQ__RCTL_51;
            return;
      
          case 2:
            code_down_2(KC_RCTL, KC_RSFT);
            dance_key_states[dance_key] = ACTION_SEQ__RCTL__RSFT_52;
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
            dance_key_states[dance_key] = ACTION_SEQ__SLSH_49;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_SLSH);
            dance_key_states[dance_key] = ACTION_SEQ__SLSH_49;
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
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_SYMBOL_56;
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
            dance_key_states[dance_key] = ACTION_SEQ__TAB_54;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_TAB);
            dance_key_states[dance_key] = ACTION_SEQ__TAB_54;
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
            code_down(KC_LCTL);
            dance_key_states[dance_key] = ACTION_SEQ__LCTL_60;
            return;
      
          case 2:
            code_down_2(KC_LCTL, KC_LSFT);
            dance_key_states[dance_key] = ACTION_SEQ__LCTL__LSFT_61;
            return;
      
          case 3:
            code_down_3(KC_LCTL, KC_LALT, KC_LGUI);
            dance_key_states[dance_key] = ACTION_SEQ__LCTL__LALT__LGUI_62;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down_2(KC_LSFT, KC_5);
            dance_key_states[dance_key] = ACTION_SEQ__LSFT__5_58;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down_2(KC_LSFT, KC_5);
            dance_key_states[dance_key] = ACTION_SEQ__LSFT__5_58;
              code_up_2(KC_LSFT, KC_5);
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
            dance_key_states[dance_key] = ACTION_SEQ__LALT_66;
            return;
      
          case 2:
            code_down_2(KC_LALT, KC_LSFT);
            dance_key_states[dance_key] = ACTION_SEQ__LALT__LSFT_67;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down_2(KC_LSFT, KC_6);
            dance_key_states[dance_key] = ACTION_SEQ__LSFT__6_64;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down_2(KC_LSFT, KC_6);
            dance_key_states[dance_key] = ACTION_SEQ__LSFT__6_64;
              code_up_2(KC_LSFT, KC_6);
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
            dance_key_states[dance_key] = ACTION_SEQ__LGUI_71;
            return;
      
          case 2:
            code_down_2(KC_LGUI, KC_LSFT);
            dance_key_states[dance_key] = ACTION_SEQ__LGUI__LSFT_72;
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
            dance_key_states[dance_key] = ACTION_SEQ__LBRC_69;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_LBRC);
            dance_key_states[dance_key] = ACTION_SEQ__LBRC_69;
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
    
            dance_key_states[dance_key] = ACTION_SEQ__1F60A_74;
            return;
      
          case 2:
            tap_unicode(EMOJI_1F601);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F601_75;
            return;
      
          case 3:
            tap_unicode(EMOJI_1F604);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F604_76;
            return;
      
          case 4:
            tap_unicode(EMOJI_1F602);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F602_77;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              tap_unicode(EMOJI_1F60A);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F60A_74;
              
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
    
            dance_key_states[dance_key] = ACTION_SEQ__1F61E_80;
            return;
      
          case 2:
            tap_unicode(EMOJI_1F622);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F622_81;
            return;
      
          case 3:
            tap_unicode(EMOJI_1F62D);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F62D_82;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              tap_unicode(EMOJI_1F61E);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F61E_80;
              
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
    
            dance_key_states[dance_key] = ACTION_SEQ__1F914_85;
            return;
      
          case 2:
            tap_unicode(EMOJI_1F928);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F928_86;
            return;
      
          case 3:
            tap_unicode(EMOJI_1F631);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F631_87;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              tap_unicode(EMOJI_1F914);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F914_85;
              
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
    
            dance_key_states[dance_key] = ACTION_SEQ__1F60F_90;
            return;
      
          case 2:
            tap_unicode(EMOJI_1F60E);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F60E_91;
            return;
      
          case 3:
            tap_unicode(EMOJI_1F4AA);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F4AA_92;
            return;
      
          case 4:
            tap_unicode(EMOJI_1F525);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F525_93;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              tap_unicode(EMOJI_1F60F);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F60F_90;
              
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
    
            dance_key_states[dance_key] = ACTION_SEQ__1F44D_96;
            return;
      
          case 2:
            tap_unicode(EMOJI_1F44C);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F44C_97;
            return;
      
          case 3:
            tap_unicode(EMOJI_1F197);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F197_98;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              tap_unicode(EMOJI_1F44D);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F44D_96;
              
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
    
            dance_key_states[dance_key] = ACTION_SEQ__1F61A_101;
            return;
      
          case 2:
            tap_unicode(EMOJI_1F60D);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F60D_102;
            return;
      
          case 3:
            tap_unicode(EMOJI_1F618);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F618_103;
            return;
      
          case 4:
            tap_unicode(EMOJI_1F496);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F496_104;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              tap_unicode(EMOJI_1F61A);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F61A_101;
              
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
            dance_key_states[dance_key] = ACTION_SEQ__F7_107;
            return;
      
          case 2:
            code_down(KC_F17);
            dance_key_states[dance_key] = ACTION_SEQ__F17_108;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F7);
            dance_key_states[dance_key] = ACTION_SEQ__F7_107;
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
            dance_key_states[dance_key] = ACTION_SEQ__F8_111;
            return;
      
          case 2:
            code_down(KC_F18);
            dance_key_states[dance_key] = ACTION_SEQ__F18_112;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F8);
            dance_key_states[dance_key] = ACTION_SEQ__F8_111;
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
            dance_key_states[dance_key] = ACTION_SEQ__F9_115;
            return;
      
          case 2:
            code_down(KC_F19);
            dance_key_states[dance_key] = ACTION_SEQ__F19_116;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F9);
            dance_key_states[dance_key] = ACTION_SEQ__F9_115;
              code_up(KC_F9);
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
            dance_key_states[dance_key] = ACTION_SEQ__F4_119;
            return;
      
          case 2:
            code_down(KC_F14);
            dance_key_states[dance_key] = ACTION_SEQ__F14_120;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F4);
            dance_key_states[dance_key] = ACTION_SEQ__F4_119;
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
            dance_key_states[dance_key] = ACTION_SEQ__F5_123;
            return;
      
          case 2:
            code_down(KC_F15);
            dance_key_states[dance_key] = ACTION_SEQ__F15_124;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F5);
            dance_key_states[dance_key] = ACTION_SEQ__F5_123;
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
            dance_key_states[dance_key] = ACTION_SEQ__F6_127;
            return;
      
          case 2:
            code_down(KC_F16);
            dance_key_states[dance_key] = ACTION_SEQ__F16_128;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F6);
            dance_key_states[dance_key] = ACTION_SEQ__F6_127;
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
            dance_key_states[dance_key] = ACTION_SEQ__F1_131;
            return;
      
          case 2:
            code_down(KC_F11);
            dance_key_states[dance_key] = ACTION_SEQ__F11_132;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F1);
            dance_key_states[dance_key] = ACTION_SEQ__F1_131;
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
            dance_key_states[dance_key] = ACTION_SEQ__F2_135;
            return;
      
          case 2:
            code_down(KC_F12);
            dance_key_states[dance_key] = ACTION_SEQ__F12_136;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F2);
            dance_key_states[dance_key] = ACTION_SEQ__F2_135;
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
            dance_key_states[dance_key] = ACTION_SEQ__F3_139;
            return;
      
          case 2:
            code_down(KC_F13);
            dance_key_states[dance_key] = ACTION_SEQ__F13_140;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F3);
            dance_key_states[dance_key] = ACTION_SEQ__F3_139;
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
            dance_key_states[dance_key] = ACTION_SEQ__F10_143;
            return;
      
          case 2:
            code_down(KC_F20);
            dance_key_states[dance_key] = ACTION_SEQ__F20_144;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F10);
            dance_key_states[dance_key] = ACTION_SEQ__F10_143;
              code_up(KC_F10);
            }
            return;
        }
      }
      break;
    
  }
}

void on_dance_reset(tap_dance_state_t *state, void *user_data) {
  uint16_t dance_key = state->index;
  switch (dance_key_states[dance_key]) {
    case ACTION_SEQ__HOLD_LAYER_SYMBOL_4:
            layer_off(LAYER_SYMBOL);
            break;
    
          case ACTION_SEQ__HOLD_LAYER_SYMBOL__LCTL_5:
            layer_off(LAYER_SYMBOL);code_up(KC_LCTL);
            break;
        case ACTION_SEQ__DO_LAYOUT_EN_1:
            run_advanced(DO_LAYOUT_EN);
            break;
    
          case ACTION_SEQ__DO_LAYOUT_RU_2:
            run_advanced(DO_LAYOUT_RU);
            break;
    
        case ACTION_SEQ__HOLD_LAYER_HISTORY_9:
            layer_off(LAYER_HISTORY);
            break;
        case ACTION_SEQ__A_7:
            code_up(KC_A);
            break;
    
        case ACTION_SEQ__LCTL_13:
            code_up(KC_LCTL);
            break;
    
          case ACTION_SEQ__LCTL__LSFT_14:
            code_up_2(KC_LCTL, KC_LSFT);
            break;
    
          case ACTION_SEQ__LCTL__LALT__LGUI_15:
            code_up_3(KC_LCTL, KC_LALT, KC_LGUI);
            break;
        case ACTION_SEQ__Z_11:
            code_up(KC_Z);
            break;
    
        case ACTION_SEQ__LALT_19:
            code_up(KC_LALT);
            break;
    
          case ACTION_SEQ__LALT__LSFT_20:
            code_up_2(KC_LALT, KC_LSFT);
            break;
        case ACTION_SEQ__X_17:
            code_up(KC_X);
            break;
    
        case ACTION_SEQ__LGUI_24:
            code_up(KC_LGUI);
            break;
    
          case ACTION_SEQ__LGUI__LSFT_25:
            code_up_2(KC_LGUI, KC_LSFT);
            break;
        case ACTION_SEQ__C_22:
            code_up(KC_C);
            break;
    
        case ACTION_SEQ__HOLD_LAYER_NAVIGATION_29:
            layer_off(LAYER_NAVIGATION);
            break;
    
          case ACTION_SEQ__HOLD_LAYER_NAVIGATION2_30:
            layer_off(LAYER_NAVIGATION2);
            break;
        case ACTION_SEQ__SCLN_27:
            code_up(KC_SCLN);
            break;
    
        case ACTION_SEQ__HOLD_LAYER_EMOJI_35:
            layer_off(LAYER_EMOJI);
            break;
    
          case ACTION_SEQ__HOLD_LAYER_EMOJI__LCTL_36:
            layer_off(LAYER_EMOJI);code_up(KC_LCTL);
            break;
        case ACTION_SEQ__QUOT_33:
            code_up(KC_QUOT);
            break;
    
        case ACTION_SEQ__RGUI_41:
            code_up(KC_RGUI);
            break;
    
          case ACTION_SEQ__RGUI__RSFT_42:
            code_up_2(KC_RGUI, KC_RSFT);
            break;
        case ACTION_SEQ__COMM_39:
            code_up(KC_COMM);
            break;
    
        case ACTION_SEQ__RALT_46:
            code_up(KC_RALT);
            break;
    
          case ACTION_SEQ__RALT__RSFT_47:
            code_up_2(KC_RALT, KC_RSFT);
            break;
        case ACTION_SEQ__DOT_44:
            code_up(KC_DOT);
            break;
    
        case ACTION_SEQ__RCTL_51:
            code_up(KC_RCTL);
            break;
    
          case ACTION_SEQ__RCTL__RSFT_52:
            code_up_2(KC_RCTL, KC_RSFT);
            break;
        case ACTION_SEQ__SLSH_49:
            code_up(KC_SLSH);
            break;
    
        case ACTION_SEQ__HOLD_LAYER_SYMBOL_56:
            layer_off(LAYER_SYMBOL);
            break;
        case ACTION_SEQ__TAB_54:
            code_up(KC_TAB);
            break;
    
        case ACTION_SEQ__LCTL_60:
            code_up(KC_LCTL);
            break;
    
          case ACTION_SEQ__LCTL__LSFT_61:
            code_up_2(KC_LCTL, KC_LSFT);
            break;
    
          case ACTION_SEQ__LCTL__LALT__LGUI_62:
            code_up_3(KC_LCTL, KC_LALT, KC_LGUI);
            break;
        case ACTION_SEQ__LSFT__5_58:
            code_up_2(KC_LSFT, KC_5);
            break;
    
        case ACTION_SEQ__LALT_66:
            code_up(KC_LALT);
            break;
    
          case ACTION_SEQ__LALT__LSFT_67:
            code_up_2(KC_LALT, KC_LSFT);
            break;
        case ACTION_SEQ__LSFT__6_64:
            code_up_2(KC_LSFT, KC_6);
            break;
    
        case ACTION_SEQ__LGUI_71:
            code_up(KC_LGUI);
            break;
    
          case ACTION_SEQ__LGUI__LSFT_72:
            code_up_2(KC_LGUI, KC_LSFT);
            break;
        case ACTION_SEQ__LBRC_69:
            code_up(KC_LBRC);
            break;
    
        
        case ACTION_SEQ__F7_107:
            code_up(KC_F7);
            break;
    
          case ACTION_SEQ__F17_108:
            code_up(KC_F17);
            break;
    
        
        case ACTION_SEQ__F8_111:
            code_up(KC_F8);
            break;
    
          case ACTION_SEQ__F18_112:
            code_up(KC_F18);
            break;
    
        
        case ACTION_SEQ__F9_115:
            code_up(KC_F9);
            break;
    
          case ACTION_SEQ__F19_116:
            code_up(KC_F19);
            break;
    
        
        case ACTION_SEQ__F4_119:
            code_up(KC_F4);
            break;
    
          case ACTION_SEQ__F14_120:
            code_up(KC_F14);
            break;
    
        
        case ACTION_SEQ__F5_123:
            code_up(KC_F5);
            break;
    
          case ACTION_SEQ__F15_124:
            code_up(KC_F15);
            break;
    
        
        case ACTION_SEQ__F6_127:
            code_up(KC_F6);
            break;
    
          case ACTION_SEQ__F16_128:
            code_up(KC_F16);
            break;
    
        
        case ACTION_SEQ__F1_131:
            code_up(KC_F1);
            break;
    
          case ACTION_SEQ__F11_132:
            code_up(KC_F11);
            break;
    
        
        case ACTION_SEQ__F2_135:
            code_up(KC_F2);
            break;
    
          case ACTION_SEQ__F12_136:
            code_up(KC_F12);
            break;
    
        
        case ACTION_SEQ__F3_139:
            code_up(KC_F3);
            break;
    
          case ACTION_SEQ__F13_140:
            code_up(KC_F13);
            break;
    
        
        case ACTION_SEQ__F10_143:
            code_up(KC_F10);
            break;
    
          case ACTION_SEQ__F20_144:
            code_up(KC_F20);
            break;
    
  }

  dance_key_states[dance_key] = 0;
}


#define DANCE_MODIFIER() ACTION_TAP_DANCE_FN_ADVANCED_WITH_RELEASE(on_dance_each_tap, on_dance_each_release, on_dance_finished, on_dance_reset)

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
  if (IS_QK_TAP_DANCE(keycode)) {
    return TAPPING_TERM_TAP_DANCE;
  }
  return TAPPING_TERM;
}

tap_dance_action_t tap_dance_actions[] = {
    [DANCE_KC_CAPSDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_ADANCE] = DANCE_MODIFIER(),
  [DANCE_KC_ZDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_XDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_CDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_SEMICOLONDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_QUOTEDANCE] = DANCE_MODIFIER(),
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
  [DANCE_KC_COOLDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_OKDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_LOVEDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F7DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F8DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F9DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F4DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F5DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F6DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F1DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F2DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F3DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F10DANCE] = DANCE_MODIFIER()
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[LAYER_DEFAULT] = LAYOUT_ergodox(
  
/* left-0 */ KC_ESC,KC_1,KC_2,KC_3,KC_4,KC_5,_______,
/* left-1 */ KC_ESC,KC_Q,KC_W,KC_E,KC_R,KC_T,_______,
/* left-2 */ TD(DANCE_KC_CAPSDANCE),TD(DANCE_KC_ADANCE),KC_S,KC_D,KC_F,KC_G,
/* left-3 */ SC_LSPO,TD(DANCE_KC_ZDANCE),TD(DANCE_KC_XDANCE),TD(DANCE_KC_CDANCE),KC_V,KC_B,KC_DO_FUTURE_APP,
/* left-4 */ QK_LEAD,TG(LAYER_NAVIGATION),_______,KC_DO_COPY,KC_DO_PASTE,
/* left-thumb-0 */ KC_ESC,KC_DO_ONE_SHOT_TURBO,
/* left-thumb-1 */ KC_HOME,
/* left-thumb-2 */ KC_SPC,KC_BSPC,KC_END,
/* right-0 */ _______,KC_6,KC_7,KC_8,KC_9,KC_0,_______,
/* right-1 */ _______,KC_Y,KC_U,KC_I,KC_O,KC_P,KC_BSLS,
/* right-2 */ KC_H,KC_J,KC_K,KC_L,TD(DANCE_KC_SEMICOLONDANCE),TD(DANCE_KC_QUOTEDANCE),
/* right-3 */ KC_DO_PAST_APP,KC_N,KC_M,TD(DANCE_KC_COMMADANCE),TD(DANCE_KC_DOTDANCE),TD(DANCE_KC_SLASHDANCE),SC_RSPC,
/* right-4 */ KC_LBRC,KC_RBRC,KC_DO_PAST_WINDOW,KC_DO_FUTURE_WINDOW,TG(LAYER_GAME),
/* right-thumb-0 */ KC_ESC,_______,
/* right-thumb-1 */ KC_PGUP,
/* right-thumb-2 */ KC_PGDN,TD(DANCE_KC_TABDANCE),KC_ENT
)
  ,

[LAYER_GAME] = LAYOUT_ergodox(
  
/* left-0 */ KC_ESC,KC_1,KC_2,KC_3,KC_4,KC_5,_______,
/* left-1 */ KC_TAB,_______,_______,_______,_______,_______,_______,
/* left-2 */ KC_CAPS,KC_A,_______,_______,_______,_______,
/* left-3 */ KC_LSFT,KC_Z,KC_X,KC_C,_______,_______,_______,
/* left-4 */ KC_LCTL,KC_LALT,KC_LGUI,MO(LAYER_GAMEMIRROR),MO(LAYER_GAMENUMBERS),
/* left-thumb-0 */ _______,_______,
/* left-thumb-1 */ _______,
/* left-thumb-2 */ _______,_______,_______,
/* right-0 */ _______,_______,_______,_______,_______,_______,_______,
/* right-1 */ _______,_______,_______,_______,_______,_______,_______,
/* right-2 */ _______,_______,_______,_______,KC_SCLN,KC_QUOT,
/* right-3 */ _______,_______,_______,KC_COMM,KC_DOT,KC_SLSH,KC_RSFT,
/* right-4 */ TG(LAYER_GAMEMOUSE),_______,_______,_______,_______,
/* right-thumb-0 */ _______,_______,
/* right-thumb-1 */ _______,
/* right-thumb-2 */ _______,KC_TAB,_______
)
  ,

[LAYER_GAMENUMBERS] = LAYOUT_ergodox(
  
/* left-0 */ _______,KC_F1,KC_F2,KC_F3,KC_F4,KC_F5,_______,
/* left-1 */ _______,KC_1,KC_2,KC_3,KC_F6,KC_F7,_______,
/* left-2 */ _______,KC_4,KC_5,KC_6,KC_F8,KC_F9,
/* left-3 */ _______,KC_7,KC_8,KC_9,KC_F10,KC_F11,_______,
/* left-4 */ _______,_______,KC_0,_______,_______,
/* left-thumb-0 */ _______,_______,
/* left-thumb-1 */ _______,
/* left-thumb-2 */ _______,_______,_______,
/* right-0 */ _______,_______,_______,_______,_______,_______,_______,
/* right-1 */ _______,_______,_______,_______,_______,_______,_______,
/* right-2 */ _______,_______,_______,_______,_______,_______,
/* right-3 */ _______,_______,_______,_______,_______,_______,_______,
/* right-4 */ _______,_______,_______,_______,_______,
/* right-thumb-0 */ _______,_______,
/* right-thumb-1 */ _______,
/* right-thumb-2 */ _______,_______,_______
)
  ,

[LAYER_GAMEMOUSE] = LAYOUT_ergodox(
  
/* left-0 */ _______,_______,_______,_______,_______,_______,_______,
/* left-1 */ _______,_______,_______,_______,_______,_______,_______,
/* left-2 */ _______,_______,_______,_______,_______,_______,
/* left-3 */ _______,_______,_______,_______,_______,_______,_______,
/* left-4 */ _______,_______,KC_0,_______,_______,
/* left-thumb-0 */ _______,_______,
/* left-thumb-1 */ _______,
/* left-thumb-2 */ _______,_______,_______,
/* right-0 */ _______,_______,_______,_______,_______,_______,_______,
/* right-1 */ _______,_______,MS_BTN1,MS_UP,MS_BTN2,_______,_______,
/* right-2 */ _______,MS_LEFT,MS_DOWN,MS_RGHT,_______,_______,
/* right-3 */ _______,_______,MS_WHLD,MS_BTN3,MS_WHLU,_______,_______,
/* right-4 */ TG(LAYER_GAMEMOUSE),_______,_______,_______,_______,
/* right-thumb-0 */ _______,_______,
/* right-thumb-1 */ _______,
/* right-thumb-2 */ _______,_______,_______
)
  ,

[LAYER_GAMEMIRROR] = LAYOUT_ergodox(
  
/* left-0 */ _______,_______,_______,_______,_______,_______,_______,
/* left-1 */ _______,KC_Y,KC_U,KC_I,KC_O,KC_P,_______,
/* left-2 */ _______,KC_H,KC_J,KC_K,KC_L,KC_SCLN,
/* left-3 */ _______,KC_N,KC_M,KC_COMM,KC_DOT,KC_SLSH,_______,
/* left-4 */ _______,_______,KC_0,_______,_______,
/* left-thumb-0 */ _______,_______,
/* left-thumb-1 */ _______,
/* left-thumb-2 */ KC_ENT,KC_DELETE,_______,
/* right-0 */ _______,_______,_______,_______,_______,_______,_______,
/* right-1 */ _______,_______,_______,_______,_______,_______,_______,
/* right-2 */ _______,_______,_______,_______,_______,_______,
/* right-3 */ _______,_______,_______,_______,_______,_______,_______,
/* right-4 */ _______,_______,_______,_______,_______,
/* right-thumb-0 */ _______,_______,
/* right-thumb-1 */ _______,
/* right-thumb-2 */ _______,_______,_______
)
  ,

[LAYER_SYMBOL] = LAYOUT_ergodox(
  
/* left-0 */ _______,_______,_______,_______,_______,_______,_______,
/* left-1 */ _______,KC_EXLM,KC_HASH,KC_LCBR,KC_RCBR,KC_SLSH,_______,
/* left-2 */ _______,KC_AT,KC_DLR,KC_LPRN,KC_RPRN,KC_GRV,
/* left-3 */ _______,TD(DANCE_KC_PERCENTDANCE),TD(DANCE_KC_CIRCUMFLEXDANCE),TD(DANCE_KC_LEFTSQUAREBRACKETDANCE),KC_RBRC,KC_TILD,_______,
/* left-4 */ _______,_______,_______,KC_DO_PAST,KC_DO_FUTURE,
/* left-thumb-0 */ _______,_______,
/* left-thumb-1 */ _______,
/* left-thumb-2 */ _______,KC_DELETE,_______,
/* right-0 */ _______,_______,_______,_______,_______,_______,_______,
/* right-1 */ _______,KC_EQL,KC_7,KC_8,KC_9,KC_AMPR,KC_PIPE,
/* right-2 */ KC_PPLS,KC_4,KC_5,KC_6,KC_PAST,_______,
/* right-3 */ _______,KC_MINS,KC_1,KC_2,KC_3,KC_SLSH,_______,
/* right-4 */ _______,KC_0,KC_DOT,_______,_______,
/* right-thumb-0 */ _______,_______,
/* right-thumb-1 */ _______,
/* right-thumb-2 */ _______,_______,_______
)
  ,

[LAYER_NAVIGATION] = LAYOUT_ergodox(
  
/* left-0 */ _______,_______,_______,_______,_______,_______,TG(LAYER_STENO),
/* left-1 */ _______,_______,MS_BTN2,MS_UP,MS_BTN1,_______,_______,
/* left-2 */ _______,_______,MS_LEFT,MS_DOWN,MS_RGHT,_______,
/* left-3 */ _______,_______,_______,_______,_______,_______,_______,
/* left-4 */ _______,_______,_______,KC_DO_FUTURE_APP,KC_DO_PAST_APP,
/* left-thumb-0 */ _______,_______,
/* left-thumb-1 */ _______,
/* left-thumb-2 */ _______,_______,_______,
/* right-0 */ _______,_______,_______,_______,_______,_______,_______,
/* right-1 */ _______,_______,KC_BRID,KC_BRIU,_______,_______,_______,
/* right-2 */ KC_LEFT,KC_DOWN,KC_UP,KC_RGHT,_______,_______,
/* right-3 */ _______,MS_WHLL,MS_WHLD,MS_WHLU,MS_WHLR,_______,_______,
/* right-4 */ KC_VOLD,KC_VOLU,KC_MUTE,_______,_______,
/* right-thumb-0 */ _______,_______,
/* right-thumb-1 */ _______,
/* right-thumb-2 */ _______,_______,_______
)
  ,

[LAYER_EMOJI] = LAYOUT_ergodox(
  
/* left-0 */ _______,_______,_______,_______,_______,_______,_______,
/* left-1 */ _______,TD(DANCE_KC_HAPPYDANCE),TD(DANCE_KC_SADDANCE),TD(DANCE_KC_STRANGEDANCE),_______,_______,_______,
/* left-2 */ _______,TD(DANCE_KC_COOLDANCE),TD(DANCE_KC_OKDANCE),TD(DANCE_KC_LOVEDANCE),_______,_______,
/* left-3 */ _______,_______,_______,KC_DO_FUTURE_WINDOW,KC_DO_PAST_WINDOW,_______,_______,
/* left-4 */ KC_DO_BOOTLOADER,_______,_______,KC_DO_FUTURE_APP,KC_DO_PAST_APP,
/* left-thumb-0 */ _______,_______,
/* left-thumb-1 */ _______,
/* left-thumb-2 */ _______,_______,_______,
/* right-0 */ _______,DM_REC1,DM_REC2,DM_RSTP,DM_PLY1,DM_PLY2,_______,
/* right-1 */ _______,_______,TD(DANCE_KC_F7DANCE),TD(DANCE_KC_F8DANCE),TD(DANCE_KC_F9DANCE),_______,_______,
/* right-2 */ _______,TD(DANCE_KC_F4DANCE),TD(DANCE_KC_F5DANCE),TD(DANCE_KC_F6DANCE),_______,_______,
/* right-3 */ _______,_______,TD(DANCE_KC_F1DANCE),TD(DANCE_KC_F2DANCE),TD(DANCE_KC_F3DANCE),_______,_______,
/* right-4 */ _______,_______,TD(DANCE_KC_F10DANCE),_______,_______,
/* right-thumb-0 */ _______,_______,
/* right-thumb-1 */ _______,
/* right-thumb-2 */ _______,_______,_______
)
  ,

[LAYER_NAVIGATION2] = LAYOUT_ergodox(
  
/* left-0 */ _______,_______,_______,_______,_______,_______,_______,
/* left-1 */ _______,_______,_______,_______,_______,_______,_______,
/* left-2 */ _______,_______,_______,_______,_______,_______,
/* left-3 */ _______,_______,_______,_______,_______,_______,_______,
/* left-4 */ _______,_______,_______,_______,_______,
/* left-thumb-0 */ _______,_______,
/* left-thumb-1 */ _______,
/* left-thumb-2 */ _______,_______,_______,
/* right-0 */ _______,_______,_______,_______,_______,_______,_______,
/* right-1 */ _______,KC_SYSTEM_POWER,KC_SYSTEM_SLEEP,UC_WINC,UC_MAC,_______,_______,
/* right-2 */ KC_HOME,KC_PGDN,KC_PGUP,KC_END,_______,_______,
/* right-3 */ _______,KC_MEDIA_PREV_TRACK,_______,KC_MEDIA_PLAY_PAUSE,KC_MEDIA_NEXT_TRACK,_______,_______,
/* right-4 */ _______,_______,_______,_______,_______,
/* right-thumb-0 */ _______,_______,
/* right-thumb-1 */ _______,
/* right-thumb-2 */ _______,_______,_______
)
  ,

[LAYER_HISTORY] = LAYOUT_ergodox(
  
/* left-0 */ _______,_______,_______,_______,_______,_______,_______,
/* left-1 */ _______,_______,_______,_______,_______,_______,_______,
/* left-2 */ _______,_______,KC_HOME,KC_PGDN,KC_PGUP,KC_END,
/* left-3 */ _______,_______,_______,_______,_______,_______,_______,
/* left-4 */ _______,_______,_______,_______,_______,
/* left-thumb-0 */ _______,_______,
/* left-thumb-1 */ _______,
/* left-thumb-2 */ _______,_______,_______,
/* right-0 */ _______,_______,_______,_______,_______,_______,_______,
/* right-1 */ _______,_______,_______,_______,_______,_______,_______,
/* right-2 */ KC_DO_FUTURE_TAB,KC_DO_FUTURE_APP,KC_DO_PAST_APP,KC_DO_PAST_TAB,_______,_______,
/* right-3 */ _______,KC_DO_FUTURE_WINDOW,KC_DO_FUTURE,KC_DO_PAST,KC_DO_PAST_WINDOW,_______,_______,
/* right-4 */ _______,_______,_______,_______,_______,
/* right-thumb-0 */ _______,_______,
/* right-thumb-1 */ _______,
/* right-thumb-2 */ _______,_______,_______
)
  ,

[LAYER_STENO] = LAYOUT_ergodox(
  
/* left-0 */ KC_NO,KC_NO,KC_NO,KC_NO,KC_NO,KC_NO,TG(LAYER_STENO),
/* left-1 */ KC_NO,ST_N1,ST_N2,ST_N3,ST_N4,ST_N5,KC_NO,
/* left-2 */ ST_X1,ST_S1,ST_TL,ST_PL,ST_HL,ST_ST1,
/* left-3 */ ST_X2,ST_S2,ST_KL,ST_WL,ST_RL,ST_ST2,KC_NO,
/* left-4 */ KC_NO,KC_NO,ST_X3,ST_A,ST_O,
/* left-thumb-0 */ KC_NO,KC_NO,
/* left-thumb-1 */ KC_NO,
/* left-thumb-2 */ ST_A,ST_O,KC_NO,
/* right-0 */ KC_NO,KC_NO,KC_NO,KC_NO,KC_NO,KC_NO,KC_NO,
/* right-1 */ KC_NO,ST_N6,ST_N7,ST_N8,ST_N9,ST_NA,ST_NB,
/* right-2 */ ST_ST3,ST_FR,ST_PR,ST_LR,ST_TR,ST_DR,
/* right-3 */ KC_NO,ST_ST4,ST_RR,ST_BR,ST_GR,ST_SR,ST_ZR,
/* right-4 */ ST_E,ST_U,KC_NO,KC_NO,KC_NO,
/* right-thumb-0 */ KC_NO,KC_NO,
/* right-thumb-1 */ KC_NO,
/* right-thumb-2 */ KC_NO,ST_E,ST_U
)
  
};



void matrix_init_user(void) {
  #ifdef STENO_ENABLE
    steno_set_mode(STENO_MODE_GEMINI);
  #endif

  ergodox_led_all_set(LED_BRIGHTNESS_LO);
}

void matrix_scan_user(void) {
  turboTick();

  appSwitchTimeout();

}

void leader_end_user(void) {
  if (leader_sequence_one_key(KC_U)) {
    run_advanced(DO_LOGIN);
  } else if (leader_sequence_one_key(KC_P)) {
    run_advanced(DO_ENPASS);
  } else if (leader_sequence_one_key(KC_S)) {
    run_advanced(DO_SCREENSHOT);
  }
}

void keyboard_post_init_user(void) {
  switch (get_unicode_input_mode()) {
    case UNICODE_MODE_MACOS:
      zored_os = ZOS_MACOS;
  }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  bool pressed = record->event.pressed;

  if (turboHandlePress(keycode, pressed)) {
    return false;
  }



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
    case UC_MAC:
      zored_os = ZOS_MACOS;
      return true;

    case UC_WINC:
      zored_os = ZOS_WINDOWS;
      return true;

    // Do keys:

      case KC_DO_LAYOUT_EN:
        run_advanced(DO_LAYOUT_EN);
        break;
  
      case KC_DO_LAYOUT_RU:
        run_advanced(DO_LAYOUT_RU);
        break;
  
      case KC_DO_FUTURE_APP:
        run_advanced(DO_FUTURE_APP);
        break;
  
      case KC_DO_COPY:
        run_advanced(DO_COPY);
        break;
  
      case KC_DO_PASTE:
        run_advanced(DO_PASTE);
        break;
  
      case KC_DO_ONE_SHOT_TURBO:
        run_advanced(DO_ONE_SHOT_TURBO);
        break;
  
      case KC_DO_PAST_APP:
        run_advanced(DO_PAST_APP);
        break;
  
      case KC_DO_PAST_WINDOW:
        run_advanced(DO_PAST_WINDOW);
        break;
  
      case KC_DO_FUTURE_WINDOW:
        run_advanced(DO_FUTURE_WINDOW);
        break;
  
      case KC_DO_PAST:
        run_advanced(DO_PAST);
        break;
  
      case KC_DO_FUTURE:
        run_advanced(DO_FUTURE);
        break;
  
      case KC_DO_BOOTLOADER:
        run_advanced(DO_BOOTLOADER);
        break;
  
      case KC_DO_FUTURE_TAB:
        run_advanced(DO_FUTURE_TAB);
        break;
  
      case KC_DO_PAST_TAB:
        run_advanced(DO_PAST_TAB);
        break;
  

    default:
      return true;
  }

  return false;
}


layer_state_t layer_state_set_user(layer_state_t state) {
  uint8_t layer = get_highest_layer(state);

  ergodox_led_all_off();
  switch (layer) {
    
      case LAYER_DEFAULT:
        combo_enable(); 
        break;
    
      case LAYER_GAME:
        combo_disable(); ergodox_right_led_3_on();
        break;
    
      case LAYER_GAMENUMBERS:
        combo_disable(); ergodox_right_led_2_on(); ergodox_right_led_3_on();
        break;
    
      case LAYER_GAMEMOUSE:
        combo_disable(); ergodox_right_led_1_on(); ergodox_right_led_3_on();
        break;
    
      case LAYER_GAMEMIRROR:
        combo_disable(); ergodox_right_led_2_on(); ergodox_right_led_3_on();
        break;
    
      case LAYER_SYMBOL:
        combo_disable(); ergodox_right_led_2_on();
        break;
    
      case LAYER_NAVIGATION:
        combo_disable(); ergodox_right_led_1_on();
        break;
    
      case LAYER_EMOJI:
        combo_disable(); ergodox_right_led_1_on(); ergodox_right_led_3_on();
        break;
    
      case LAYER_NAVIGATION2:
        combo_disable(); ergodox_right_led_3_on();
        break;
    
      case LAYER_HISTORY:
        combo_disable(); ergodox_right_led_3_on();
        break;
    
      case LAYER_STENO:
        combo_disable(); ergodox_right_led_1_on(); ergodox_right_led_2_on(); ergodox_right_led_3_on();
        break;
    
  }

  return state;
};
