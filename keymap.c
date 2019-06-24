#include "ergodox_ez.h"
#include "debug.h"
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
  EMOJI_1F60A = 0,
  EMOJI_1F602,
  EMOJI_1F914,
  EMOJI_1F44D,
  EMOJI_1F4AA,
  EMOJI_1F60F,
  EMOJI_1F525,
  EMOJI_1F496,
  EMOJI_1F622,
  EMOJI_1F62D
};

const uint32_t PROGMEM
unicode_map[] = {
  [EMOJI_1F60A] 0x1F60A, // 😊
[EMOJI_1F602] 0x1F602, // 😂
[EMOJI_1F914] 0x1F914, // 🤔
[EMOJI_1F44D] 0x1F44D, // 👍
[EMOJI_1F4AA] 0x1F4AA, // 💪
[EMOJI_1F60F] 0x1F60F, // 😏
[EMOJI_1F525] 0x1F525, // 🔥
[EMOJI_1F496] 0x1F496, // 💖
[EMOJI_1F622] 0x1F622, // 😢
[EMOJI_1F62D] 0x1F62D, // 😭
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
LAYER_DEFAULT = 0,
  LAYER_SYMBOL,
  LAYER_NAVIGATION,
  LAYER_EMOJI,
  LAYER_PLOVER
};

enum dance_keys {
  DANCE_KC_CAPSDANCE = 1,
  DANCE_KC_ZDANCE,
  DANCE_KC_SEMICOLONDANCE,
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
  ACTION_NULL_3 = 1,
  ACTION_UNDEFINED_1,
  ACTION_UNDEFINED_2,
  ACTION_NULL_7,
  ACTION_UNDEFINED_4,
  ACTION_UNDEFINED_5,
  ACTION_UNDEFINED_6,
  ACTION_NULL_9,
  ACTION_UNDEFINED_8,
  ACTION_NULL_13,
  ACTION_UNDEFINED_10,
  ACTION_UNDEFINED_11,
  ACTION_UNDEFINED_12,
  ACTION_NULL_15,
  ACTION_UNDEFINED_14,
  ACTION_NULL_18,
  ACTION_UNDEFINED_16,
  ACTION_UNDEFINED_17,
  ACTION_NULL_21,
  ACTION_UNDEFINED_19,
  ACTION_UNDEFINED_20,
  ACTION_NULL_22,
  ACTION_NULL_25,
  ACTION_UNDEFINED_23,
  ACTION_UNDEFINED_24,
  ACTION_NULL_26,
  ACTION_NULL_29,
  ACTION_UNDEFINED_27,
  ACTION_UNDEFINED_28,
  ACTION_NULL_30,
  ACTION_NULL_33,
  ACTION_UNDEFINED_31,
  ACTION_UNDEFINED_32,
  ACTION_NULL_34,
  ACTION_NULL_37,
  ACTION_UNDEFINED_35,
  ACTION_UNDEFINED_36,
  ACTION_NULL_38,
  ACTION_NULL_41,
  ACTION_UNDEFINED_39,
  ACTION_UNDEFINED_40,
  ACTION_NULL_42,
  ACTION_NULL_45,
  ACTION_UNDEFINED_43,
  ACTION_UNDEFINED_44,
  ACTION_NULL_46,
  ACTION_NULL_49,
  ACTION_UNDEFINED_47,
  ACTION_UNDEFINED_48,
  ACTION_NULL_50,
  ACTION_NULL_53,
  ACTION_UNDEFINED_51,
  ACTION_UNDEFINED_52,
  ACTION_NULL_54,
  ACTION_NULL_57,
  ACTION_UNDEFINED_55,
  ACTION_UNDEFINED_56,
  ACTION_NULL_58
};

static int dance_key_states[58] = {0};

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
            dance_key_states[dance_key] = ACTION_UNDEFINED_4;
    
            return;
      
          case 2:
            layer_on(LAYER_SYMBOL);
            register_win_code(KC_LCTRL);
            dance_key_states[dance_key] = ACTION_UNDEFINED_5;
    
            return;
      
          case 3:
            layer_on(LAYER_SYMBOL);
            register_win_code(KC_LALT);
            dance_key_states[dance_key] = ACTION_UNDEFINED_6;
    
            return;
      

          default:
            
          layer_on(LAYER_SYMBOL);
            register_win_code(KC_LALT);
            dance_key_states[dance_key] = ACTION_UNDEFINED_6;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            register_win_code(KC_CAPSLOCK);
            dance_key_states[dance_key] = ACTION_UNDEFINED_1;
    
            return;
      
          case 2:
            layer_invert(LAYER_SYMBOL);
            dance_key_states[dance_key] = ACTION_UNDEFINED_2;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            register_win_code(KC_CAPSLOCK);
            dance_key_states[dance_key] = ACTION_UNDEFINED_1;
    
            unregister_win_code(KC_CAPSLOCK);
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
            register_win_code(KC_LCTRL);
            dance_key_states[dance_key] = ACTION_UNDEFINED_10;
    
            return;
      
          case 2:
            register_win_code(KC_LCTRL);
            register_win_code(KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_UNDEFINED_11;
    
            return;
      
          case 3:
            register_win_code(KC_LCTRL);
            register_win_code(KC_LSHIFT);
            register_win_code(KC_LALT);
            dance_key_states[dance_key] = ACTION_UNDEFINED_12;
    
            return;
      

          default:
            
          register_win_code(KC_LCTRL);
            register_win_code(KC_LSHIFT);
            register_win_code(KC_LALT);
            dance_key_states[dance_key] = ACTION_UNDEFINED_12;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            register_win_code(KC_Z);
            dance_key_states[dance_key] = ACTION_UNDEFINED_8;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            register_win_code(KC_Z);
            dance_key_states[dance_key] = ACTION_UNDEFINED_8;
    
            unregister_win_code(KC_Z);
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
            dance_key_states[dance_key] = ACTION_UNDEFINED_16;
    
            return;
      
          case 2:
            layer_on(LAYER_SYMBOL);
            dance_key_states[dance_key] = ACTION_UNDEFINED_17;
    
            return;
      

          default:
            
          layer_on(LAYER_SYMBOL);
            dance_key_states[dance_key] = ACTION_UNDEFINED_17;
    
        
            return;
        }
    
      }
      else {
        // Tap actions:
        
        switch (state->count) {
          
          case 1:
            register_win_code(KC_SCOLON);
            dance_key_states[dance_key] = ACTION_UNDEFINED_14;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            register_win_code(KC_SCOLON);
            dance_key_states[dance_key] = ACTION_UNDEFINED_14;
    
            unregister_win_code(KC_SCOLON);
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
            register_win_code(KC_F7);
            dance_key_states[dance_key] = ACTION_UNDEFINED_19;
    
            return;
      
          case 2:
            register_win_code(KC_F17);
            dance_key_states[dance_key] = ACTION_UNDEFINED_20;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            register_win_code(KC_F7);
            dance_key_states[dance_key] = ACTION_UNDEFINED_19;
    
            unregister_win_code(KC_F7);
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
            register_win_code(KC_F8);
            dance_key_states[dance_key] = ACTION_UNDEFINED_23;
    
            return;
      
          case 2:
            register_win_code(KC_F18);
            dance_key_states[dance_key] = ACTION_UNDEFINED_24;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            register_win_code(KC_F8);
            dance_key_states[dance_key] = ACTION_UNDEFINED_23;
    
            unregister_win_code(KC_F8);
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
            register_win_code(KC_F9);
            dance_key_states[dance_key] = ACTION_UNDEFINED_27;
    
            return;
      
          case 2:
            register_win_code(KC_F19);
            dance_key_states[dance_key] = ACTION_UNDEFINED_28;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            register_win_code(KC_F9);
            dance_key_states[dance_key] = ACTION_UNDEFINED_27;
    
            unregister_win_code(KC_F9);
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
            register_win_code(KC_F4);
            dance_key_states[dance_key] = ACTION_UNDEFINED_31;
    
            return;
      
          case 2:
            register_win_code(KC_F14);
            dance_key_states[dance_key] = ACTION_UNDEFINED_32;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            register_win_code(KC_F4);
            dance_key_states[dance_key] = ACTION_UNDEFINED_31;
    
            unregister_win_code(KC_F4);
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
            register_win_code(KC_F5);
            dance_key_states[dance_key] = ACTION_UNDEFINED_35;
    
            return;
      
          case 2:
            register_win_code(KC_F15);
            dance_key_states[dance_key] = ACTION_UNDEFINED_36;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            register_win_code(KC_F5);
            dance_key_states[dance_key] = ACTION_UNDEFINED_35;
    
            unregister_win_code(KC_F5);
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
            register_win_code(KC_F6);
            dance_key_states[dance_key] = ACTION_UNDEFINED_39;
    
            return;
      
          case 2:
            register_win_code(KC_F16);
            dance_key_states[dance_key] = ACTION_UNDEFINED_40;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            register_win_code(KC_F6);
            dance_key_states[dance_key] = ACTION_UNDEFINED_39;
    
            unregister_win_code(KC_F6);
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
            register_win_code(KC_F1);
            dance_key_states[dance_key] = ACTION_UNDEFINED_43;
    
            return;
      
          case 2:
            register_win_code(KC_F11);
            dance_key_states[dance_key] = ACTION_UNDEFINED_44;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            register_win_code(KC_F1);
            dance_key_states[dance_key] = ACTION_UNDEFINED_43;
    
            unregister_win_code(KC_F1);
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
            register_win_code(KC_F2);
            dance_key_states[dance_key] = ACTION_UNDEFINED_47;
    
            return;
      
          case 2:
            register_win_code(KC_F12);
            dance_key_states[dance_key] = ACTION_UNDEFINED_48;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            register_win_code(KC_F2);
            dance_key_states[dance_key] = ACTION_UNDEFINED_47;
    
            unregister_win_code(KC_F2);
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
            register_win_code(KC_F3);
            dance_key_states[dance_key] = ACTION_UNDEFINED_51;
    
            return;
      
          case 2:
            register_win_code(KC_F13);
            dance_key_states[dance_key] = ACTION_UNDEFINED_52;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            register_win_code(KC_F3);
            dance_key_states[dance_key] = ACTION_UNDEFINED_51;
    
            unregister_win_code(KC_F3);
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
            register_win_code(KC_F10);
            dance_key_states[dance_key] = ACTION_UNDEFINED_55;
    
            return;
      
          case 2:
            register_win_code(KC_F20);
            dance_key_states[dance_key] = ACTION_UNDEFINED_56;
    
            return;
      

          default:
            
          for (int i=0; i < state->count; i++) {
            register_win_code(KC_F10);
            dance_key_states[dance_key] = ACTION_UNDEFINED_55;
    
            unregister_win_code(KC_F10);
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
    case ACTION_UNDEFINED_4:
            layer_off(LAYER_SYMBOL);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_UNDEFINED_5:
            layer_off(LAYER_SYMBOL);
            unregister_win_code(KC_LCTRL);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_UNDEFINED_6:
            layer_off(LAYER_SYMBOL);
            unregister_win_code(KC_LALT);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_UNDEFINED_1:
            unregister_win_code(KC_CAPSLOCK);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_UNDEFINED_2:
            
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
          case ACTION_UNDEFINED_10:
            unregister_win_code(KC_LCTRL);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_UNDEFINED_11:
            unregister_win_code(KC_LCTRL);
            unregister_win_code(KC_LSHIFT);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_UNDEFINED_12:
            unregister_win_code(KC_LCTRL);
            unregister_win_code(KC_LSHIFT);
            unregister_win_code(KC_LALT);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_UNDEFINED_8:
            unregister_win_code(KC_Z);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
          case ACTION_UNDEFINED_16:
            layer_off(LAYER_NAVIGATION);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_UNDEFINED_17:
            layer_off(LAYER_SYMBOL);
            dance_key_states[dance_key] = 0;
    
            break;
    
        
          case ACTION_UNDEFINED_14:
            unregister_win_code(KC_SCOLON);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_UNDEFINED_19:
            unregister_win_code(KC_F7);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_UNDEFINED_20:
            unregister_win_code(KC_F17);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_UNDEFINED_23:
            unregister_win_code(KC_F8);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_UNDEFINED_24:
            unregister_win_code(KC_F18);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_UNDEFINED_27:
            unregister_win_code(KC_F9);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_UNDEFINED_28:
            unregister_win_code(KC_F19);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_UNDEFINED_31:
            unregister_win_code(KC_F4);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_UNDEFINED_32:
            unregister_win_code(KC_F14);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_UNDEFINED_35:
            unregister_win_code(KC_F5);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_UNDEFINED_36:
            unregister_win_code(KC_F15);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_UNDEFINED_39:
            unregister_win_code(KC_F6);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_UNDEFINED_40:
            unregister_win_code(KC_F16);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_UNDEFINED_43:
            unregister_win_code(KC_F1);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_UNDEFINED_44:
            unregister_win_code(KC_F11);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_UNDEFINED_47:
            unregister_win_code(KC_F2);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_UNDEFINED_48:
            unregister_win_code(KC_F12);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_UNDEFINED_51:
            unregister_win_code(KC_F3);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_UNDEFINED_52:
            unregister_win_code(KC_F13);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
        
        
          case ACTION_UNDEFINED_55:
            unregister_win_code(KC_F10);
            dance_key_states[dance_key] = 0;
    
            break;
    
          case ACTION_UNDEFINED_56:
            unregister_win_code(KC_F20);
            dance_key_states[dance_key] = 0;
    
            break;
    
    
  }
}

#define DANCE_MODIFIER() ACTION_TAP_DANCE_FN_ADVANCED_TIME(NULL, on_dance, on_dance_reset, TAPPING_TERM_TAP_DANCE)
#define DANCE_TWO(k1,k11) ACTION_TAP_DANCE_DOUBLE_TIME(k1, k11, TAPPING_TERM_TAP_DANCE)

qk_tap_dance_action_t tap_dance_actions[] = {
    [DANCE_KC_CAPSDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_ZDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_SEMICOLONDANCE] = DANCE_MODIFIER(),
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
/* keys-left-1 */ KC_TAB,KC_Q,KC_W,KC_E,KC_R,KC_T,_______,
/* keys-left-2 */ TD(DANCE_KC_CAPSDANCE),KC_A,KC_S,KC_D,KC_F,KC_G,
/* keys-left-3 */ KC_LSPO,TD(DANCE_KC_ZDANCE),KC_X,KC_C,KC_V,KC_B,_______,
/* keys-left-4 */ _______,_______,_______,KC_LEFT,KC_RGHT,
/* keys-left-thumb-0 */ KC_ESC,_______,
/* keys-left-thumb-1 */ KC_HOME,
/* keys-left-thumb-2 */ KC_SPC,KC_BSPC,KC_END,
/* keys-right-0 */ _______,KC_6,KC_7,KC_8,KC_9,KC_0,_______,
/* keys-right-1 */ _______,KC_Y,KC_U,KC_I,KC_O,KC_P,KC_BSLS,
/* keys-right-2 */ KC_H,KC_J,KC_K,KC_L,TD(DANCE_KC_SEMICOLONDANCE),KC_QUOT,
/* keys-right-3 */ _______,KC_N,KC_M,KC_COMM,KC_DOT,KC_SLSH,KC_RSPC,
/* keys-right-4 */ KC_LBRC,KC_RBRC,KC_BSLS,KC_GRV,TG(LAYER_PLOVER),
/* keys-right-thumb-0 */ KC_ESC,_______,
/* keys-right-thumb-1 */ KC_PGUP,
/* keys-right-thumb-2 */ KC_PGDN,KC_TAB,KC_ENT
)
  ,

[LAYER_SYMBOL] = LAYOUT_ergodox(
  
/* keys-left-0 */ _______,_______,_______,_______,_______,_______,_______,
/* keys-left-1 */ _______,KC_EXLM,KC_AT,_______,KC_BSLS,KC_PIPE,_______,
/* keys-left-2 */ KC_HASH,KC_DLR,KC_LPRN,KC_RPRN,KC_GRV,_______,
/* keys-left-3 */ KC_PERC,KC_CIRC,KC_LBRC,KC_RBRC,KC_TILD,_______,_______,
/* keys-left-4 */ _______,_______,_______,_______,_______,
/* keys-left-thumb-0 */ _______,_______,
/* keys-left-thumb-1 */ _______,
/* keys-left-thumb-2 */ KC_DELETE,_______,_______,
/* keys-right-0 */ _______,_______,_______,_______,_______,_______,_______,
/* keys-right-1 */ KC_PPLS,KC_7,KC_8,KC_9,_______,_______,KC_EQL,
/* keys-right-2 */ KC_4,KC_5,KC_6,KC_PAST,_______,_______,
/* keys-right-3 */ KC_MINS,KC_1,KC_2,KC_3,KC_AMPR,_______,_______,
/* keys-right-4 */ _______,KC_EQL,KC_0,KC_DOT,_______,
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
/* keys-right-0 */ _______,KC_PWR,KC_SLEP,UC_M_WC,UC_M_OS,_______,_______,
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
/* keys-left-1 */ _______,X(EMOJI_1F60A) /*😊*/,X(EMOJI_1F602) /*😂*/,X(EMOJI_1F914) /*🤔*/,X(EMOJI_1F44D) /*👍*/,X(EMOJI_1F4AA) /*💪*/,_______,
/* keys-left-2 */ _______,X(EMOJI_1F60F) /*😏*/,X(EMOJI_1F525) /*🔥*/,X(EMOJI_1F496) /*💖*/,X(EMOJI_1F622) /*😢*/,X(EMOJI_1F62D) /*😭*/,
/* keys-left-3 */ _______,_______,_______,_______,_______,_______,_______,
/* keys-left-4 */ _______,_______,_______,_______,_______,
/* keys-left-thumb-0 */ _______,_______,
/* keys-left-thumb-1 */ _______,
/* keys-left-thumb-2 */ _______,_______,_______,
/* keys-right-0 */ ZKC_BTL,DYN_REC_START1,DYN_REC_START2,DYN_REC_STOP,DYN_MACRO_PLAY1,DYN_MACRO_PLAY2,_______,
/* keys-right-1 */ _______,_______,TD(DANCE_KC_F7DANCE),TD(DANCE_KC_F8DANCE),TD(DANCE_KC_F9DANCE),_______,_______,
/* keys-right-2 */ _______,TD(DANCE_KC_F4DANCE),TD(DANCE_KC_F5DANCE),TD(DANCE_KC_F6DANCE),_______,_______,
/* keys-right-3 */ _______,_______,TD(DANCE_KC_F1DANCE),TD(DANCE_KC_F2DANCE),TD(DANCE_KC_F3DANCE),_______,_______,
/* keys-right-4 */ _______,_______,TD(DANCE_KC_F10DANCE),_______,_______,
/* keys-right-thumb-0 */ _______,_______,
/* keys-right-thumb-1 */ _______,
/* keys-right-thumb-2 */ _______,_______,_______
)
  ,

[LAYER_PLOVER] = LAYOUT_ergodox(
  
/* keys-left-0 */ _______,_______,_______,_______,_______,_______,_______,
/* keys-left-1 */ _______,_______,_______,_______,_______,_______,_______,
/* keys-left-2 */ _______,_______,_______,_______,_______,_______,
/* keys-left-3 */ _______,_______,_______,_______,_______,_______,_______,
/* keys-left-4 */ _______,_______,_______,_______,_______,
/* keys-left-thumb-0 */ _______,_______,
/* keys-left-thumb-1 */ _______,
/* keys-left-thumb-2 */ _______,_______,_______,
/* keys-right-0 */ _______,_______,_______,_______,_______,_______,_______,
/* keys-right-1 */ _______,_______,_______,_______,_______,_______,_______,
/* keys-right-2 */ _______,_______,_______,_______,_______,_______,
/* keys-right-3 */ _______,_______,_______,_______,_______,_______,_______,
/* keys-right-4 */ _______,_______,_______,_______,_______,
/* keys-right-thumb-0 */ _______,_______,
/* keys-right-thumb-1 */ _______,
/* keys-right-thumb-2 */ _______,_______,_______
)
  
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
    
    case LAYER_SYMBOL:
      ergodox_right_led_on(2);
      break;
  
    case LAYER_NAVIGATION:
      ergodox_right_led_on(1);
      break;
  
    case LAYER_EMOJI:
      ergodox_right_led_on(1); ergodox_right_led_on(3);
      break;
  
    case LAYER_PLOVER:
      ergodox_right_led_on(3);
      break;
  
  }

  return state;
};

