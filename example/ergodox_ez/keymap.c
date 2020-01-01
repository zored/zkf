
#include "ergodox_ez.h"


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

void code_down_2(uint8_t code1, uint8_t code2) {
  register_code(map_windows_keycode(code1));
  register_code(map_windows_keycode(code2));
}

void code_up_2(uint8_t code1, uint8_t code2) {
  unregister_code(map_windows_keycode(code1));
  unregister_code(map_windows_keycode(code2));
}

void code_down_3(uint8_t code1, uint8_t code2, uint8_t code3) {
  register_code(map_windows_keycode(code1));
  register_code(map_windows_keycode(code2));
  register_code(map_windows_keycode(code3));
}

void code_up_3(uint8_t code1, uint8_t code2, uint8_t code3) {
  unregister_code(map_windows_keycode(code1));
  unregister_code(map_windows_keycode(code2));
  unregister_code(map_windows_keycode(code3));
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
      switch (zored_os) {
        case OS_WINDOWS:
          tap_code16(A(KC_LSHIFT));
          break;
        case OS_MACOS:
          tap_code16(G(KC_SPC));
          break;
      }
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
  LAYER_GAME,
  LAYER_SYMBOL,
  LAYER_NAVIGATION,
  LAYER_EMOJI,
  LAYER_NAVIGATION2
};

enum dance_keys {
  DANCE_KC_CAPSDANCE = 1,
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
  ACTION_SEQ__DO_NEXT_LANGUAGE_1 = 1,
  ACTION_SEQ__HOLD_LAYER_SYMBOL_3,
  ACTION_SEQ__HOLD_LAYER_SYMBOL__LCTRL_4,
  ACTION_SEQ__HOLD_LAYER_SYMBOL__LALT_5,
  ACTION_SEQ__Z_7,
  ACTION_SEQ__LCTRL_9,
  ACTION_SEQ__LCTRL__LSHIFT_10,
  ACTION_SEQ__LCTRL__LSHIFT__LALT_11,
  ACTION_SEQ__X_13,
  ACTION_SEQ__LALT_15,
  ACTION_SEQ__LALT__LSHIFT_16,
  ACTION_SEQ__LALT__LSHIFT__LCTRL_17,
  ACTION_SEQ__C_19,
  ACTION_SEQ__LGUI_21,
  ACTION_SEQ__LGUI__LSHIFT_22,
  ACTION_SEQ__LGUI__LSHIFT__LCTRL_23,
  ACTION_SEQ__SCOLON_25,
  ACTION_SEQ__HOLD_LAYER_NAVIGATION_27,
  ACTION_SEQ__HOLD_LAYER_NAVIGATION2_28,
  ACTION_SEQ__TOGGLE_LAYER_NAVIGATION_29,
  ACTION_SEQ__QUOT_31,
  ACTION_SEQ__HOLD_LAYER_EMOJI_33,
  ACTION_SEQ__HOLD_LAYER_EMOJI__LCTRL_34,
  ACTION_SEQ__HOLD_LAYER_EMOJI__LCTRL__LALT_35,
  ACTION_SEQ__COMM_37,
  ACTION_SEQ__RGUI_39,
  ACTION_SEQ__RGUI__RSHIFT_40,
  ACTION_SEQ__RGUI__RSHIFT__RCTRL_41,
  ACTION_SEQ__DOT_43,
  ACTION_SEQ__RALT_45,
  ACTION_SEQ__RALT__RSHIFT_46,
  ACTION_SEQ__RALT__RSHIFT__RCTRL_47,
  ACTION_SEQ__SLSH_49,
  ACTION_SEQ__RCTRL_51,
  ACTION_SEQ__RCTRL__RSHIFT_52,
  ACTION_SEQ__RCTRL__RSHIFT__RALT_53,
  ACTION_SEQ__TAB_55,
  ACTION_SEQ__HOLD_LAYER_SYMBOL_57,
  ACTION_SEQ__LSHIFT__5_59,
  ACTION_SEQ__LCTRL_61,
  ACTION_SEQ__LCTRL__LSHIFT_62,
  ACTION_SEQ__LCTRL__LSHIFT__LALT_63,
  ACTION_SEQ__LSHIFT__6_65,
  ACTION_SEQ__LALT_67,
  ACTION_SEQ__LALT__LSHIFT_68,
  ACTION_SEQ__LALT__LSHIFT__LCTRL_69,
  ACTION_SEQ__LBRC_71,
  ACTION_SEQ__LGUI_73,
  ACTION_SEQ__LGUI__LSHIFT_74,
  ACTION_SEQ__LGUI__LSHIFT__LCTRL_75,
  ACTION_SEQ__1F60A_77,
  ACTION_SEQ__1F601_78,
  ACTION_SEQ__1F604_79,
  ACTION_SEQ__1F602_80,
  ACTION_SEQ__1F61E_83,
  ACTION_SEQ__1F622_84,
  ACTION_SEQ__1F62D_85,
  ACTION_SEQ__1F914_88,
  ACTION_SEQ__1F928_89,
  ACTION_SEQ__1F631_90,
  ACTION_SEQ__1F60F_93,
  ACTION_SEQ__1F60E_94,
  ACTION_SEQ__1F4AA_95,
  ACTION_SEQ__1F525_96,
  ACTION_SEQ__1F44D_99,
  ACTION_SEQ__1F44C_100,
  ACTION_SEQ__1F197_101,
  ACTION_SEQ__1F61A_104,
  ACTION_SEQ__1F60D_105,
  ACTION_SEQ__1F618_106,
  ACTION_SEQ__1F496_107,
  ACTION_SEQ__F7_110,
  ACTION_SEQ__F17_111,
  ACTION_SEQ__F8_114,
  ACTION_SEQ__F18_115,
  ACTION_SEQ__F9_118,
  ACTION_SEQ__F19_119,
  ACTION_SEQ__F4_122,
  ACTION_SEQ__F14_123,
  ACTION_SEQ__F5_126,
  ACTION_SEQ__F15_127,
  ACTION_SEQ__F6_130,
  ACTION_SEQ__F16_131,
  ACTION_SEQ__F1_134,
  ACTION_SEQ__F11_135,
  ACTION_SEQ__F2_138,
  ACTION_SEQ__F12_139,
  ACTION_SEQ__F3_142,
  ACTION_SEQ__F13_143,
  ACTION_SEQ__F10_146,
  ACTION_SEQ__F20_147
};

static int dance_key_states[91] = {0};

void on_dance(qk_tap_dance_state_t *state, void *user_data) {
  uint16_t dance_key = state->keycode - QK_TAP_DANCE;

  if (state->count == 0) {
    return;
  }

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
      
          case 3:
            layer_on(LAYER_SYMBOL);
    code_down(KC_LALT);
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_SYMBOL__LALT_5;
            return;
      
          default:
            layer_on(LAYER_SYMBOL);
    code_down(KC_LALT);
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_SYMBOL__LALT_5;
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
    
    case DANCE_KC_ZDANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            code_down(KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL_9;
            return;
      
          case 2:
            code_down_2(KC_LCTRL, KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL__LSHIFT_10;
            return;
      
          case 3:
            code_down_3(KC_LCTRL, KC_LSHIFT, KC_LALT);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL__LSHIFT__LALT_11;
            return;
      
          default:
            code_down_3(KC_LCTRL, KC_LSHIFT, KC_LALT);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL__LSHIFT__LALT_11;
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_Z);
            dance_key_states[dance_key] = ACTION_SEQ__Z_7;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_Z);
            dance_key_states[dance_key] = ACTION_SEQ__Z_7;
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
            dance_key_states[dance_key] = ACTION_SEQ__LALT_15;
            return;
      
          case 2:
            code_down_2(KC_LALT, KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LALT__LSHIFT_16;
            return;
      
          case 3:
            code_down_3(KC_LALT, KC_LSHIFT, KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__LALT__LSHIFT__LCTRL_17;
            return;
      
          default:
            code_down_3(KC_LALT, KC_LSHIFT, KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__LALT__LSHIFT__LCTRL_17;
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_X);
            dance_key_states[dance_key] = ACTION_SEQ__X_13;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_X);
            dance_key_states[dance_key] = ACTION_SEQ__X_13;
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
            dance_key_states[dance_key] = ACTION_SEQ__LGUI_21;
            return;
      
          case 2:
            code_down_2(KC_LGUI, KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LGUI__LSHIFT_22;
            return;
      
          case 3:
            code_down_3(KC_LGUI, KC_LSHIFT, KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__LGUI__LSHIFT__LCTRL_23;
            return;
      
          default:
            code_down_3(KC_LGUI, KC_LSHIFT, KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__LGUI__LSHIFT__LCTRL_23;
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_C);
            dance_key_states[dance_key] = ACTION_SEQ__C_19;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_C);
            dance_key_states[dance_key] = ACTION_SEQ__C_19;
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
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_NAVIGATION_27;
            return;
      
          case 2:
            layer_on(LAYER_NAVIGATION2);
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_NAVIGATION2_28;
            return;
      
          case 3:
            layer_invert(LAYER_NAVIGATION);
            dance_key_states[dance_key] = ACTION_SEQ__TOGGLE_LAYER_NAVIGATION_29;
            return;
      
          default:
            layer_invert(LAYER_NAVIGATION);
            dance_key_states[dance_key] = ACTION_SEQ__TOGGLE_LAYER_NAVIGATION_29;
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_SCOLON);
            dance_key_states[dance_key] = ACTION_SEQ__SCOLON_25;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_SCOLON);
            dance_key_states[dance_key] = ACTION_SEQ__SCOLON_25;
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
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_EMOJI_33;
            return;
      
          case 2:
            layer_on(LAYER_EMOJI);
    code_down(KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_EMOJI__LCTRL_34;
            return;
      
          case 3:
            layer_on(LAYER_EMOJI);
    code_down_2(KC_LCTRL, KC_LALT);
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_EMOJI__LCTRL__LALT_35;
            return;
      
          default:
            layer_on(LAYER_EMOJI);
    code_down_2(KC_LCTRL, KC_LALT);
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_EMOJI__LCTRL__LALT_35;
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_QUOT);
            dance_key_states[dance_key] = ACTION_SEQ__QUOT_31;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_QUOT);
            dance_key_states[dance_key] = ACTION_SEQ__QUOT_31;
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
            dance_key_states[dance_key] = ACTION_SEQ__RGUI_39;
            return;
      
          case 2:
            code_down_2(KC_RGUI, KC_RSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__RGUI__RSHIFT_40;
            return;
      
          case 3:
            code_down_3(KC_RGUI, KC_RSHIFT, KC_RCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__RGUI__RSHIFT__RCTRL_41;
            return;
      
          default:
            code_down_3(KC_RGUI, KC_RSHIFT, KC_RCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__RGUI__RSHIFT__RCTRL_41;
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_COMM);
            dance_key_states[dance_key] = ACTION_SEQ__COMM_37;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_COMM);
            dance_key_states[dance_key] = ACTION_SEQ__COMM_37;
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
      
          case 3:
            code_down_3(KC_RALT, KC_RSHIFT, KC_RCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__RALT__RSHIFT__RCTRL_47;
            return;
      
          default:
            code_down_3(KC_RALT, KC_RSHIFT, KC_RCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__RALT__RSHIFT__RCTRL_47;
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
            dance_key_states[dance_key] = ACTION_SEQ__RCTRL_51;
            return;
      
          case 2:
            code_down_2(KC_RCTRL, KC_RSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__RCTRL__RSHIFT_52;
            return;
      
          case 3:
            code_down_3(KC_RCTRL, KC_RSHIFT, KC_RALT);
            dance_key_states[dance_key] = ACTION_SEQ__RCTRL__RSHIFT__RALT_53;
            return;
      
          default:
            code_down_3(KC_RCTRL, KC_RSHIFT, KC_RALT);
            dance_key_states[dance_key] = ACTION_SEQ__RCTRL__RSHIFT__RALT_53;
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
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_SYMBOL_57;
            return;
      
          default:
            layer_on(LAYER_SYMBOL);
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_SYMBOL_57;
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_TAB);
            dance_key_states[dance_key] = ACTION_SEQ__TAB_55;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_TAB);
            dance_key_states[dance_key] = ACTION_SEQ__TAB_55;
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
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL_61;
            return;
      
          case 2:
            code_down_2(KC_LCTRL, KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL__LSHIFT_62;
            return;
      
          case 3:
            code_down_3(KC_LCTRL, KC_LSHIFT, KC_LALT);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL__LSHIFT__LALT_63;
            return;
      
          default:
            code_down_3(KC_LCTRL, KC_LSHIFT, KC_LALT);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL__LSHIFT__LALT_63;
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down_2(KC_LSHIFT, KC_5);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__5_59;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down_2(KC_LSHIFT, KC_5);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__5_59;
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
            dance_key_states[dance_key] = ACTION_SEQ__LALT_67;
            return;
      
          case 2:
            code_down_2(KC_LALT, KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LALT__LSHIFT_68;
            return;
      
          case 3:
            code_down_3(KC_LALT, KC_LSHIFT, KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__LALT__LSHIFT__LCTRL_69;
            return;
      
          default:
            code_down_3(KC_LALT, KC_LSHIFT, KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__LALT__LSHIFT__LCTRL_69;
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down_2(KC_LSHIFT, KC_6);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__6_65;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down_2(KC_LSHIFT, KC_6);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__6_65;
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
            dance_key_states[dance_key] = ACTION_SEQ__LGUI_73;
            return;
      
          case 2:
            code_down_2(KC_LGUI, KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LGUI__LSHIFT_74;
            return;
      
          case 3:
            code_down_3(KC_LGUI, KC_LSHIFT, KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__LGUI__LSHIFT__LCTRL_75;
            return;
      
          default:
            code_down_3(KC_LGUI, KC_LSHIFT, KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__LGUI__LSHIFT__LCTRL_75;
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_LBRC);
            dance_key_states[dance_key] = ACTION_SEQ__LBRC_71;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_LBRC);
            dance_key_states[dance_key] = ACTION_SEQ__LBRC_71;
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
    
            dance_key_states[dance_key] = ACTION_SEQ__1F60A_77;
            return;
      
          case 2:
            tap_unicode(EMOJI_1F601);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F601_78;
            return;
      
          case 3:
            tap_unicode(EMOJI_1F604);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F604_79;
            return;
      
          case 4:
            tap_unicode(EMOJI_1F602);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F602_80;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              tap_unicode(EMOJI_1F60A);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F60A_77;
              
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
    
            dance_key_states[dance_key] = ACTION_SEQ__1F61E_83;
            return;
      
          case 2:
            tap_unicode(EMOJI_1F622);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F622_84;
            return;
      
          case 3:
            tap_unicode(EMOJI_1F62D);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F62D_85;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              tap_unicode(EMOJI_1F61E);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F61E_83;
              
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
    
            dance_key_states[dance_key] = ACTION_SEQ__1F914_88;
            return;
      
          case 2:
            tap_unicode(EMOJI_1F928);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F928_89;
            return;
      
          case 3:
            tap_unicode(EMOJI_1F631);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F631_90;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              tap_unicode(EMOJI_1F914);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F914_88;
              
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
    
            dance_key_states[dance_key] = ACTION_SEQ__1F60F_93;
            return;
      
          case 2:
            tap_unicode(EMOJI_1F60E);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F60E_94;
            return;
      
          case 3:
            tap_unicode(EMOJI_1F4AA);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F4AA_95;
            return;
      
          case 4:
            tap_unicode(EMOJI_1F525);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F525_96;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              tap_unicode(EMOJI_1F60F);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F60F_93;
              
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
    
            dance_key_states[dance_key] = ACTION_SEQ__1F44D_99;
            return;
      
          case 2:
            tap_unicode(EMOJI_1F44C);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F44C_100;
            return;
      
          case 3:
            tap_unicode(EMOJI_1F197);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F197_101;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              tap_unicode(EMOJI_1F44D);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F44D_99;
              
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
    
            dance_key_states[dance_key] = ACTION_SEQ__1F61A_104;
            return;
      
          case 2:
            tap_unicode(EMOJI_1F60D);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F60D_105;
            return;
      
          case 3:
            tap_unicode(EMOJI_1F618);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F618_106;
            return;
      
          case 4:
            tap_unicode(EMOJI_1F496);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F496_107;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              tap_unicode(EMOJI_1F61A);
    
            dance_key_states[dance_key] = ACTION_SEQ__1F61A_104;
              
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
            dance_key_states[dance_key] = ACTION_SEQ__F7_110;
            return;
      
          case 2:
            code_down(KC_F17);
            dance_key_states[dance_key] = ACTION_SEQ__F17_111;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F7);
            dance_key_states[dance_key] = ACTION_SEQ__F7_110;
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
            dance_key_states[dance_key] = ACTION_SEQ__F8_114;
            return;
      
          case 2:
            code_down(KC_F18);
            dance_key_states[dance_key] = ACTION_SEQ__F18_115;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F8);
            dance_key_states[dance_key] = ACTION_SEQ__F8_114;
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
            dance_key_states[dance_key] = ACTION_SEQ__F9_118;
            return;
      
          case 2:
            code_down(KC_F19);
            dance_key_states[dance_key] = ACTION_SEQ__F19_119;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F9);
            dance_key_states[dance_key] = ACTION_SEQ__F9_118;
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
            dance_key_states[dance_key] = ACTION_SEQ__F4_122;
            return;
      
          case 2:
            code_down(KC_F14);
            dance_key_states[dance_key] = ACTION_SEQ__F14_123;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F4);
            dance_key_states[dance_key] = ACTION_SEQ__F4_122;
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
            dance_key_states[dance_key] = ACTION_SEQ__F5_126;
            return;
      
          case 2:
            code_down(KC_F15);
            dance_key_states[dance_key] = ACTION_SEQ__F15_127;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F5);
            dance_key_states[dance_key] = ACTION_SEQ__F5_126;
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
            dance_key_states[dance_key] = ACTION_SEQ__F6_130;
            return;
      
          case 2:
            code_down(KC_F16);
            dance_key_states[dance_key] = ACTION_SEQ__F16_131;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F6);
            dance_key_states[dance_key] = ACTION_SEQ__F6_130;
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
            dance_key_states[dance_key] = ACTION_SEQ__F1_134;
            return;
      
          case 2:
            code_down(KC_F11);
            dance_key_states[dance_key] = ACTION_SEQ__F11_135;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F1);
            dance_key_states[dance_key] = ACTION_SEQ__F1_134;
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
            dance_key_states[dance_key] = ACTION_SEQ__F2_138;
            return;
      
          case 2:
            code_down(KC_F12);
            dance_key_states[dance_key] = ACTION_SEQ__F12_139;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F2);
            dance_key_states[dance_key] = ACTION_SEQ__F2_138;
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
            dance_key_states[dance_key] = ACTION_SEQ__F3_142;
            return;
      
          case 2:
            code_down(KC_F13);
            dance_key_states[dance_key] = ACTION_SEQ__F13_143;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F3);
            dance_key_states[dance_key] = ACTION_SEQ__F3_142;
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
            dance_key_states[dance_key] = ACTION_SEQ__F10_146;
            return;
      
          case 2:
            code_down(KC_F20);
            dance_key_states[dance_key] = ACTION_SEQ__F20_147;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F10);
            dance_key_states[dance_key] = ACTION_SEQ__F10_146;
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
    
          case ACTION_SEQ__HOLD_LAYER_SYMBOL__LALT_5:
            layer_off(LAYER_SYMBOL);code_up(KC_LALT);
            break;
        case ACTION_SEQ__DO_NEXT_LANGUAGE_1:
            run_advanced(DO_NEXT_LANGUAGE);
            break;
    
        case ACTION_SEQ__LCTRL_9:
            code_up(KC_LCTRL);
            break;
    
          case ACTION_SEQ__LCTRL__LSHIFT_10:
            code_up_2(KC_LCTRL, KC_LSHIFT);
            break;
    
          case ACTION_SEQ__LCTRL__LSHIFT__LALT_11:
            code_up_3(KC_LCTRL, KC_LSHIFT, KC_LALT);
            break;
        case ACTION_SEQ__Z_7:
            code_up(KC_Z);
            break;
    
        case ACTION_SEQ__LALT_15:
            code_up(KC_LALT);
            break;
    
          case ACTION_SEQ__LALT__LSHIFT_16:
            code_up_2(KC_LALT, KC_LSHIFT);
            break;
    
          case ACTION_SEQ__LALT__LSHIFT__LCTRL_17:
            code_up_3(KC_LALT, KC_LSHIFT, KC_LCTRL);
            break;
        case ACTION_SEQ__X_13:
            code_up(KC_X);
            break;
    
        case ACTION_SEQ__LGUI_21:
            code_up(KC_LGUI);
            break;
    
          case ACTION_SEQ__LGUI__LSHIFT_22:
            code_up_2(KC_LGUI, KC_LSHIFT);
            break;
    
          case ACTION_SEQ__LGUI__LSHIFT__LCTRL_23:
            code_up_3(KC_LGUI, KC_LSHIFT, KC_LCTRL);
            break;
        case ACTION_SEQ__C_19:
            code_up(KC_C);
            break;
    
        case ACTION_SEQ__HOLD_LAYER_NAVIGATION_27:
            layer_off(LAYER_NAVIGATION);
            break;
    
          case ACTION_SEQ__HOLD_LAYER_NAVIGATION2_28:
            layer_off(LAYER_NAVIGATION2);
            break;
        case ACTION_SEQ__SCOLON_25:
            code_up(KC_SCOLON);
            break;
    
        case ACTION_SEQ__HOLD_LAYER_EMOJI_33:
            layer_off(LAYER_EMOJI);
            break;
    
          case ACTION_SEQ__HOLD_LAYER_EMOJI__LCTRL_34:
            layer_off(LAYER_EMOJI);code_up(KC_LCTRL);
            break;
    
          case ACTION_SEQ__HOLD_LAYER_EMOJI__LCTRL__LALT_35:
            layer_off(LAYER_EMOJI);code_up_2(KC_LCTRL, KC_LALT);
            break;
        case ACTION_SEQ__QUOT_31:
            code_up(KC_QUOT);
            break;
    
        case ACTION_SEQ__RGUI_39:
            code_up(KC_RGUI);
            break;
    
          case ACTION_SEQ__RGUI__RSHIFT_40:
            code_up_2(KC_RGUI, KC_RSHIFT);
            break;
    
          case ACTION_SEQ__RGUI__RSHIFT__RCTRL_41:
            code_up_3(KC_RGUI, KC_RSHIFT, KC_RCTRL);
            break;
        case ACTION_SEQ__COMM_37:
            code_up(KC_COMM);
            break;
    
        case ACTION_SEQ__RALT_45:
            code_up(KC_RALT);
            break;
    
          case ACTION_SEQ__RALT__RSHIFT_46:
            code_up_2(KC_RALT, KC_RSHIFT);
            break;
    
          case ACTION_SEQ__RALT__RSHIFT__RCTRL_47:
            code_up_3(KC_RALT, KC_RSHIFT, KC_RCTRL);
            break;
        case ACTION_SEQ__DOT_43:
            code_up(KC_DOT);
            break;
    
        case ACTION_SEQ__RCTRL_51:
            code_up(KC_RCTRL);
            break;
    
          case ACTION_SEQ__RCTRL__RSHIFT_52:
            code_up_2(KC_RCTRL, KC_RSHIFT);
            break;
    
          case ACTION_SEQ__RCTRL__RSHIFT__RALT_53:
            code_up_3(KC_RCTRL, KC_RSHIFT, KC_RALT);
            break;
        case ACTION_SEQ__SLSH_49:
            code_up(KC_SLSH);
            break;
    
        case ACTION_SEQ__HOLD_LAYER_SYMBOL_57:
            layer_off(LAYER_SYMBOL);
            break;
        case ACTION_SEQ__TAB_55:
            code_up(KC_TAB);
            break;
    
        case ACTION_SEQ__LCTRL_61:
            code_up(KC_LCTRL);
            break;
    
          case ACTION_SEQ__LCTRL__LSHIFT_62:
            code_up_2(KC_LCTRL, KC_LSHIFT);
            break;
    
          case ACTION_SEQ__LCTRL__LSHIFT__LALT_63:
            code_up_3(KC_LCTRL, KC_LSHIFT, KC_LALT);
            break;
        case ACTION_SEQ__LSHIFT__5_59:
            code_up_2(KC_LSHIFT, KC_5);
            break;
    
        case ACTION_SEQ__LALT_67:
            code_up(KC_LALT);
            break;
    
          case ACTION_SEQ__LALT__LSHIFT_68:
            code_up_2(KC_LALT, KC_LSHIFT);
            break;
    
          case ACTION_SEQ__LALT__LSHIFT__LCTRL_69:
            code_up_3(KC_LALT, KC_LSHIFT, KC_LCTRL);
            break;
        case ACTION_SEQ__LSHIFT__6_65:
            code_up_2(KC_LSHIFT, KC_6);
            break;
    
        case ACTION_SEQ__LGUI_73:
            code_up(KC_LGUI);
            break;
    
          case ACTION_SEQ__LGUI__LSHIFT_74:
            code_up_2(KC_LGUI, KC_LSHIFT);
            break;
    
          case ACTION_SEQ__LGUI__LSHIFT__LCTRL_75:
            code_up_3(KC_LGUI, KC_LSHIFT, KC_LCTRL);
            break;
        case ACTION_SEQ__LBRC_71:
            code_up(KC_LBRC);
            break;
    
        
        case ACTION_SEQ__F7_110:
            code_up(KC_F7);
            break;
    
          case ACTION_SEQ__F17_111:
            code_up(KC_F17);
            break;
    
        
        case ACTION_SEQ__F8_114:
            code_up(KC_F8);
            break;
    
          case ACTION_SEQ__F18_115:
            code_up(KC_F18);
            break;
    
        
        case ACTION_SEQ__F9_118:
            code_up(KC_F9);
            break;
    
          case ACTION_SEQ__F19_119:
            code_up(KC_F19);
            break;
    
        
        case ACTION_SEQ__F4_122:
            code_up(KC_F4);
            break;
    
          case ACTION_SEQ__F14_123:
            code_up(KC_F14);
            break;
    
        
        case ACTION_SEQ__F5_126:
            code_up(KC_F5);
            break;
    
          case ACTION_SEQ__F15_127:
            code_up(KC_F15);
            break;
    
        
        case ACTION_SEQ__F6_130:
            code_up(KC_F6);
            break;
    
          case ACTION_SEQ__F16_131:
            code_up(KC_F16);
            break;
    
        
        case ACTION_SEQ__F1_134:
            code_up(KC_F1);
            break;
    
          case ACTION_SEQ__F11_135:
            code_up(KC_F11);
            break;
    
        
        case ACTION_SEQ__F2_138:
            code_up(KC_F2);
            break;
    
          case ACTION_SEQ__F12_139:
            code_up(KC_F12);
            break;
    
        
        case ACTION_SEQ__F3_142:
            code_up(KC_F3);
            break;
    
          case ACTION_SEQ__F13_143:
            code_up(KC_F13);
            break;
    
        
        case ACTION_SEQ__F10_146:
            code_up(KC_F10);
            break;
    
          case ACTION_SEQ__F20_147:
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
#define DANCE_MODIFIER() ACTION_TAP_DANCE_FN_ADVANCED_TIME(NULL, on_dance, on_dance_reset, TAPPING_TERM_TAP_DANCE)
#define DANCE_TWO(k1,k11) ACTION_TAP_DANCE_DOUBLE_TIME(k1, k11, TAPPING_TERM_TAP_DANCE)

qk_tap_dance_action_t tap_dance_actions[] = {
    [DANCE_KC_CAPSDANCE] = DANCE_MODIFIER(),
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

enum custom_keycodes {
  ZKC_BTL = SAFE_RANGE,

  // At the end:
  DYNAMIC_MACRO_RANGE,
};

#include "dynamic_macro.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[LAYER_DEFAULT] = LAYOUT_ergodox(
  
/* left-0 */ KC_ESC,KC_1,KC_2,KC_3,KC_4,KC_5,_______,
/* left-1 */ KC_ESC,KC_Q,KC_W,KC_E,KC_R,KC_T,_______,
/* left-2 */ TD(DANCE_KC_CAPSDANCE),KC_A,KC_S,KC_D,KC_F,KC_G,
/* left-3 */ KC_LSPO,TD(DANCE_KC_ZDANCE),TD(DANCE_KC_XDANCE),TD(DANCE_KC_CDANCE),KC_V,KC_B,_______,
/* left-4 */ KC_LEAD,TG(LAYER_NAVIGATION),_______,KC_LEFT,KC_RGHT,
/* left-thumb-0 */ KC_ESC,_______,
/* left-thumb-1 */ KC_HOME,
/* left-thumb-2 */ KC_SPC,KC_BSPC,KC_END,
/* right-0 */ _______,KC_6,KC_7,KC_8,KC_9,KC_0,_______,
/* right-1 */ _______,KC_Y,KC_U,KC_I,KC_O,KC_P,KC_BSLS,
/* right-2 */ KC_H,KC_J,KC_K,KC_L,TD(DANCE_KC_SEMICOLONDANCE),TD(DANCE_KC_QUOTEDANCE),
/* right-3 */ _______,KC_N,KC_M,TD(DANCE_KC_COMMADANCE),TD(DANCE_KC_DOTDANCE),TD(DANCE_KC_SLASHDANCE),KC_RSPC,
/* right-4 */ KC_LBRC,KC_RBRC,_______,_______,TG(LAYER_GAME),
/* right-thumb-0 */ KC_ESC,_______,
/* right-thumb-1 */ KC_PGUP,
/* right-thumb-2 */ KC_PGDN,TD(DANCE_KC_TABDANCE),KC_ENT
)
  ,

[LAYER_GAME] = LAYOUT_ergodox(
  
/* left-0 */ KC_ESC,KC_1,KC_2,KC_3,KC_4,KC_5,_______,
/* left-1 */ KC_TAB,_______,_______,_______,_______,_______,_______,
/* left-2 */ KC_CAPSLOCK,_______,_______,_______,_______,_______,
/* left-3 */ KC_LSHIFT,KC_Z,KC_X,KC_C,_______,_______,_______,
/* left-4 */ KC_LGUI,KC_LALT,KC_LCTRL,_______,_______,
/* left-thumb-0 */ _______,_______,
/* left-thumb-1 */ _______,
/* left-thumb-2 */ _______,_______,_______,
/* right-0 */ _______,_______,_______,_______,_______,_______,_______,
/* right-1 */ _______,_______,_______,_______,_______,_______,_______,
/* right-2 */ _______,_______,_______,_______,KC_SCOLON,KC_QUOT,
/* right-3 */ _______,_______,_______,KC_COMM,KC_DOT,KC_SLSH,KC_RSHIFT,
/* right-4 */ _______,_______,_______,_______,_______,
/* right-thumb-0 */ _______,_______,
/* right-thumb-1 */ _______,
/* right-thumb-2 */ _______,KC_TAB,_______
)
  ,

[LAYER_SYMBOL] = LAYOUT_ergodox(
  
/* left-0 */ _______,_______,_______,_______,_______,_______,_______,
/* left-1 */ _______,KC_EXLM,KC_HASH,KC_LCBR,KC_RCBR,KC_SLSH,_______,
/* left-2 */ _______,KC_AT,KC_DLR,KC_LPRN,KC_RPRN,KC_GRV,
/* left-3 */ _______,TD(DANCE_KC_PERCENTDANCE),TD(DANCE_KC_CIRCUMFLEXDANCE),TD(DANCE_KC_LEFTSQUAREBRACKETDANCE),KC_RBRC,KC_TILD,_______,
/* left-4 */ _______,_______,_______,_______,_______,
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
  
/* left-0 */ _______,_______,_______,_______,_______,_______,_______,
/* left-1 */ _______,_______,KC_BTN2,KC_MS_U,KC_BTN1,_______,_______,
/* left-2 */ _______,_______,KC_MS_L,KC_MS_D,KC_MS_R,_______,
/* left-3 */ _______,_______,_______,_______,_______,_______,_______,
/* left-4 */ _______,_______,_______,_______,_______,
/* left-thumb-0 */ _______,_______,
/* left-thumb-1 */ _______,
/* left-thumb-2 */ _______,_______,_______,
/* right-0 */ _______,_______,_______,_______,_______,_______,_______,
/* right-1 */ _______,_______,KC_BRID,KC_BRIU,_______,_______,_______,
/* right-2 */ KC_LEFT,KC_DOWN,KC_UP,KC_RGHT,_______,_______,
/* right-3 */ _______,KC_WH_L,KC_WH_D,KC_WH_U,KC_WH_R,_______,_______,
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
/* left-3 */ _______,_______,_______,_______,_______,_______,_______,
/* left-4 */ ZKC_BTL,_______,_______,_______,_______,
/* left-thumb-0 */ _______,_______,
/* left-thumb-1 */ _______,
/* left-thumb-2 */ _______,_______,_______,
/* right-0 */ _______,DYN_REC_START1,DYN_REC_START2,DYN_REC_STOP,DYN_MACRO_PLAY1,DYN_MACRO_PLAY2,_______,
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
/* right-1 */ _______,KC_SYSTEM_POWER,KC_SYSTEM_SLEEP,UC_M_WC,UC_M_OS,_______,_______,
/* right-2 */ KC_HOME,KC_PGDN,KC_PGUP,KC_END,_______,_______,
/* right-3 */ _______,KC_MEDIA_PREV_TRACK,_______,KC_MEDIA_PLAY_PAUSE,KC_MEDIA_NEXT_TRACK,_______,_______,
/* right-4 */ _______,_______,_______,_______,_______,
/* right-thumb-0 */ _______,_______,
/* right-thumb-1 */ _______,
/* right-thumb-2 */ _______,_______,_______
)
  
};



void matrix_init_user(void) {
  #ifdef STENO_ENABLE
    steno_set_mode(STENO_MODE_GEMINI);
  #endif

  ergodox_led_all_set(LED_BRIGHTNESS_LO);
}

void keyboard_post_init_user(void) {
  switch (get_unicode_input_mode()) {
    case UC_OSX:
      zored_os = OS_MACOS;
  }
}

LEADER_EXTERNS();
void matrix_scan_user(void) {
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

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (!process_record_dynamic_macro(keycode, record)) {
    return false;
  }

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

uint32_t layer_state_set_user(uint32_t state) {
  uint8_t layer = biton32(state);

  ergodox_led_all_off();
  switch (layer) {
    
      case LAYER_DEFAULT:
        combo_enable(); 
        break;
    
      case LAYER_GAME:
        combo_disable(); ergodox_right_led_on(3);
        break;
    
      case LAYER_SYMBOL:
        combo_disable(); ergodox_right_led_on(2);
        break;
    
      case LAYER_NAVIGATION:
        combo_disable(); ergodox_right_led_on(1);
        break;
    
      case LAYER_EMOJI:
        combo_disable(); ergodox_right_led_on(1); ergodox_right_led_on(3);
        break;
    
      case LAYER_NAVIGATION2:
        combo_disable(); ergodox_right_led_on(3);
        break;
    
  }

  return state;
};

