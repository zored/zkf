

#include QMK_KEYBOARD_H
#include "muse.h"
#include "eeprom.h"

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
  unicode_input_start();
  register_hex32(pgm_read_dword(&unicode_map[name]));
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
  LAYER_NAVIGATIONEXTEND,
  LAYER_EMOJI,
  LAYER_PLOVER
};

enum dance_keys {
  DANCE_KC_CAPSDANCE = 1,
  DANCE_KC_SEMICOLONDANCE,
  DANCE_KC_QUOTEDANCE,
  DANCE_KC_LSHIFTDANCE,
  DANCE_KC_ZDANCE,
  DANCE_KC_XDANCE,
  DANCE_KC_CDANCE,
  DANCE_KC_COMMADANCE,
  DANCE_KC_DOTDANCE,
  DANCE_KC_SLASHDANCE,
  DANCE_KC_RSHIFTDANCE,
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
  ACTION_SEQ__CAPSLOCK_1 = 1,
  ACTION_SEQ__TOGGLE_LAYER_SYMBOL_2,
  ACTION_SEQ__HOLD_LAYER_SYMBOL_4,
  ACTION_SEQ__HOLD_LAYER_SYMBOL__LCTRL_5,
  ACTION_SEQ__HOLD_LAYER_SYMBOL__LALT_6,
  ACTION_SEQ__SCOLON_8,
  ACTION_SEQ__HOLD_LAYER_NAVIGATION_10,
  ACTION_SEQ__HOLD_LAYER_NAVIGATION__HOLD_LAYER_NAVIGATIONEXTEND_11,
  ACTION_SEQ__QUOT_13,
  ACTION_SEQ__HOLD_LAYER_EMOJI_15,
  ACTION_SEQ__HOLD_LAYER_EMOJI__LCTRL_16,
  ACTION_SEQ__HOLD_LAYER_EMOJI__LCTRL__LALT_17,
  ACTION_SEQ__LSHIFT__9_19,
  ACTION_SEQ__LSHIFT_21,
  ACTION_SEQ__LSHIFT__LALT_22,
  ACTION_SEQ__LSHIFT__LALT__LGUI_23,
  ACTION_SEQ__Z_25,
  ACTION_SEQ__LCTRL_27,
  ACTION_SEQ__LCTRL__LSHIFT_28,
  ACTION_SEQ__LCTRL__LSHIFT__LALT_29,
  ACTION_SEQ__X_31,
  ACTION_SEQ__LALT_33,
  ACTION_SEQ__LALT__LSHIFT_34,
  ACTION_SEQ__LALT__LSHIFT__LCTRL_35,
  ACTION_SEQ__C_37,
  ACTION_SEQ__LGUI_39,
  ACTION_SEQ__LGUI__LSHIFT_40,
  ACTION_SEQ__LGUI__LSHIFT__LCTRL_41,
  ACTION_SEQ__COMM_43,
  ACTION_SEQ__RGUI_45,
  ACTION_SEQ__RGUI__RSHIFT_46,
  ACTION_SEQ__RGUI__RSHIFT__RCTRL_47,
  ACTION_SEQ__DOT_49,
  ACTION_SEQ__RALT_51,
  ACTION_SEQ__RALT__RSHIFT_52,
  ACTION_SEQ__RALT__RSHIFT__RCTRL_53,
  ACTION_SEQ__SLSH_55,
  ACTION_SEQ__RCTRL_57,
  ACTION_SEQ__RCTRL__RSHIFT_58,
  ACTION_SEQ__RCTRL__RSHIFT__RALT_59,
  ACTION_SEQ__LSHIFT__0_61,
  ACTION_SEQ__RSHIFT_63,
  ACTION_SEQ__RSHIFT__RALT_64,
  ACTION_SEQ__RSHIFT__RALT__RGUI_65,
  ACTION_SEQ__1F60A_67,
  ACTION_SEQ__1F601_68,
  ACTION_SEQ__1F604_69,
  ACTION_SEQ__1F602_70,
  ACTION_SEQ__1F61E_73,
  ACTION_SEQ__1F622_74,
  ACTION_SEQ__1F62D_75,
  ACTION_SEQ__1F914_78,
  ACTION_SEQ__1F928_79,
  ACTION_SEQ__1F631_80,
  ACTION_SEQ__F7_83,
  ACTION_SEQ__F17_84,
  ACTION_SEQ__F8_87,
  ACTION_SEQ__F18_88,
  ACTION_SEQ__F9_91,
  ACTION_SEQ__F19_92,
  ACTION_SEQ__1F60F_95,
  ACTION_SEQ__1F60E_96,
  ACTION_SEQ__1F4AA_97,
  ACTION_SEQ__1F525_98,
  ACTION_SEQ__1F44D_101,
  ACTION_SEQ__1F44C_102,
  ACTION_SEQ__1F197_103,
  ACTION_SEQ__1F61A_106,
  ACTION_SEQ__1F60D_107,
  ACTION_SEQ__1F618_108,
  ACTION_SEQ__1F496_109,
  ACTION_SEQ__F4_112,
  ACTION_SEQ__F14_113,
  ACTION_SEQ__F5_116,
  ACTION_SEQ__F15_117,
  ACTION_SEQ__F6_120,
  ACTION_SEQ__F16_121,
  ACTION_SEQ__F1_124,
  ACTION_SEQ__F11_125,
  ACTION_SEQ__F2_128,
  ACTION_SEQ__F12_129,
  ACTION_SEQ__F3_132,
  ACTION_SEQ__F13_133,
  ACTION_SEQ__F10_136,
  ACTION_SEQ__F20_137
};

static int dance_key_states[85] = {0};

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
    
    case DANCE_KC_SEMICOLONDANCE:
      if (state->pressed) {
        // Hold actions:
        
        switch (state->count) {
          
          case 1:
            layer_on(LAYER_NAVIGATION);
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_NAVIGATION_10;
    
            return;
      
          case 2:
            layer_on(LAYER_NAVIGATION);
            
      layer_on(LAYER_NAVIGATIONEXTEND);
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_NAVIGATION__HOLD_LAYER_NAVIGATIONEXTEND_11;
    
            return;
      

          default:
            
          layer_on(LAYER_NAVIGATION);
            
      layer_on(LAYER_NAVIGATIONEXTEND);
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_NAVIGATION__HOLD_LAYER_NAVIGATIONEXTEND_11;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_SCOLON);
            dance_key_states[dance_key] = ACTION_SEQ__SCOLON_8;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_SCOLON);
            dance_key_states[dance_key] = ACTION_SEQ__SCOLON_8;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_EMOJI_15;
    
            return;
      
          case 2:
            layer_on(LAYER_EMOJI);
            code_down(KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_EMOJI__LCTRL_16;
    
            return;
      
          case 3:
            layer_on(LAYER_EMOJI);
            code_down(KC_LCTRL);
            code_down(KC_LALT);
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_EMOJI__LCTRL__LALT_17;
    
            return;
      

          default:
            
          layer_on(LAYER_EMOJI);
            code_down(KC_LCTRL);
            code_down(KC_LALT);
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_EMOJI__LCTRL__LALT_17;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_QUOT);
            dance_key_states[dance_key] = ACTION_SEQ__QUOT_13;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_QUOT);
            dance_key_states[dance_key] = ACTION_SEQ__QUOT_13;
    
            code_up(KC_QUOT);
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
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT_21;
    
            return;
      
          case 2:
            code_down(KC_LSHIFT);
            code_down(KC_LALT);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__LALT_22;
    
            return;
      
          case 3:
            code_down(KC_LSHIFT);
            code_down(KC_LALT);
            code_down(KC_LGUI);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__LALT__LGUI_23;
    
            return;
      

          default:
            
          code_down(KC_LSHIFT);
            code_down(KC_LALT);
            code_down(KC_LGUI);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__LALT__LGUI_23;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_LSHIFT);
            code_down(KC_9);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__9_19;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_LSHIFT);
            code_down(KC_9);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__9_19;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL_27;
    
            return;
      
          case 2:
            code_down(KC_LCTRL);
            code_down(KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL__LSHIFT_28;
    
            return;
      
          case 3:
            code_down(KC_LCTRL);
            code_down(KC_LSHIFT);
            code_down(KC_LALT);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL__LSHIFT__LALT_29;
    
            return;
      

          default:
            
          code_down(KC_LCTRL);
            code_down(KC_LSHIFT);
            code_down(KC_LALT);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL__LSHIFT__LALT_29;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_Z);
            dance_key_states[dance_key] = ACTION_SEQ__Z_25;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_Z);
            dance_key_states[dance_key] = ACTION_SEQ__Z_25;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__LALT_33;
    
            return;
      
          case 2:
            code_down(KC_LALT);
            code_down(KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LALT__LSHIFT_34;
    
            return;
      
          case 3:
            code_down(KC_LALT);
            code_down(KC_LSHIFT);
            code_down(KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__LALT__LSHIFT__LCTRL_35;
    
            return;
      

          default:
            
          code_down(KC_LALT);
            code_down(KC_LSHIFT);
            code_down(KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__LALT__LSHIFT__LCTRL_35;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_X);
            dance_key_states[dance_key] = ACTION_SEQ__X_31;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_X);
            dance_key_states[dance_key] = ACTION_SEQ__X_31;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__LGUI_39;
    
            return;
      
          case 2:
            code_down(KC_LGUI);
            code_down(KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LGUI__LSHIFT_40;
    
            return;
      
          case 3:
            code_down(KC_LGUI);
            code_down(KC_LSHIFT);
            code_down(KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__LGUI__LSHIFT__LCTRL_41;
    
            return;
      

          default:
            
          code_down(KC_LGUI);
            code_down(KC_LSHIFT);
            code_down(KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__LGUI__LSHIFT__LCTRL_41;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_C);
            dance_key_states[dance_key] = ACTION_SEQ__C_37;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_C);
            dance_key_states[dance_key] = ACTION_SEQ__C_37;
    
            code_up(KC_C);
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
            dance_key_states[dance_key] = ACTION_SEQ__RGUI_45;
    
            return;
      
          case 2:
            code_down(KC_RGUI);
            code_down(KC_RSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__RGUI__RSHIFT_46;
    
            return;
      
          case 3:
            code_down(KC_RGUI);
            code_down(KC_RSHIFT);
            code_down(KC_RCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__RGUI__RSHIFT__RCTRL_47;
    
            return;
      

          default:
            
          code_down(KC_RGUI);
            code_down(KC_RSHIFT);
            code_down(KC_RCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__RGUI__RSHIFT__RCTRL_47;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_COMM);
            dance_key_states[dance_key] = ACTION_SEQ__COMM_43;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_COMM);
            dance_key_states[dance_key] = ACTION_SEQ__COMM_43;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__RALT_51;
    
            return;
      
          case 2:
            code_down(KC_RALT);
            code_down(KC_RSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__RALT__RSHIFT_52;
    
            return;
      
          case 3:
            code_down(KC_RALT);
            code_down(KC_RSHIFT);
            code_down(KC_RCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__RALT__RSHIFT__RCTRL_53;
    
            return;
      

          default:
            
          code_down(KC_RALT);
            code_down(KC_RSHIFT);
            code_down(KC_RCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__RALT__RSHIFT__RCTRL_53;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_DOT);
            dance_key_states[dance_key] = ACTION_SEQ__DOT_49;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_DOT);
            dance_key_states[dance_key] = ACTION_SEQ__DOT_49;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__RCTRL_57;
    
            return;
      
          case 2:
            code_down(KC_RCTRL);
            code_down(KC_RSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__RCTRL__RSHIFT_58;
    
            return;
      
          case 3:
            code_down(KC_RCTRL);
            code_down(KC_RSHIFT);
            code_down(KC_RALT);
            dance_key_states[dance_key] = ACTION_SEQ__RCTRL__RSHIFT__RALT_59;
    
            return;
      

          default:
            
          code_down(KC_RCTRL);
            code_down(KC_RSHIFT);
            code_down(KC_RALT);
            dance_key_states[dance_key] = ACTION_SEQ__RCTRL__RSHIFT__RALT_59;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_SLSH);
            dance_key_states[dance_key] = ACTION_SEQ__SLSH_55;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_SLSH);
            dance_key_states[dance_key] = ACTION_SEQ__SLSH_55;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__RSHIFT_63;
    
            return;
      
          case 2:
            code_down(KC_RSHIFT);
            code_down(KC_RALT);
            dance_key_states[dance_key] = ACTION_SEQ__RSHIFT__RALT_64;
    
            return;
      
          case 3:
            code_down(KC_RSHIFT);
            code_down(KC_RALT);
            code_down(KC_RGUI);
            dance_key_states[dance_key] = ACTION_SEQ__RSHIFT__RALT__RGUI_65;
    
            return;
      

          default:
            
          code_down(KC_RSHIFT);
            code_down(KC_RALT);
            code_down(KC_RGUI);
            dance_key_states[dance_key] = ACTION_SEQ__RSHIFT__RALT__RGUI_65;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            code_down(KC_LSHIFT);
            code_down(KC_0);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__0_61;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_LSHIFT);
            code_down(KC_0);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__0_61;
    
            code_up(KC_LSHIFT);
            code_up(KC_0);
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
            dance_key_states[dance_key] = ACTION_SEQ__1F60A_67;
    
            return;
      
          case 2:
            tap_unicode(EMOJI_1F601);
            dance_key_states[dance_key] = ACTION_SEQ__1F601_68;
    
            return;
      
          case 3:
            tap_unicode(EMOJI_1F604);
            dance_key_states[dance_key] = ACTION_SEQ__1F604_69;
    
            return;
      
          case 4:
            tap_unicode(EMOJI_1F602);
            dance_key_states[dance_key] = ACTION_SEQ__1F602_70;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            tap_unicode(EMOJI_1F60A);
            dance_key_states[dance_key] = ACTION_SEQ__1F60A_67;
    
            
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
            dance_key_states[dance_key] = ACTION_SEQ__1F61E_73;
    
            return;
      
          case 2:
            tap_unicode(EMOJI_1F622);
            dance_key_states[dance_key] = ACTION_SEQ__1F622_74;
    
            return;
      
          case 3:
            tap_unicode(EMOJI_1F62D);
            dance_key_states[dance_key] = ACTION_SEQ__1F62D_75;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            tap_unicode(EMOJI_1F61E);
            dance_key_states[dance_key] = ACTION_SEQ__1F61E_73;
    
            
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
            dance_key_states[dance_key] = ACTION_SEQ__1F914_78;
    
            return;
      
          case 2:
            tap_unicode(EMOJI_1F928);
            dance_key_states[dance_key] = ACTION_SEQ__1F928_79;
    
            return;
      
          case 3:
            tap_unicode(EMOJI_1F631);
            dance_key_states[dance_key] = ACTION_SEQ__1F631_80;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            tap_unicode(EMOJI_1F914);
            dance_key_states[dance_key] = ACTION_SEQ__1F914_78;
    
            
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
            dance_key_states[dance_key] = ACTION_SEQ__F7_83;
    
            return;
      
          case 2:
            code_down(KC_F17);
            dance_key_states[dance_key] = ACTION_SEQ__F17_84;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_F7);
            dance_key_states[dance_key] = ACTION_SEQ__F7_83;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__F8_87;
    
            return;
      
          case 2:
            code_down(KC_F18);
            dance_key_states[dance_key] = ACTION_SEQ__F18_88;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_F8);
            dance_key_states[dance_key] = ACTION_SEQ__F8_87;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__F9_91;
    
            return;
      
          case 2:
            code_down(KC_F19);
            dance_key_states[dance_key] = ACTION_SEQ__F19_92;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_F9);
            dance_key_states[dance_key] = ACTION_SEQ__F9_91;
    
            code_up(KC_F9);
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
            dance_key_states[dance_key] = ACTION_SEQ__1F60F_95;
    
            return;
      
          case 2:
            tap_unicode(EMOJI_1F60E);
            dance_key_states[dance_key] = ACTION_SEQ__1F60E_96;
    
            return;
      
          case 3:
            tap_unicode(EMOJI_1F4AA);
            dance_key_states[dance_key] = ACTION_SEQ__1F4AA_97;
    
            return;
      
          case 4:
            tap_unicode(EMOJI_1F525);
            dance_key_states[dance_key] = ACTION_SEQ__1F525_98;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            tap_unicode(EMOJI_1F60F);
            dance_key_states[dance_key] = ACTION_SEQ__1F60F_95;
    
            
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
            dance_key_states[dance_key] = ACTION_SEQ__1F44D_101;
    
            return;
      
          case 2:
            tap_unicode(EMOJI_1F44C);
            dance_key_states[dance_key] = ACTION_SEQ__1F44C_102;
    
            return;
      
          case 3:
            tap_unicode(EMOJI_1F197);
            dance_key_states[dance_key] = ACTION_SEQ__1F197_103;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            tap_unicode(EMOJI_1F44D);
            dance_key_states[dance_key] = ACTION_SEQ__1F44D_101;
    
            
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
            dance_key_states[dance_key] = ACTION_SEQ__1F61A_106;
    
            return;
      
          case 2:
            tap_unicode(EMOJI_1F60D);
            dance_key_states[dance_key] = ACTION_SEQ__1F60D_107;
    
            return;
      
          case 3:
            tap_unicode(EMOJI_1F618);
            dance_key_states[dance_key] = ACTION_SEQ__1F618_108;
    
            return;
      
          case 4:
            tap_unicode(EMOJI_1F496);
            dance_key_states[dance_key] = ACTION_SEQ__1F496_109;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            tap_unicode(EMOJI_1F61A);
            dance_key_states[dance_key] = ACTION_SEQ__1F61A_106;
    
            
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
            dance_key_states[dance_key] = ACTION_SEQ__F4_112;
    
            return;
      
          case 2:
            code_down(KC_F14);
            dance_key_states[dance_key] = ACTION_SEQ__F14_113;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_F4);
            dance_key_states[dance_key] = ACTION_SEQ__F4_112;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__F5_116;
    
            return;
      
          case 2:
            code_down(KC_F15);
            dance_key_states[dance_key] = ACTION_SEQ__F15_117;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_F5);
            dance_key_states[dance_key] = ACTION_SEQ__F5_116;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__F6_120;
    
            return;
      
          case 2:
            code_down(KC_F16);
            dance_key_states[dance_key] = ACTION_SEQ__F16_121;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_F6);
            dance_key_states[dance_key] = ACTION_SEQ__F6_120;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__F1_124;
    
            return;
      
          case 2:
            code_down(KC_F11);
            dance_key_states[dance_key] = ACTION_SEQ__F11_125;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_F1);
            dance_key_states[dance_key] = ACTION_SEQ__F1_124;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__F2_128;
    
            return;
      
          case 2:
            code_down(KC_F12);
            dance_key_states[dance_key] = ACTION_SEQ__F12_129;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_F2);
            dance_key_states[dance_key] = ACTION_SEQ__F2_128;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__F3_132;
    
            return;
      
          case 2:
            code_down(KC_F13);
            dance_key_states[dance_key] = ACTION_SEQ__F13_133;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_F3);
            dance_key_states[dance_key] = ACTION_SEQ__F3_132;
    
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
            dance_key_states[dance_key] = ACTION_SEQ__F10_136;
    
            return;
      
          case 2:
            code_down(KC_F20);
            dance_key_states[dance_key] = ACTION_SEQ__F20_137;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            code_down(KC_F10);
            dance_key_states[dance_key] = ACTION_SEQ__F10_136;
    
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
    
    
        
          case ACTION_SEQ__HOLD_LAYER_NAVIGATION_10:
            layer_off(LAYER_NAVIGATION);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__HOLD_LAYER_NAVIGATION__HOLD_LAYER_NAVIGATIONEXTEND_11:
            layer_off(LAYER_NAVIGATION);
            layer_off(LAYER_NAVIGATIONEXTEND);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_SEQ__SCOLON_8:
            code_up(KC_SCOLON);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
          case ACTION_SEQ__HOLD_LAYER_EMOJI_15:
            layer_off(LAYER_EMOJI);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__HOLD_LAYER_EMOJI__LCTRL_16:
            layer_off(LAYER_EMOJI);
            code_up(KC_LCTRL);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__HOLD_LAYER_EMOJI__LCTRL__LALT_17:
            layer_off(LAYER_EMOJI);
            code_up(KC_LCTRL);
            code_up(KC_LALT);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_SEQ__QUOT_13:
            code_up(KC_QUOT);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
          case ACTION_SEQ__LSHIFT_21:
            code_up(KC_LSHIFT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__LSHIFT__LALT_22:
            code_up(KC_LSHIFT);
            code_up(KC_LALT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__LSHIFT__LALT__LGUI_23:
            code_up(KC_LSHIFT);
            code_up(KC_LALT);
            code_up(KC_LGUI);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_SEQ__LSHIFT__9_19:
            code_up(KC_LSHIFT);
            code_up(KC_9);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
          case ACTION_SEQ__LCTRL_27:
            code_up(KC_LCTRL);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__LCTRL__LSHIFT_28:
            code_up(KC_LCTRL);
            code_up(KC_LSHIFT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__LCTRL__LSHIFT__LALT_29:
            code_up(KC_LCTRL);
            code_up(KC_LSHIFT);
            code_up(KC_LALT);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_SEQ__Z_25:
            code_up(KC_Z);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
          case ACTION_SEQ__LALT_33:
            code_up(KC_LALT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__LALT__LSHIFT_34:
            code_up(KC_LALT);
            code_up(KC_LSHIFT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__LALT__LSHIFT__LCTRL_35:
            code_up(KC_LALT);
            code_up(KC_LSHIFT);
            code_up(KC_LCTRL);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_SEQ__X_31:
            code_up(KC_X);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
          case ACTION_SEQ__LGUI_39:
            code_up(KC_LGUI);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__LGUI__LSHIFT_40:
            code_up(KC_LGUI);
            code_up(KC_LSHIFT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__LGUI__LSHIFT__LCTRL_41:
            code_up(KC_LGUI);
            code_up(KC_LSHIFT);
            code_up(KC_LCTRL);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_SEQ__C_37:
            code_up(KC_C);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
          case ACTION_SEQ__RGUI_45:
            code_up(KC_RGUI);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__RGUI__RSHIFT_46:
            code_up(KC_RGUI);
            code_up(KC_RSHIFT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__RGUI__RSHIFT__RCTRL_47:
            code_up(KC_RGUI);
            code_up(KC_RSHIFT);
            code_up(KC_RCTRL);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_SEQ__COMM_43:
            code_up(KC_COMM);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
          case ACTION_SEQ__RALT_51:
            code_up(KC_RALT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__RALT__RSHIFT_52:
            code_up(KC_RALT);
            code_up(KC_RSHIFT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__RALT__RSHIFT__RCTRL_53:
            code_up(KC_RALT);
            code_up(KC_RSHIFT);
            code_up(KC_RCTRL);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_SEQ__DOT_49:
            code_up(KC_DOT);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
          case ACTION_SEQ__RCTRL_57:
            code_up(KC_RCTRL);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__RCTRL__RSHIFT_58:
            code_up(KC_RCTRL);
            code_up(KC_RSHIFT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__RCTRL__RSHIFT__RALT_59:
            code_up(KC_RCTRL);
            code_up(KC_RSHIFT);
            code_up(KC_RALT);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_SEQ__SLSH_55:
            code_up(KC_SLSH);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
          case ACTION_SEQ__RSHIFT_63:
            code_up(KC_RSHIFT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__RSHIFT__RALT_64:
            code_up(KC_RSHIFT);
            code_up(KC_RALT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__RSHIFT__RALT__RGUI_65:
            code_up(KC_RSHIFT);
            code_up(KC_RALT);
            code_up(KC_RGUI);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_SEQ__LSHIFT__0_61:
            code_up(KC_LSHIFT);
            code_up(KC_0);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__1F60A_67:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F601_68:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F604_69:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F602_70:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__1F61E_73:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F622_74:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F62D_75:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__1F914_78:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F928_79:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F631_80:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__F7_83:
            code_up(KC_F7);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__F17_84:
            code_up(KC_F17);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__F8_87:
            code_up(KC_F8);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__F18_88:
            code_up(KC_F18);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__F9_91:
            code_up(KC_F9);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__F19_92:
            code_up(KC_F19);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__1F60F_95:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F60E_96:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F4AA_97:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F525_98:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__1F44D_101:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F44C_102:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F197_103:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__1F61A_106:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F60D_107:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F618_108:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__1F496_109:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__F4_112:
            code_up(KC_F4);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__F14_113:
            code_up(KC_F14);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__F5_116:
            code_up(KC_F5);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__F15_117:
            code_up(KC_F15);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__F6_120:
            code_up(KC_F6);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__F16_121:
            code_up(KC_F16);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__F1_124:
            code_up(KC_F1);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__F11_125:
            code_up(KC_F11);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__F2_128:
            code_up(KC_F2);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__F12_129:
            code_up(KC_F12);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__F3_132:
            code_up(KC_F3);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__F13_133:
            code_up(KC_F13);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_SEQ__F10_136:
            code_up(KC_F10);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_SEQ__F20_137:
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
  [DANCE_KC_SEMICOLONDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_QUOTEDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_LSHIFTDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_ZDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_XDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_CDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_COMMADANCE] = DANCE_MODIFIER(),
  [DANCE_KC_DOTDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_SLASHDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_RSHIFTDANCE] = DANCE_MODIFIER(),
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

  // At the end:
  DYNAMIC_MACRO_RANGE,
};

#include "dynamic_macro.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[LAYER_DEFAULT] = LAYOUT_planck_grid(
  
/* keys-0 */ KC_ESC,KC_Q,KC_W,KC_E,KC_R,KC_T,KC_Y,KC_U,KC_I,KC_O,KC_P,KC_BSLS,
/* keys-1 */ TD(DANCE_KC_CAPSDANCE),KC_A,KC_S,KC_D,KC_F,KC_G,KC_H,KC_J,KC_K,KC_L,TD(DANCE_KC_SEMICOLONDANCE),TD(DANCE_KC_QUOTEDANCE),
/* keys-2 */ TD(DANCE_KC_LSHIFTDANCE),TD(DANCE_KC_ZDANCE),TD(DANCE_KC_XDANCE),TD(DANCE_KC_CDANCE),KC_V,KC_B,KC_N,KC_M,TD(DANCE_KC_COMMADANCE),TD(DANCE_KC_DOTDANCE),TD(DANCE_KC_SLASHDANCE),TD(DANCE_KC_RSHIFTDANCE),
/* keys-3 */ _______,_______,_______,KC_DELETE,KC_BSPC,KC_SPC,_______,KC_TAB,KC_ENT,KC_LBRC,KC_RBRC,TG(LAYER_PLOVER)
)
  ,

[LAYER_SYMBOL] = LAYOUT_planck_grid(
  
/* keys-0 */ _______,KC_EXLM,KC_AT,KC_LCBR,KC_RCBR,KC_SLSH,KC_EQL,KC_7,KC_8,KC_9,KC_AMPR,KC_PIPE,
/* keys-1 */ _______,KC_HASH,KC_DLR,KC_LPRN,KC_RPRN,KC_GRV,KC_PPLS,KC_4,KC_5,KC_6,KC_PAST,_______,
/* keys-2 */ _______,KC_PERC,KC_CIRC,KC_LBRC,KC_RBRC,KC_TILD,KC_MINS,KC_1,KC_2,KC_3,KC_SLSH,_______,
/* keys-3 */ _______,_______,_______,_______,_______,_______,_______,_______,KC_0,KC_DOT,_______,_______
)
  ,

[LAYER_NAVIGATION] = LAYOUT_planck_grid(
  
/* keys-0 */ _______,_______,KC_BTN2,KC_MS_U,KC_BTN1,_______,_______,KC_BRID,KC_BRIU,_______,_______,_______,
/* keys-1 */ _______,_______,KC_MS_L,KC_MS_D,KC_MS_R,_______,KC_LEFT,KC_DOWN,KC_UP,KC_RGHT,_______,_______,
/* keys-2 */ _______,_______,_______,_______,_______,_______,KC_WH_L,KC_WH_D,KC_WH_U,KC_WH_R,_______,_______,
/* keys-3 */ _______,_______,_______,_______,_______,_______,_______,KC_VOLD,KC_VOLU,KC_MUTE,_______,_______
)
  ,

[LAYER_NAVIGATIONEXTEND] = LAYOUT_planck_grid(
  
/* keys-0 */ _______,_______,_______,_______,_______,_______,KC_PWR,KC_SLEP,UC_M_WC,UC_M_OS,_______,_______,
/* keys-1 */ _______,AU_TOG,CK_TOGG,CK_UP,CK_DOWN,_______,KC_HOME,KC_PGUP,KC_PGDN,KC_END,_______,_______,
/* keys-2 */ _______,MU_TOG,MU_MOD,_______,_______,_______,_______,RGB_VAD,RGB_VAI,RGB_SAD,RGB_SAI,_______,
/* keys-3 */ _______,_______,_______,_______,_______,_______,_______,RGB_HUD,RGB_HUI,RGB_TOG,RGB_MOD,_______
)
  ,

[LAYER_EMOJI] = LAYOUT_planck_grid(
  
/* keys-0 */ _______,TD(DANCE_KC_HAPPYDANCE),TD(DANCE_KC_SADDANCE),TD(DANCE_KC_STRANGEDANCE),_______,_______,_______,TD(DANCE_KC_F7DANCE),TD(DANCE_KC_F8DANCE),TD(DANCE_KC_F9DANCE),_______,_______,
/* keys-1 */ _______,TD(DANCE_KC_COOLDANCE),TD(DANCE_KC_OKDANCE),TD(DANCE_KC_LOVEDANCE),_______,_______,_______,TD(DANCE_KC_F4DANCE),TD(DANCE_KC_F5DANCE),TD(DANCE_KC_F6DANCE),_______,_______,
/* keys-2 */ _______,_______,_______,_______,_______,_______,_______,TD(DANCE_KC_F1DANCE),TD(DANCE_KC_F2DANCE),TD(DANCE_KC_F3DANCE),_______,_______,
/* keys-3 */ ZKC_BTL,_______,_______,_______,_______,_______,_______,_______,TD(DANCE_KC_F10DANCE),_______,_______,_______
)
  ,

[LAYER_PLOVER] = LAYOUT_planck_grid(
  
/* keys-0 */ _______,STN_N1,STN_N2,STN_N3,STN_N4,STN_N5,STN_N6,STN_N7,STN_N8,STN_N9,STN_NA,STN_NB,
/* keys-1 */ _______,STN_S1,STN_TL,STN_PL,STN_HL,STN_ST1,STN_ST3,STN_FR,STN_PR,STN_LR,STN_TR,STN_DR,
/* keys-2 */ _______,STN_S2,STN_KL,STN_WL,STN_RL,STN_ST2,STN_ST4,STN_RR,STN_BR,STN_GR,STN_SR,STN_ZR,
/* keys-3 */ _______,_______,_______,STN_A,STN_O,_______,_______,STN_E,STN_U,_______,_______,_______
)
  
};

void matrix_init_user(void) {
  steno_set_mode(STENO_MODE_GEMINI);
  switch (get_unicode_input_mode()) {
    case UC_OSX:
      zored_os = OS_MACOS;
    default:
      zored_os = OS_WINDOWS;
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

  palClearPad(GPIOB, 8);
  palClearPad(GPIOB, 9);
  uint8_t layer = biton32(state);
  switch (layer) {
    
      case LAYER_SYMBOL:
        palSetPad(GPIOB, 9);
        break;
    
      case LAYER_NAVIGATION:
        palSetPad(GPIOB, 8);
        break;
    
      case LAYER_NAVIGATIONEXTEND:
        palSetPad(GPIOB, 8);
        break;
    
      case LAYER_EMOJI:
        palSetPad(GPIOB, 9); palSetPad(GPIOB, 8);
        break;
    
      case LAYER_PLOVER:
        palSetPad(GPIOB, 8);
        break;
    
  }

  return state;
};
