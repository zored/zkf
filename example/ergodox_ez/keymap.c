
#include "ergodox_ez.h"


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

void close_app(void) {
  switch (zored_os) {
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
LAYER_DEFAULT = 0,
  LAYER_SYMBOL,
  LAYER_NAVIGATION,
  LAYER_EMOJI,
  LAYER_NAVIGATION2,
  LAYER_PLOVER
};

enum dance_keys {
  DANCE_KC_CAPSDANCE = 1,
  DANCE_KC_LSHIFTDANCE,
  DANCE_KC_ZDANCE,
  DANCE_KC_XDANCE,
  DANCE_KC_CDANCE,
  DANCE_KC_SEMICOLONDANCE,
  DANCE_KC_QUOTEDANCE,
  DANCE_KC_COMMADANCE,
  DANCE_KC_DOTDANCE,
  DANCE_KC_SLASHDANCE,
  DANCE_KC_RSHIFTDANCE,
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
  ACTION_SEQ__CAPSLOCK_1 = 1,
  ACTION_SEQ__TOGGLE_LAYER_SYMBOL_2,
  ACTION_SEQ__HOLD_LAYER_SYMBOL_4,
  ACTION_SEQ__HOLD_LAYER_SYMBOL__LCTRL_5,
  ACTION_SEQ__HOLD_LAYER_SYMBOL__LALT_6,
  ACTION_SEQ__LSHIFT__9_8,
  ACTION_SEQ__LSHIFT__LBRC_9,
  ACTION_SEQ__LBRC_10,
  ACTION_SEQ__LSHIFT_12,
  ACTION_SEQ__LSHIFT__LALT_13,
  ACTION_SEQ__LSHIFT__LALT__LGUI_14,
  ACTION_SEQ__Z_16,
  ACTION_SEQ__LCTRL_18,
  ACTION_SEQ__LCTRL__LSHIFT_19,
  ACTION_SEQ__LCTRL__LSHIFT__LALT_20,
  ACTION_SEQ__X_22,
  ACTION_SEQ__LALT_24,
  ACTION_SEQ__LALT__LSHIFT_25,
  ACTION_SEQ__LALT__LSHIFT__LCTRL_26,
  ACTION_SEQ__C_28,
  ACTION_SEQ__LGUI_30,
  ACTION_SEQ__LGUI__LSHIFT_31,
  ACTION_SEQ__LGUI__LSHIFT__LCTRL_32,
  ACTION_SEQ__SCOLON_34,
  ACTION_SEQ__HOLD_LAYER_NAVIGATION_36,
  ACTION_SEQ__HOLD_LAYER_NAVIGATION__HOLD_LAYER_NAVIGATION2_37,
  ACTION_SEQ__QUOT_39,
  ACTION_SEQ__HOLD_LAYER_EMOJI_41,
  ACTION_SEQ__HOLD_LAYER_EMOJI__LCTRL_42,
  ACTION_SEQ__HOLD_LAYER_EMOJI__LCTRL__LALT_43,
  ACTION_SEQ__COMM_45,
  ACTION_SEQ__RGUI_47,
  ACTION_SEQ__RGUI__RSHIFT_48,
  ACTION_SEQ__RGUI__RSHIFT__RCTRL_49,
  ACTION_SEQ__DOT_51,
  ACTION_SEQ__RALT_53,
  ACTION_SEQ__RALT__RSHIFT_54,
  ACTION_SEQ__RALT__RSHIFT__RCTRL_55,
  ACTION_SEQ__SLSH_57,
  ACTION_SEQ__RCTRL_59,
  ACTION_SEQ__RCTRL__RSHIFT_60,
  ACTION_SEQ__RCTRL__RSHIFT__RALT_61,
  ACTION_SEQ__LSHIFT__0_63,
  ACTION_SEQ__LSHIFT__RBRC_64,
  ACTION_SEQ__RBRC_65,
  ACTION_SEQ__RSHIFT_67,
  ACTION_SEQ__RSHIFT__RALT_68,
  ACTION_SEQ__RSHIFT__RALT__RGUI_69,
  ACTION_SEQ__TAB_71,
  ACTION_SEQ__HOLD_LAYER_SYMBOL_73,
  ACTION_SEQ__LSHIFT__5_75,
  ACTION_SEQ__LCTRL_77,
  ACTION_SEQ__LCTRL__LSHIFT_78,
  ACTION_SEQ__LCTRL__LSHIFT__LALT_79,
  ACTION_SEQ__LSHIFT__6_81,
  ACTION_SEQ__LALT_83,
  ACTION_SEQ__LALT__LSHIFT_84,
  ACTION_SEQ__LALT__LSHIFT__LCTRL_85,
  ACTION_SEQ__LBRC_87,
  ACTION_SEQ__LGUI_89,
  ACTION_SEQ__LGUI__LSHIFT_90,
  ACTION_SEQ__LGUI__LSHIFT__LCTRL_91,
  ACTION_SEQ__1F60A_93,
  ACTION_SEQ__1F601_94,
  ACTION_SEQ__1F604_95,
  ACTION_SEQ__1F602_96,
  ACTION_SEQ__1F61E_99,
  ACTION_SEQ__1F622_100,
  ACTION_SEQ__1F62D_101,
  ACTION_SEQ__1F914_104,
  ACTION_SEQ__1F928_105,
  ACTION_SEQ__1F631_106,
  ACTION_SEQ__1F60F_109,
  ACTION_SEQ__1F60E_110,
  ACTION_SEQ__1F4AA_111,
  ACTION_SEQ__1F525_112,
  ACTION_SEQ__1F44D_115,
  ACTION_SEQ__1F44C_116,
  ACTION_SEQ__1F197_117,
  ACTION_SEQ__1F61A_120,
  ACTION_SEQ__1F60D_121,
  ACTION_SEQ__1F618_122,
  ACTION_SEQ__1F496_123,
  ACTION_SEQ__F7_126,
  ACTION_SEQ__F17_127,
  ACTION_SEQ__F8_130,
  ACTION_SEQ__F18_131,
  ACTION_SEQ__F9_134,
  ACTION_SEQ__F19_135,
  ACTION_SEQ__F4_138,
  ACTION_SEQ__F14_139,
  ACTION_SEQ__F5_142,
  ACTION_SEQ__F15_143,
  ACTION_SEQ__F6_146,
  ACTION_SEQ__F16_147,
  ACTION_SEQ__F1_150,
  ACTION_SEQ__F11_151,
  ACTION_SEQ__F2_154,
  ACTION_SEQ__F12_155,
  ACTION_SEQ__F3_158,
  ACTION_SEQ__F13_159,
  ACTION_SEQ__F10_162,
  ACTION_SEQ__F20_163
};

static int dance_key_states[103] = {0};

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
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_SYMBOL_4;
    
            return;
      
          case 2:
            layer_on(LAYER_SYMBOL);
            code_down(KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_SYMBOL__LCTRL_5;
    
            return;
      
          case 3:
            layer_on(LAYER_SYMBOL);
            code_down(KC_LALT);
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_SYMBOL__LALT_6;
    
            return;
      

          default:
            
          layer_on(LAYER_SYMBOL);
            code_down(KC_LALT);
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_SYMBOL__LALT_6;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_CAPSLOCK);
            dance_key_states[dance_key] = ACTION_SEQ__CAPSLOCK_1;
    
            return;
      
          case 2:
            layer_invert(LAYER_SYMBOL);
            dance_key_states[dance_key] = ACTION_SEQ__TOGGLE_LAYER_SYMBOL_2;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_CAPSLOCK);
            dance_key_states[dance_key] = ACTION_SEQ__CAPSLOCK_1;
    
            code_up(KC_CAPSLOCK);
            dance_key_states[dance_key] = 0;
    
          }
        
            return;
        }
    
      }
      break;
    
    case DANCE_KC_LSHIFTDANCE:
      if (state->pressed) {
        // Hold actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT_12;
    
            return;
      
          case 2:
            code_down(KC_LSHIFT);
            code_down(KC_LALT);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__LALT_13;
    
            return;
      
          case 3:
            code_down(KC_LSHIFT);
            code_down(KC_LALT);
            code_down(KC_LGUI);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__LALT__LGUI_14;
    
            return;
      

          default:
            
          code_down(KC_LSHIFT);
            code_down(KC_LALT);
            code_down(KC_LGUI);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__LALT__LGUI_14;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_LSHIFT);
            code_down(KC_9);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__9_8;
    
            return;
      
          case 2:
            code_down(KC_LSHIFT);
            code_down(KC_LBRC);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__LBRC_9;
    
            return;
      
          case 3:
            code_down(KC_LBRC);
            dance_key_states[dance_key] = ACTION_SEQ__LBRC_10;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_LSHIFT);
            code_down(KC_9);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__9_8;
    
            code_up(KC_LSHIFT);
            code_up(KC_9);
            dance_key_states[dance_key] = 0;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL_18;
    
            return;
      
          case 2:
            code_down(KC_LCTRL);
            code_down(KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL__LSHIFT_19;
    
            return;
      
          case 3:
            code_down(KC_LCTRL);
            code_down(KC_LSHIFT);
            code_down(KC_LALT);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL__LSHIFT__LALT_20;
    
            return;
      

          default:
            
          code_down(KC_LCTRL);
            code_down(KC_LSHIFT);
            code_down(KC_LALT);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL__LSHIFT__LALT_20;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_Z);
            dance_key_states[dance_key] = ACTION_SEQ__Z_16;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_Z);
            dance_key_states[dance_key] = ACTION_SEQ__Z_16;
    
            code_up(KC_Z);
            dance_key_states[dance_key] = 0;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__LALT_24;
    
            return;
      
          case 2:
            code_down(KC_LALT);
            code_down(KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LALT__LSHIFT_25;
    
            return;
      
          case 3:
            code_down(KC_LALT);
            code_down(KC_LSHIFT);
            code_down(KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__LALT__LSHIFT__LCTRL_26;
    
            return;
      

          default:
            
          code_down(KC_LALT);
            code_down(KC_LSHIFT);
            code_down(KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__LALT__LSHIFT__LCTRL_26;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_X);
            dance_key_states[dance_key] = ACTION_SEQ__X_22;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_X);
            dance_key_states[dance_key] = ACTION_SEQ__X_22;
    
            code_up(KC_X);
            dance_key_states[dance_key] = 0;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__LGUI_30;
    
            return;
      
          case 2:
            code_down(KC_LGUI);
            code_down(KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LGUI__LSHIFT_31;
    
            return;
      
          case 3:
            code_down(KC_LGUI);
            code_down(KC_LSHIFT);
            code_down(KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__LGUI__LSHIFT__LCTRL_32;
    
            return;
      

          default:
            
          code_down(KC_LGUI);
            code_down(KC_LSHIFT);
            code_down(KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__LGUI__LSHIFT__LCTRL_32;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_C);
            dance_key_states[dance_key] = ACTION_SEQ__C_28;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_C);
            dance_key_states[dance_key] = ACTION_SEQ__C_28;
    
            code_up(KC_C);
            dance_key_states[dance_key] = 0;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_NAVIGATION_36;
    
            return;
      
          case 2:
            layer_on(LAYER_NAVIGATION);
            
      layer_on(LAYER_NAVIGATION2);
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_NAVIGATION__HOLD_LAYER_NAVIGATION2_37;
    
            return;
      

          default:
            
          layer_on(LAYER_NAVIGATION);
            
      layer_on(LAYER_NAVIGATION2);
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_NAVIGATION__HOLD_LAYER_NAVIGATION2_37;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_SCOLON);
            dance_key_states[dance_key] = ACTION_SEQ__SCOLON_34;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_SCOLON);
            dance_key_states[dance_key] = ACTION_SEQ__SCOLON_34;
    
            code_up(KC_SCOLON);
            dance_key_states[dance_key] = 0;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_EMOJI_41;
    
            return;
      
          case 2:
            layer_on(LAYER_EMOJI);
            code_down(KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_EMOJI__LCTRL_42;
    
            return;
      
          case 3:
            layer_on(LAYER_EMOJI);
            code_down(KC_LCTRL);
            code_down(KC_LALT);
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_EMOJI__LCTRL__LALT_43;
    
            return;
      

          default:
            
          layer_on(LAYER_EMOJI);
            code_down(KC_LCTRL);
            code_down(KC_LALT);
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_EMOJI__LCTRL__LALT_43;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_QUOT);
            dance_key_states[dance_key] = ACTION_SEQ__QUOT_39;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_QUOT);
            dance_key_states[dance_key] = ACTION_SEQ__QUOT_39;
    
            code_up(KC_QUOT);
            dance_key_states[dance_key] = 0;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__RGUI_47;
    
            return;
      
          case 2:
            code_down(KC_RGUI);
            code_down(KC_RSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__RGUI__RSHIFT_48;
    
            return;
      
          case 3:
            code_down(KC_RGUI);
            code_down(KC_RSHIFT);
            code_down(KC_RCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__RGUI__RSHIFT__RCTRL_49;
    
            return;
      

          default:
            
          code_down(KC_RGUI);
            code_down(KC_RSHIFT);
            code_down(KC_RCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__RGUI__RSHIFT__RCTRL_49;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_COMM);
            dance_key_states[dance_key] = ACTION_SEQ__COMM_45;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_COMM);
            dance_key_states[dance_key] = ACTION_SEQ__COMM_45;
    
            code_up(KC_COMM);
            dance_key_states[dance_key] = 0;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__RALT_53;
    
            return;
      
          case 2:
            code_down(KC_RALT);
            code_down(KC_RSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__RALT__RSHIFT_54;
    
            return;
      
          case 3:
            code_down(KC_RALT);
            code_down(KC_RSHIFT);
            code_down(KC_RCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__RALT__RSHIFT__RCTRL_55;
    
            return;
      

          default:
            
          code_down(KC_RALT);
            code_down(KC_RSHIFT);
            code_down(KC_RCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__RALT__RSHIFT__RCTRL_55;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_DOT);
            dance_key_states[dance_key] = ACTION_SEQ__DOT_51;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_DOT);
            dance_key_states[dance_key] = ACTION_SEQ__DOT_51;
    
            code_up(KC_DOT);
            dance_key_states[dance_key] = 0;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__RCTRL_59;
    
            return;
      
          case 2:
            code_down(KC_RCTRL);
            code_down(KC_RSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__RCTRL__RSHIFT_60;
    
            return;
      
          case 3:
            code_down(KC_RCTRL);
            code_down(KC_RSHIFT);
            code_down(KC_RALT);
            dance_key_states[dance_key] = ACTION_SEQ__RCTRL__RSHIFT__RALT_61;
    
            return;
      

          default:
            
          code_down(KC_RCTRL);
            code_down(KC_RSHIFT);
            code_down(KC_RALT);
            dance_key_states[dance_key] = ACTION_SEQ__RCTRL__RSHIFT__RALT_61;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_SLSH);
            dance_key_states[dance_key] = ACTION_SEQ__SLSH_57;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_SLSH);
            dance_key_states[dance_key] = ACTION_SEQ__SLSH_57;
    
            code_up(KC_SLSH);
            dance_key_states[dance_key] = 0;
    
          }
        
            return;
        }
    
      }
      break;
    
    case DANCE_KC_RSHIFTDANCE:
      if (state->pressed) {
        // Hold actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_RSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__RSHIFT_67;
    
            return;
      
          case 2:
            code_down(KC_RSHIFT);
            code_down(KC_RALT);
            dance_key_states[dance_key] = ACTION_SEQ__RSHIFT__RALT_68;
    
            return;
      
          case 3:
            code_down(KC_RSHIFT);
            code_down(KC_RALT);
            code_down(KC_RGUI);
            dance_key_states[dance_key] = ACTION_SEQ__RSHIFT__RALT__RGUI_69;
    
            return;
      

          default:
            
          code_down(KC_RSHIFT);
            code_down(KC_RALT);
            code_down(KC_RGUI);
            dance_key_states[dance_key] = ACTION_SEQ__RSHIFT__RALT__RGUI_69;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_LSHIFT);
            code_down(KC_0);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__0_63;
    
            return;
      
          case 2:
            code_down(KC_LSHIFT);
            code_down(KC_RBRC);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__RBRC_64;
    
            return;
      
          case 3:
            code_down(KC_RBRC);
            dance_key_states[dance_key] = ACTION_SEQ__RBRC_65;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_LSHIFT);
            code_down(KC_0);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__0_63;
    
            code_up(KC_LSHIFT);
            code_up(KC_0);
            dance_key_states[dance_key] = 0;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_SYMBOL_73;
    
            return;
      

          default:
            
          layer_on(LAYER_SYMBOL);
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_SYMBOL_73;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_TAB);
            dance_key_states[dance_key] = ACTION_SEQ__TAB_71;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_TAB);
            dance_key_states[dance_key] = ACTION_SEQ__TAB_71;
    
            code_up(KC_TAB);
            dance_key_states[dance_key] = 0;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL_77;
    
            return;
      
          case 2:
            code_down(KC_LCTRL);
            code_down(KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL__LSHIFT_78;
    
            return;
      
          case 3:
            code_down(KC_LCTRL);
            code_down(KC_LSHIFT);
            code_down(KC_LALT);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL__LSHIFT__LALT_79;
    
            return;
      

          default:
            
          code_down(KC_LCTRL);
            code_down(KC_LSHIFT);
            code_down(KC_LALT);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL__LSHIFT__LALT_79;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_LSHIFT);
            code_down(KC_5);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__5_75;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_LSHIFT);
            code_down(KC_5);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__5_75;
    
            code_up(KC_LSHIFT);
            code_up(KC_5);
            dance_key_states[dance_key] = 0;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__LALT_83;
    
            return;
      
          case 2:
            code_down(KC_LALT);
            code_down(KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LALT__LSHIFT_84;
    
            return;
      
          case 3:
            code_down(KC_LALT);
            code_down(KC_LSHIFT);
            code_down(KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__LALT__LSHIFT__LCTRL_85;
    
            return;
      

          default:
            
          code_down(KC_LALT);
            code_down(KC_LSHIFT);
            code_down(KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__LALT__LSHIFT__LCTRL_85;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_LSHIFT);
            code_down(KC_6);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__6_81;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_LSHIFT);
            code_down(KC_6);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__6_81;
    
            code_up(KC_LSHIFT);
            code_up(KC_6);
            dance_key_states[dance_key] = 0;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__LGUI_89;
    
            return;
      
          case 2:
            code_down(KC_LGUI);
            code_down(KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LGUI__LSHIFT_90;
    
            return;
      
          case 3:
            code_down(KC_LGUI);
            code_down(KC_LSHIFT);
            code_down(KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__LGUI__LSHIFT__LCTRL_91;
    
            return;
      

          default:
            
          code_down(KC_LGUI);
            code_down(KC_LSHIFT);
            code_down(KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__LGUI__LSHIFT__LCTRL_91;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_LBRC);
            dance_key_states[dance_key] = ACTION_SEQ__LBRC_87;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_LBRC);
            dance_key_states[dance_key] = ACTION_SEQ__LBRC_87;
    
            code_up(KC_LBRC);
            dance_key_states[dance_key] = 0;
    
          }
        
            return;
        }
    
      }
      break;
    
    case DANCE_KC_HAPPYDANCE:
      if (state->pressed) {
        // Hold actions:
        
        switch (state->count) {
          

          default:
            
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            tap_unicode(EMOJI_1F60A);
            dance_key_states[dance_key] = ACTION_SEQ__1F60A_93;
    
            return;
      
          case 2:
            tap_unicode(EMOJI_1F601);
            dance_key_states[dance_key] = ACTION_SEQ__1F601_94;
    
            return;
      
          case 3:
            tap_unicode(EMOJI_1F604);
            dance_key_states[dance_key] = ACTION_SEQ__1F604_95;
    
            return;
      
          case 4:
            tap_unicode(EMOJI_1F602);
            dance_key_states[dance_key] = ACTION_SEQ__1F602_96;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            tap_unicode(EMOJI_1F60A);
            dance_key_states[dance_key] = ACTION_SEQ__1F60A_93;
    
            
            dance_key_states[dance_key] = 0;
    
          }
        
            return;
        }
    
      }
      break;
    
    case DANCE_KC_SADDANCE:
      if (state->pressed) {
        // Hold actions:
        
        switch (state->count) {
          

          default:
            
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            tap_unicode(EMOJI_1F61E);
            dance_key_states[dance_key] = ACTION_SEQ__1F61E_99;
    
            return;
      
          case 2:
            tap_unicode(EMOJI_1F622);
            dance_key_states[dance_key] = ACTION_SEQ__1F622_100;
    
            return;
      
          case 3:
            tap_unicode(EMOJI_1F62D);
            dance_key_states[dance_key] = ACTION_SEQ__1F62D_101;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            tap_unicode(EMOJI_1F61E);
            dance_key_states[dance_key] = ACTION_SEQ__1F61E_99;
    
            
            dance_key_states[dance_key] = 0;
    
          }
        
            return;
        }
    
      }
      break;
    
    case DANCE_KC_STRANGEDANCE:
      if (state->pressed) {
        // Hold actions:
        
        switch (state->count) {
          

          default:
            
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            tap_unicode(EMOJI_1F914);
            dance_key_states[dance_key] = ACTION_SEQ__1F914_104;
    
            return;
      
          case 2:
            tap_unicode(EMOJI_1F928);
            dance_key_states[dance_key] = ACTION_SEQ__1F928_105;
    
            return;
      
          case 3:
            tap_unicode(EMOJI_1F631);
            dance_key_states[dance_key] = ACTION_SEQ__1F631_106;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            tap_unicode(EMOJI_1F914);
            dance_key_states[dance_key] = ACTION_SEQ__1F914_104;
    
            
            dance_key_states[dance_key] = 0;
    
          }
        
            return;
        }
    
      }
      break;
    
    case DANCE_KC_COOLDANCE:
      if (state->pressed) {
        // Hold actions:
        
        switch (state->count) {
          

          default:
            
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            tap_unicode(EMOJI_1F60F);
            dance_key_states[dance_key] = ACTION_SEQ__1F60F_109;
    
            return;
      
          case 2:
            tap_unicode(EMOJI_1F60E);
            dance_key_states[dance_key] = ACTION_SEQ__1F60E_110;
    
            return;
      
          case 3:
            tap_unicode(EMOJI_1F4AA);
            dance_key_states[dance_key] = ACTION_SEQ__1F4AA_111;
    
            return;
      
          case 4:
            tap_unicode(EMOJI_1F525);
            dance_key_states[dance_key] = ACTION_SEQ__1F525_112;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            tap_unicode(EMOJI_1F60F);
            dance_key_states[dance_key] = ACTION_SEQ__1F60F_109;
    
            
            dance_key_states[dance_key] = 0;
    
          }
        
            return;
        }
    
      }
      break;
    
    case DANCE_KC_OKDANCE:
      if (state->pressed) {
        // Hold actions:
        
        switch (state->count) {
          

          default:
            
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            tap_unicode(EMOJI_1F44D);
            dance_key_states[dance_key] = ACTION_SEQ__1F44D_115;
    
            return;
      
          case 2:
            tap_unicode(EMOJI_1F44C);
            dance_key_states[dance_key] = ACTION_SEQ__1F44C_116;
    
            return;
      
          case 3:
            tap_unicode(EMOJI_1F197);
            dance_key_states[dance_key] = ACTION_SEQ__1F197_117;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            tap_unicode(EMOJI_1F44D);
            dance_key_states[dance_key] = ACTION_SEQ__1F44D_115;
    
            
            dance_key_states[dance_key] = 0;
    
          }
        
            return;
        }
    
      }
      break;
    
    case DANCE_KC_LOVEDANCE:
      if (state->pressed) {
        // Hold actions:
        
        switch (state->count) {
          

          default:
            
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            tap_unicode(EMOJI_1F61A);
            dance_key_states[dance_key] = ACTION_SEQ__1F61A_120;
    
            return;
      
          case 2:
            tap_unicode(EMOJI_1F60D);
            dance_key_states[dance_key] = ACTION_SEQ__1F60D_121;
    
            return;
      
          case 3:
            tap_unicode(EMOJI_1F618);
            dance_key_states[dance_key] = ACTION_SEQ__1F618_122;
    
            return;
      
          case 4:
            tap_unicode(EMOJI_1F496);
            dance_key_states[dance_key] = ACTION_SEQ__1F496_123;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            tap_unicode(EMOJI_1F61A);
            dance_key_states[dance_key] = ACTION_SEQ__1F61A_120;
    
            
            dance_key_states[dance_key] = 0;
    
          }
        
            return;
        }
    
      }
      break;
    
    case DANCE_KC_F7DANCE:
      if (state->pressed) {
        // Hold actions:
        
        switch (state->count) {
          

          default:
            
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_F7);
            dance_key_states[dance_key] = ACTION_SEQ__F7_126;
    
            return;
      
          case 2:
            code_down(KC_F17);
            dance_key_states[dance_key] = ACTION_SEQ__F17_127;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_F7);
            dance_key_states[dance_key] = ACTION_SEQ__F7_126;
    
            code_up(KC_F7);
            dance_key_states[dance_key] = 0;
    
          }
        
            return;
        }
    
      }
      break;
    
    case DANCE_KC_F8DANCE:
      if (state->pressed) {
        // Hold actions:
        
        switch (state->count) {
          

          default:
            
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_F8);
            dance_key_states[dance_key] = ACTION_SEQ__F8_130;
    
            return;
      
          case 2:
            code_down(KC_F18);
            dance_key_states[dance_key] = ACTION_SEQ__F18_131;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_F8);
            dance_key_states[dance_key] = ACTION_SEQ__F8_130;
    
            code_up(KC_F8);
            dance_key_states[dance_key] = 0;
    
          }
        
            return;
        }
    
      }
      break;
    
    case DANCE_KC_F9DANCE:
      if (state->pressed) {
        // Hold actions:
        
        switch (state->count) {
          

          default:
            
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_F9);
            dance_key_states[dance_key] = ACTION_SEQ__F9_134;
    
            return;
      
          case 2:
            code_down(KC_F19);
            dance_key_states[dance_key] = ACTION_SEQ__F19_135;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_F9);
            dance_key_states[dance_key] = ACTION_SEQ__F9_134;
    
            code_up(KC_F9);
            dance_key_states[dance_key] = 0;
    
          }
        
            return;
        }
    
      }
      break;
    
    case DANCE_KC_F4DANCE:
      if (state->pressed) {
        // Hold actions:
        
        switch (state->count) {
          

          default:
            
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_F4);
            dance_key_states[dance_key] = ACTION_SEQ__F4_138;
    
            return;
      
          case 2:
            code_down(KC_F14);
            dance_key_states[dance_key] = ACTION_SEQ__F14_139;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_F4);
            dance_key_states[dance_key] = ACTION_SEQ__F4_138;
    
            code_up(KC_F4);
            dance_key_states[dance_key] = 0;
    
          }
        
            return;
        }
    
      }
      break;
    
    case DANCE_KC_F5DANCE:
      if (state->pressed) {
        // Hold actions:
        
        switch (state->count) {
          

          default:
            
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_F5);
            dance_key_states[dance_key] = ACTION_SEQ__F5_142;
    
            return;
      
          case 2:
            code_down(KC_F15);
            dance_key_states[dance_key] = ACTION_SEQ__F15_143;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_F5);
            dance_key_states[dance_key] = ACTION_SEQ__F5_142;
    
            code_up(KC_F5);
            dance_key_states[dance_key] = 0;
    
          }
        
            return;
        }
    
      }
      break;
    
    case DANCE_KC_F6DANCE:
      if (state->pressed) {
        // Hold actions:
        
        switch (state->count) {
          

          default:
            
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_F6);
            dance_key_states[dance_key] = ACTION_SEQ__F6_146;
    
            return;
      
          case 2:
            code_down(KC_F16);
            dance_key_states[dance_key] = ACTION_SEQ__F16_147;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_F6);
            dance_key_states[dance_key] = ACTION_SEQ__F6_146;
    
            code_up(KC_F6);
            dance_key_states[dance_key] = 0;
    
          }
        
            return;
        }
    
      }
      break;
    
    case DANCE_KC_F1DANCE:
      if (state->pressed) {
        // Hold actions:
        
        switch (state->count) {
          

          default:
            
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_F1);
            dance_key_states[dance_key] = ACTION_SEQ__F1_150;
    
            return;
      
          case 2:
            code_down(KC_F11);
            dance_key_states[dance_key] = ACTION_SEQ__F11_151;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_F1);
            dance_key_states[dance_key] = ACTION_SEQ__F1_150;
    
            code_up(KC_F1);
            dance_key_states[dance_key] = 0;
    
          }
        
            return;
        }
    
      }
      break;
    
    case DANCE_KC_F2DANCE:
      if (state->pressed) {
        // Hold actions:
        
        switch (state->count) {
          

          default:
            
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_F2);
            dance_key_states[dance_key] = ACTION_SEQ__F2_154;
    
            return;
      
          case 2:
            code_down(KC_F12);
            dance_key_states[dance_key] = ACTION_SEQ__F12_155;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_F2);
            dance_key_states[dance_key] = ACTION_SEQ__F2_154;
    
            code_up(KC_F2);
            dance_key_states[dance_key] = 0;
    
          }
        
            return;
        }
    
      }
      break;
    
    case DANCE_KC_F3DANCE:
      if (state->pressed) {
        // Hold actions:
        
        switch (state->count) {
          

          default:
            
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_F3);
            dance_key_states[dance_key] = ACTION_SEQ__F3_158;
    
            return;
      
          case 2:
            code_down(KC_F13);
            dance_key_states[dance_key] = ACTION_SEQ__F13_159;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_F3);
            dance_key_states[dance_key] = ACTION_SEQ__F3_158;
    
            code_up(KC_F3);
            dance_key_states[dance_key] = 0;
    
          }
        
            return;
        }
    
      }
      break;
    
    case DANCE_KC_F10DANCE:
      if (state->pressed) {
        // Hold actions:
        
        switch (state->count) {
          

          default:
            
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_F10);
            dance_key_states[dance_key] = ACTION_SEQ__F10_162;
    
            return;
      
          case 2:
            code_down(KC_F20);
            dance_key_states[dance_key] = ACTION_SEQ__F20_163;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_F10);
            dance_key_states[dance_key] = ACTION_SEQ__F10_162;
    
            code_up(KC_F10);
            dance_key_states[dance_key] = 0;
    
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
    case ACTION_SEQ__HOLD_LAYER_SYMBOL_4:
            layer_off(LAYER_SYMBOL);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__HOLD_LAYER_SYMBOL__LCTRL_5:
            layer_off(LAYER_SYMBOL);
            code_up(KC_LCTRL);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__HOLD_LAYER_SYMBOL__LALT_6:
            layer_off(LAYER_SYMBOL);
            code_up(KC_LALT);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_SEQ__CAPSLOCK_1:
            code_up(KC_CAPSLOCK);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__TOGGLE_LAYER_SYMBOL_2:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
          case ACTION_SEQ__LSHIFT_12:
            code_up(KC_LSHIFT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__LSHIFT__LALT_13:
            code_up(KC_LSHIFT);
            code_up(KC_LALT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__LSHIFT__LALT__LGUI_14:
            code_up(KC_LSHIFT);
            code_up(KC_LALT);
            code_up(KC_LGUI);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_SEQ__LSHIFT__9_8:
            code_up(KC_LSHIFT);
            code_up(KC_9);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__LSHIFT__LBRC_9:
            code_up(KC_LSHIFT);
            code_up(KC_LBRC);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__LBRC_10:
            code_up(KC_LBRC);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
          case ACTION_SEQ__LCTRL_18:
            code_up(KC_LCTRL);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__LCTRL__LSHIFT_19:
            code_up(KC_LCTRL);
            code_up(KC_LSHIFT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__LCTRL__LSHIFT__LALT_20:
            code_up(KC_LCTRL);
            code_up(KC_LSHIFT);
            code_up(KC_LALT);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_SEQ__Z_16:
            code_up(KC_Z);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
          case ACTION_SEQ__LALT_24:
            code_up(KC_LALT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__LALT__LSHIFT_25:
            code_up(KC_LALT);
            code_up(KC_LSHIFT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__LALT__LSHIFT__LCTRL_26:
            code_up(KC_LALT);
            code_up(KC_LSHIFT);
            code_up(KC_LCTRL);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_SEQ__X_22:
            code_up(KC_X);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
          case ACTION_SEQ__LGUI_30:
            code_up(KC_LGUI);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__LGUI__LSHIFT_31:
            code_up(KC_LGUI);
            code_up(KC_LSHIFT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__LGUI__LSHIFT__LCTRL_32:
            code_up(KC_LGUI);
            code_up(KC_LSHIFT);
            code_up(KC_LCTRL);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_SEQ__C_28:
            code_up(KC_C);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
          case ACTION_SEQ__HOLD_LAYER_NAVIGATION_36:
            layer_off(LAYER_NAVIGATION);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__HOLD_LAYER_NAVIGATION__HOLD_LAYER_NAVIGATION2_37:
            layer_off(LAYER_NAVIGATION);
            layer_off(LAYER_NAVIGATION2);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_SEQ__SCOLON_34:
            code_up(KC_SCOLON);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
          case ACTION_SEQ__HOLD_LAYER_EMOJI_41:
            layer_off(LAYER_EMOJI);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__HOLD_LAYER_EMOJI__LCTRL_42:
            layer_off(LAYER_EMOJI);
            code_up(KC_LCTRL);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__HOLD_LAYER_EMOJI__LCTRL__LALT_43:
            layer_off(LAYER_EMOJI);
            code_up(KC_LCTRL);
            code_up(KC_LALT);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_SEQ__QUOT_39:
            code_up(KC_QUOT);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
          case ACTION_SEQ__RGUI_47:
            code_up(KC_RGUI);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__RGUI__RSHIFT_48:
            code_up(KC_RGUI);
            code_up(KC_RSHIFT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__RGUI__RSHIFT__RCTRL_49:
            code_up(KC_RGUI);
            code_up(KC_RSHIFT);
            code_up(KC_RCTRL);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_SEQ__COMM_45:
            code_up(KC_COMM);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
          case ACTION_SEQ__RALT_53:
            code_up(KC_RALT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__RALT__RSHIFT_54:
            code_up(KC_RALT);
            code_up(KC_RSHIFT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__RALT__RSHIFT__RCTRL_55:
            code_up(KC_RALT);
            code_up(KC_RSHIFT);
            code_up(KC_RCTRL);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_SEQ__DOT_51:
            code_up(KC_DOT);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
          case ACTION_SEQ__RCTRL_59:
            code_up(KC_RCTRL);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__RCTRL__RSHIFT_60:
            code_up(KC_RCTRL);
            code_up(KC_RSHIFT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__RCTRL__RSHIFT__RALT_61:
            code_up(KC_RCTRL);
            code_up(KC_RSHIFT);
            code_up(KC_RALT);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_SEQ__SLSH_57:
            code_up(KC_SLSH);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
          case ACTION_SEQ__RSHIFT_67:
            code_up(KC_RSHIFT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__RSHIFT__RALT_68:
            code_up(KC_RSHIFT);
            code_up(KC_RALT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__RSHIFT__RALT__RGUI_69:
            code_up(KC_RSHIFT);
            code_up(KC_RALT);
            code_up(KC_RGUI);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_SEQ__LSHIFT__0_63:
            code_up(KC_LSHIFT);
            code_up(KC_0);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__LSHIFT__RBRC_64:
            code_up(KC_LSHIFT);
            code_up(KC_RBRC);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__RBRC_65:
            code_up(KC_RBRC);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
          case ACTION_SEQ__HOLD_LAYER_SYMBOL_73:
            layer_off(LAYER_SYMBOL);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_SEQ__TAB_71:
            code_up(KC_TAB);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
          case ACTION_SEQ__LCTRL_77:
            code_up(KC_LCTRL);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__LCTRL__LSHIFT_78:
            code_up(KC_LCTRL);
            code_up(KC_LSHIFT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__LCTRL__LSHIFT__LALT_79:
            code_up(KC_LCTRL);
            code_up(KC_LSHIFT);
            code_up(KC_LALT);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_SEQ__LSHIFT__5_75:
            code_up(KC_LSHIFT);
            code_up(KC_5);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
          case ACTION_SEQ__LALT_83:
            code_up(KC_LALT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__LALT__LSHIFT_84:
            code_up(KC_LALT);
            code_up(KC_LSHIFT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__LALT__LSHIFT__LCTRL_85:
            code_up(KC_LALT);
            code_up(KC_LSHIFT);
            code_up(KC_LCTRL);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_SEQ__LSHIFT__6_81:
            code_up(KC_LSHIFT);
            code_up(KC_6);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
          case ACTION_SEQ__LGUI_89:
            code_up(KC_LGUI);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__LGUI__LSHIFT_90:
            code_up(KC_LGUI);
            code_up(KC_LSHIFT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__LGUI__LSHIFT__LCTRL_91:
            code_up(KC_LGUI);
            code_up(KC_LSHIFT);
            code_up(KC_LCTRL);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_SEQ__LBRC_87:
            code_up(KC_LBRC);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__1F60A_93:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F601_94:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F604_95:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F602_96:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__1F61E_99:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F622_100:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F62D_101:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__1F914_104:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F928_105:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F631_106:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__1F60F_109:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F60E_110:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F4AA_111:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F525_112:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__1F44D_115:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F44C_116:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F197_117:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__1F61A_120:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F60D_121:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F618_122:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F496_123:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__F7_126:
            code_up(KC_F7);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__F17_127:
            code_up(KC_F17);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__F8_130:
            code_up(KC_F8);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__F18_131:
            code_up(KC_F18);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__F9_134:
            code_up(KC_F9);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__F19_135:
            code_up(KC_F19);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__F4_138:
            code_up(KC_F4);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__F14_139:
            code_up(KC_F14);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__F5_142:
            code_up(KC_F5);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__F15_143:
            code_up(KC_F15);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__F6_146:
            code_up(KC_F6);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__F16_147:
            code_up(KC_F16);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__F1_150:
            code_up(KC_F1);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__F11_151:
            code_up(KC_F11);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__F2_154:
            code_up(KC_F2);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__F12_155:
            code_up(KC_F12);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__F3_158:
            code_up(KC_F3);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__F13_159:
            code_up(KC_F13);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__F10_162:
            code_up(KC_F10);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__F20_163:
            code_up(KC_F20);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
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
    [DANCE_KC_CAPSDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_LSHIFTDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_ZDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_XDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_CDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_SEMICOLONDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_QUOTEDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_COMMADANCE] = DANCE_MODIFIER(),
  [DANCE_KC_DOTDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_SLASHDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_RSHIFTDANCE] = DANCE_MODIFIER(),
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
  
/* keys-left-0 */ KC_ESC,KC_1,KC_2,KC_3,KC_4,KC_5,_______,
/* keys-left-1 */ KC_ESC,KC_Q,KC_W,KC_E,KC_R,KC_T,_______,
/* keys-left-2 */ TD(DANCE_KC_CAPSDANCE),KC_A,KC_S,KC_D,KC_F,KC_G,
/* keys-left-3 */ TD(DANCE_KC_LSHIFTDANCE),TD(DANCE_KC_ZDANCE),TD(DANCE_KC_XDANCE),TD(DANCE_KC_CDANCE),KC_V,KC_B,_______,
/* keys-left-4 */ KC_LEAD,_______,_______,KC_LEFT,KC_RGHT,
/* keys-left-thumb-0 */ KC_ESC,_______,
/* keys-left-thumb-1 */ KC_HOME,
/* keys-left-thumb-2 */ KC_SPC,KC_BSPC,KC_END,
/* keys-right-0 */ _______,KC_6,KC_7,KC_8,KC_9,KC_0,_______,
/* keys-right-1 */ _______,KC_Y,KC_U,KC_I,KC_O,KC_P,KC_BSLS,
/* keys-right-2 */ KC_H,KC_J,KC_K,KC_L,TD(DANCE_KC_SEMICOLONDANCE),TD(DANCE_KC_QUOTEDANCE),
/* keys-right-3 */ _______,KC_N,KC_M,TD(DANCE_KC_COMMADANCE),TD(DANCE_KC_DOTDANCE),TD(DANCE_KC_SLASHDANCE),TD(DANCE_KC_RSHIFTDANCE),
/* keys-right-4 */ KC_LBRC,KC_RBRC,_______,_______,TG(LAYER_PLOVER),
/* keys-right-thumb-0 */ KC_ESC,_______,
/* keys-right-thumb-1 */ KC_PGUP,
/* keys-right-thumb-2 */ KC_PGDN,TD(DANCE_KC_TABDANCE),KC_ENT
)
  ,

[LAYER_SYMBOL] = LAYOUT_ergodox(
  
/* keys-left-0 */ _______,_______,_______,_______,_______,_______,_______,
/* keys-left-1 */ _______,KC_EXLM,KC_HASH,KC_LCBR,KC_RCBR,KC_SLSH,_______,
/* keys-left-2 */ _______,KC_AT,KC_DLR,KC_LPRN,KC_RPRN,KC_GRV,
/* keys-left-3 */ _______,TD(DANCE_KC_PERCENTDANCE),TD(DANCE_KC_CIRCUMFLEXDANCE),TD(DANCE_KC_LEFTSQUAREBRACKETDANCE),KC_RBRC,KC_TILD,_______,
/* keys-left-4 */ _______,_______,_______,_______,_______,
/* keys-left-thumb-0 */ _______,_______,
/* keys-left-thumb-1 */ _______,
/* keys-left-thumb-2 */ _______,KC_DELETE,_______,
/* keys-right-0 */ _______,_______,_______,_______,_______,_______,_______,
/* keys-right-1 */ _______,KC_EQL,KC_7,KC_8,KC_9,KC_AMPR,KC_PIPE,
/* keys-right-2 */ KC_PPLS,KC_4,KC_5,KC_6,KC_PAST,_______,
/* keys-right-3 */ _______,KC_MINS,KC_1,KC_2,KC_3,KC_SLSH,_______,
/* keys-right-4 */ _______,KC_0,KC_DOT,_______,_______,
/* keys-right-thumb-0 */ _______,_______,
/* keys-right-thumb-1 */ _______,
/* keys-right-thumb-2 */ _______,_______,_______
)
  ,

[LAYER_NAVIGATION] = LAYOUT_ergodox(
  
/* keys-left-0 */ _______,_______,_______,_______,_______,_______,_______,
/* keys-left-1 */ _______,_______,KC_BTN2,KC_MS_U,KC_BTN1,_______,_______,
/* keys-left-2 */ _______,_______,KC_MS_L,KC_MS_D,KC_MS_R,_______,
/* keys-left-3 */ _______,_______,_______,_______,_______,_______,_______,
/* keys-left-4 */ _______,_______,_______,_______,_______,
/* keys-left-thumb-0 */ _______,_______,
/* keys-left-thumb-1 */ _______,
/* keys-left-thumb-2 */ _______,_______,_______,
/* keys-right-0 */ _______,_______,_______,_______,_______,_______,_______,
/* keys-right-1 */ _______,_______,KC_BRID,KC_BRIU,_______,_______,_______,
/* keys-right-2 */ KC_LEFT,KC_DOWN,KC_UP,KC_RGHT,_______,_______,
/* keys-right-3 */ _______,KC_WH_L,KC_WH_D,KC_WH_U,KC_WH_R,_______,_______,
/* keys-right-4 */ KC_VOLD,KC_VOLU,KC_MUTE,_______,_______,
/* keys-right-thumb-0 */ _______,_______,
/* keys-right-thumb-1 */ _______,
/* keys-right-thumb-2 */ _______,_______,_______
)
  ,

[LAYER_EMOJI] = LAYOUT_ergodox(
  
/* keys-left-0 */ _______,_______,_______,_______,_______,_______,_______,
/* keys-left-1 */ _______,TD(DANCE_KC_HAPPYDANCE),TD(DANCE_KC_SADDANCE),TD(DANCE_KC_STRANGEDANCE),_______,_______,_______,
/* keys-left-2 */ _______,TD(DANCE_KC_COOLDANCE),TD(DANCE_KC_OKDANCE),TD(DANCE_KC_LOVEDANCE),_______,_______,
/* keys-left-3 */ _______,_______,_______,_______,_______,_______,_______,
/* keys-left-4 */ ZKC_BTL,_______,_______,_______,_______,
/* keys-left-thumb-0 */ _______,_______,
/* keys-left-thumb-1 */ _______,
/* keys-left-thumb-2 */ _______,_______,_______,
/* keys-right-0 */ _______,DYN_REC_START1,DYN_REC_START2,DYN_REC_STOP,DYN_MACRO_PLAY1,DYN_MACRO_PLAY2,_______,
/* keys-right-1 */ _______,_______,TD(DANCE_KC_F7DANCE),TD(DANCE_KC_F8DANCE),TD(DANCE_KC_F9DANCE),_______,_______,
/* keys-right-2 */ _______,TD(DANCE_KC_F4DANCE),TD(DANCE_KC_F5DANCE),TD(DANCE_KC_F6DANCE),_______,_______,
/* keys-right-3 */ _______,_______,TD(DANCE_KC_F1DANCE),TD(DANCE_KC_F2DANCE),TD(DANCE_KC_F3DANCE),_______,_______,
/* keys-right-4 */ _______,_______,TD(DANCE_KC_F10DANCE),_______,_______,
/* keys-right-thumb-0 */ _______,_______,
/* keys-right-thumb-1 */ _______,
/* keys-right-thumb-2 */ _______,_______,_______
)
  ,

[LAYER_NAVIGATION2] = LAYOUT_ergodox(
  
/* keys-left-0 */ _______,_______,_______,_______,_______,_______,_______,
/* keys-left-1 */ _______,_______,_______,_______,_______,_______,_______,
/* keys-left-2 */ _______,_______,_______,_______,_______,_______,
/* keys-left-3 */ _______,_______,_______,_______,_______,_______,_______,
/* keys-left-4 */ _______,_______,_______,_______,_______,
/* keys-left-thumb-0 */ _______,_______,
/* keys-left-thumb-1 */ _______,
/* keys-left-thumb-2 */ _______,_______,_______,
/* keys-right-0 */ _______,_______,_______,_______,_______,_______,_______,
/* keys-right-1 */ _______,KC_SYSTEM_POWER,KC_SYSTEM_SLEEP,UC_M_WC,UC_M_OS,_______,_______,
/* keys-right-2 */ KC_HOME,KC_PGDN,KC_PGUP,KC_END,_______,_______,
/* keys-right-3 */ _______,KC_MEDIA_PREV_TRACK,_______,KC_MEDIA_PLAY_PAUSE,KC_MEDIA_NEXT_TRACK,_______,_______,
/* keys-right-4 */ _______,_______,_______,_______,_______,
/* keys-right-thumb-0 */ _______,_______,
/* keys-right-thumb-1 */ _______,
/* keys-right-thumb-2 */ _______,_______,_______
)
  ,

[LAYER_PLOVER] = LAYOUT_ergodox(
  
/* keys-left-0 */ _______,_______,_______,_______,_______,_______,_______,
/* keys-left-1 */ _______,STN_N1,STN_N2,STN_N3,STN_N4,STN_N5,_______,
/* keys-left-2 */ _______,STN_S1,STN_TL,STN_PL,STN_HL,STN_ST1,
/* keys-left-3 */ _______,STN_S2,STN_KL,STN_WL,STN_RL,STN_ST2,_______,
/* keys-left-4 */ _______,_______,_______,STN_A,STN_O,
/* keys-left-thumb-0 */ _______,_______,
/* keys-left-thumb-1 */ _______,
/* keys-left-thumb-2 */ STN_A,STN_O,_______,
/* keys-right-0 */ _______,_______,_______,_______,_______,_______,_______,
/* keys-right-1 */ _______,STN_N6,STN_N7,STN_N8,STN_N9,STN_NA,STN_NB,
/* keys-right-2 */ STN_ST3,STN_FR,STN_PR,STN_LR,STN_TR,STN_DR,
/* keys-right-3 */ _______,STN_ST4,STN_RR,STN_BR,STN_GR,STN_SR,STN_ZR,
/* keys-right-4 */ STN_E,STN_U,_______,_______,_______,
/* keys-right-thumb-0 */ _______,_______,
/* keys-right-thumb-1 */ _______,
/* keys-right-thumb-2 */ _______,STN_E,STN_U
)
  
};

void matrix_init_user(void) {
  steno_set_mode(STENO_MODE_GEMINI);
  ergodox_led_all_set(LED_BRIGHTNESS_LO);
  switch (get_unicode_input_mode()) {
    case UC_OSX:
      zored_os = OS_MACOS;
    default:
      zored_os = OS_WINDOWS;
  }
}

void spotlight_start(void) {
  register_code(KC_LGUI);
  switch (zored_os) {
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
      switch (zored_os) {
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
  ergodox_led_all_off();

  switch (biton32(state)) {
    
      case LAYER_SYMBOL:
        ergodox_right_led_on(2);
        break;
    
      case LAYER_NAVIGATION:
        ergodox_right_led_on(1);
        break;
    
      case LAYER_EMOJI:
        ergodox_right_led_on(1); ergodox_right_led_on(3);
        break;
    
      case LAYER_NAVIGATION2:
        ergodox_right_led_on(3);
        break;
    
      case LAYER_PLOVER:
        ergodox_right_led_on(3);
        break;
    
  }


  return state;
};
