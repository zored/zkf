


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

uint8_t mappingIndex = 0;
uint8_t mappingIndexMax = 1;
uint8_t map_code8_hash (uint8_t keycode) {
  
      switch(mappingIndex) {
        case 0:
          break;
        
      case 1:
        // Mapping "ru":
        switch(keycode) {
          case KC_A: return KC_F;
case KC_B: return KC_COMM;
case KC_C: return KC_D;
case KC_D: return KC_U;
case KC_E: return KC_L;
case KC_F: return KC_T;
case KC_G: return KC_GRV;
case KC_H: return KC_SCOLON;
case KC_I: return KC_P;
case KC_J: return KC_B;
case KC_K: return KC_Q;
case KC_L: return KC_R;
case KC_M: return KC_K;
case KC_N: return KC_V;
case KC_O: return KC_Y;
case KC_P: return KC_J;
case KC_Q: return KC_G;
case KC_R: return KC_H;
case KC_S: return KC_C;
case KC_T: return KC_N;
case KC_U: return KC_E;
case KC_V: return KC_A;
case KC_W: return KC_LBRC;
case KC_X: return KC_W;
case KC_Y: return KC_X;
case KC_Z: return KC_I;
case KC_COMM: return KC_O;
case KC_SCOLON: return KC_RBRC;
case KC_LBRC: return KC_S;
case KC_RBRC: return KC_M;
case KC_QUOT: return KC_QUOT;
case KC_DOT: return KC_DOT;
case KC_GRV: return KC_Z;
        }
        break;
    
      }
  
  return keycode;
}
uint8_t map_code16_hash (uint16_t keycode) {
  
      switch(mappingIndex) {
        case 0:
          break;
        
      case 1:
        // Mapping "ru":
        switch(keycode) {
          case KC_A: return KC_F;
case KC_B: return KC_COMM;
case KC_C: return KC_D;
case KC_D: return KC_U;
case KC_E: return KC_L;
case KC_F: return KC_T;
case KC_G: return KC_GRV;
case KC_H: return KC_SCOLON;
case KC_I: return KC_P;
case KC_J: return KC_B;
case KC_K: return KC_Q;
case KC_L: return KC_R;
case KC_M: return KC_K;
case KC_N: return KC_V;
case KC_O: return KC_Y;
case KC_P: return KC_J;
case KC_Q: return KC_G;
case KC_R: return KC_H;
case KC_S: return KC_C;
case KC_T: return KC_N;
case KC_U: return KC_E;
case KC_V: return KC_A;
case KC_W: return KC_LBRC;
case KC_X: return KC_W;
case KC_Y: return KC_X;
case KC_Z: return KC_I;
case KC_COMM: return KC_O;
case KC_SCOLON: return KC_RBRC;
case KC_LBRC: return KC_S;
case KC_RBRC: return KC_M;
case KC_QUOT: return KC_QUOT;
case KC_DOT: return KC_DOT;
case KC_GRV: return KC_Z;
        }
        break;
    
      }
  
  return 0;
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

  keycode = map_code8_hash(keycode);

  return keycode;
}

void code_down(uint8_t code) {
  register_code(map_code(code));
}

void code_up(uint8_t code) {
  unregister_code(map_code(code));
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
  DO_NEXT_MAPPING,
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
    case DO_NEXT_MAPPING:
      run_advanced(DO_NEXT_LANGUAGE);
      mappingIndex++;
      if (mappingIndex > mappingIndexMax) {
        mappingIndex = 0;
      }
      break;
    case DO_NEXT_LANGUAGE:
      ; // empty statement.
      uint8_t hold = 0;
      uint8_t tap = 0;
      uint32_t timeout = 100;

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
LAYER_DEFAULT = 0,
  LAYER_LAYERA,
  LAYER_LAYERB,
  LAYER_LAYERC,
  LAYER_LAYERD,
  LAYER_LAYERE,
  LAYER_LAYERF,
  LAYER_NAVIGATION,
  LAYER_COLOR
};

enum dance_keys {
  DANCE_KC_YADANCE = 1,
  DANCE_KC_YBDANCE,
  DANCE_KC_YCDANCE,
  DANCE_KC_YDDANCE,
  DANCE_KC_YEDANCE,
  DANCE_KC_YFDANCE,
  DANCE_KC_YCTRLDANCE,
  DANCE_KC_YALTDANCE,
  DANCE_KC_YGUIDANCE,
  DANCE_KC_YHDANCE,
  DANCE_KC_YMAPPINGDANCE
};
enum dance_action_names {
  ACTION_SEQ__A_1 = 1,
  ACTION_SEQ__HOLD_LAYER_LAYERA_3,
  ACTION_SEQ__B_5,
  ACTION_SEQ__HOLD_LAYER_LAYERB_7,
  ACTION_SEQ__C_9,
  ACTION_SEQ__HOLD_LAYER_LAYERC_11,
  ACTION_SEQ__D_13,
  ACTION_SEQ__HOLD_LAYER_LAYERD_15,
  ACTION_SEQ__E_17,
  ACTION_SEQ__HOLD_LAYER_LAYERE_19,
  ACTION_SEQ__F_21,
  ACTION_SEQ__HOLD_LAYER_LAYERF_23,
  ACTION_SEQ__BSPC_25,
  ACTION_SEQ__LCTRL_27,
  ACTION_SEQ__LSHIFT_28,
  ACTION_SEQ__SPC_30,
  ACTION_SEQ__LALT_32,
  ACTION_SEQ__ENT_34,
  ACTION_SEQ__LGUI_36,
  ACTION_SEQ__RSHIFT_37,
  ACTION_SEQ__H_39,
  ACTION_SEQ__HOLD_LAYER_COLOR_41,
  ACTION_SEQ__DO_NEXT_MAPPING_43
};

static int dance_key_states[23] = {0};

void dance_flush_on_many_taps(qk_tap_dance_state_t *state) {
  if (state->count == 0) {
    return;
  }
  uint16_t dance_key = state->keycode - QK_TAP_DANCE;

  switch (dance_key) {
    
    case DANCE_KC_YADANCE:
      if (state->count <= 1) {
        return;
      }
      break;
 

    case DANCE_KC_YBDANCE:
      if (state->count <= 1) {
        return;
      }
      break;
 

    case DANCE_KC_YCDANCE:
      if (state->count <= 1) {
        return;
      }
      break;
 

    case DANCE_KC_YDDANCE:
      if (state->count <= 1) {
        return;
      }
      break;
 

    case DANCE_KC_YEDANCE:
      if (state->count <= 1) {
        return;
      }
      break;
 

    case DANCE_KC_YFDANCE:
      if (state->count <= 1) {
        return;
      }
      break;
 

    case DANCE_KC_YCTRLDANCE:
      if (state->count <= 2) {
        return;
      }
      break;
 

    case DANCE_KC_YALTDANCE:
      if (state->count <= 1) {
        return;
      }
      break;
 

    case DANCE_KC_YGUIDANCE:
      if (state->count <= 2) {
        return;
      }
      break;
 

    case DANCE_KC_YHDANCE:
      if (state->count <= 1) {
        return;
      }
      break;
 

    case DANCE_KC_YMAPPINGDANCE:
      if (state->count <= 1) {
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
    
    case DANCE_KC_YADANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            layer_on(LAYER_LAYERA);
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_LAYERA_3;
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
            dance_key_states[dance_key] = ACTION_SEQ__A_1;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_A);
            dance_key_states[dance_key] = ACTION_SEQ__A_1;
              code_up(KC_A);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_YBDANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            layer_on(LAYER_LAYERB);
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_LAYERB_7;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_B);
            dance_key_states[dance_key] = ACTION_SEQ__B_5;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_B);
            dance_key_states[dance_key] = ACTION_SEQ__B_5;
              code_up(KC_B);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_YCDANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            layer_on(LAYER_LAYERC);
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_LAYERC_11;
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
            dance_key_states[dance_key] = ACTION_SEQ__C_9;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_C);
            dance_key_states[dance_key] = ACTION_SEQ__C_9;
              code_up(KC_C);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_YDDANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            layer_on(LAYER_LAYERD);
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_LAYERD_15;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_D);
            dance_key_states[dance_key] = ACTION_SEQ__D_13;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_D);
            dance_key_states[dance_key] = ACTION_SEQ__D_13;
              code_up(KC_D);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_YEDANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            layer_on(LAYER_LAYERE);
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_LAYERE_19;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_E);
            dance_key_states[dance_key] = ACTION_SEQ__E_17;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_E);
            dance_key_states[dance_key] = ACTION_SEQ__E_17;
              code_up(KC_E);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_YFDANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            layer_on(LAYER_LAYERF);
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_LAYERF_23;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_F);
            dance_key_states[dance_key] = ACTION_SEQ__F_21;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F);
            dance_key_states[dance_key] = ACTION_SEQ__F_21;
              code_up(KC_F);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_YCTRLDANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            code_down(KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL_27;
            return;
      
          case 2:
            code_down(KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT_28;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_BSPC);
            dance_key_states[dance_key] = ACTION_SEQ__BSPC_25;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_BSPC);
            dance_key_states[dance_key] = ACTION_SEQ__BSPC_25;
              code_up(KC_BSPC);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_YALTDANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            code_down(KC_LALT);
            dance_key_states[dance_key] = ACTION_SEQ__LALT_32;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_SPC);
            dance_key_states[dance_key] = ACTION_SEQ__SPC_30;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_SPC);
            dance_key_states[dance_key] = ACTION_SEQ__SPC_30;
              code_up(KC_SPC);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_YGUIDANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            code_down(KC_LGUI);
            dance_key_states[dance_key] = ACTION_SEQ__LGUI_36;
            return;
      
          case 2:
            code_down(KC_RSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__RSHIFT_37;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_ENT);
            dance_key_states[dance_key] = ACTION_SEQ__ENT_34;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_ENT);
            dance_key_states[dance_key] = ACTION_SEQ__ENT_34;
              code_up(KC_ENT);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_YHDANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            layer_on(LAYER_COLOR);
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_COLOR_41;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_H);
            dance_key_states[dance_key] = ACTION_SEQ__H_39;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_H);
            dance_key_states[dance_key] = ACTION_SEQ__H_39;
              code_up(KC_H);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_YMAPPINGDANCE:
      if (state->pressed) {
        // Hold actions:
        return;
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            dance_key_states[dance_key] = ACTION_SEQ__DO_NEXT_MAPPING_43;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              dance_key_states[dance_key] = ACTION_SEQ__DO_NEXT_MAPPING_43;
              run_advanced(DO_NEXT_MAPPING);
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
    case ACTION_SEQ__HOLD_LAYER_LAYERA_3:
            layer_off(LAYER_LAYERA);
            break;
        case ACTION_SEQ__A_1:
            code_up(KC_A);
            break;
    
        case ACTION_SEQ__HOLD_LAYER_LAYERB_7:
            layer_off(LAYER_LAYERB);
            break;
        case ACTION_SEQ__B_5:
            code_up(KC_B);
            break;
    
        case ACTION_SEQ__HOLD_LAYER_LAYERC_11:
            layer_off(LAYER_LAYERC);
            break;
        case ACTION_SEQ__C_9:
            code_up(KC_C);
            break;
    
        case ACTION_SEQ__HOLD_LAYER_LAYERD_15:
            layer_off(LAYER_LAYERD);
            break;
        case ACTION_SEQ__D_13:
            code_up(KC_D);
            break;
    
        case ACTION_SEQ__HOLD_LAYER_LAYERE_19:
            layer_off(LAYER_LAYERE);
            break;
        case ACTION_SEQ__E_17:
            code_up(KC_E);
            break;
    
        case ACTION_SEQ__HOLD_LAYER_LAYERF_23:
            layer_off(LAYER_LAYERF);
            break;
        case ACTION_SEQ__F_21:
            code_up(KC_F);
            break;
    
        case ACTION_SEQ__LCTRL_27:
            code_up(KC_LCTRL);
            break;
    
          case ACTION_SEQ__LSHIFT_28:
            code_up(KC_LSHIFT);
            break;
        case ACTION_SEQ__BSPC_25:
            code_up(KC_BSPC);
            break;
    
        case ACTION_SEQ__LALT_32:
            code_up(KC_LALT);
            break;
        case ACTION_SEQ__SPC_30:
            code_up(KC_SPC);
            break;
    
        case ACTION_SEQ__LGUI_36:
            code_up(KC_LGUI);
            break;
    
          case ACTION_SEQ__RSHIFT_37:
            code_up(KC_RSHIFT);
            break;
        case ACTION_SEQ__ENT_34:
            code_up(KC_ENT);
            break;
    
        case ACTION_SEQ__HOLD_LAYER_COLOR_41:
            layer_off(LAYER_COLOR);
            break;
        case ACTION_SEQ__H_39:
            code_up(KC_H);
            break;
    
        
        case ACTION_SEQ__DO_NEXT_MAPPING_43:
            run_advanced(DO_NEXT_MAPPING);
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
    [DANCE_KC_YADANCE] = DANCE_MODIFIER(),
  [DANCE_KC_YBDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_YCDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_YDDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_YEDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_YFDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_YCTRLDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_YALTDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_YGUIDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_YHDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_YMAPPINGDANCE] = DANCE_MODIFIER()
};

enum custom_keycodes {
  ZKC_BTL = SAFE_RANGE,

  // At the end:
  DYNAMIC_MACRO_RANGE,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[LAYER_DEFAULT] = LAYOUT(
  
/* 0 */ TD(DANCE_KC_YADANCE),TD(DANCE_KC_YBDANCE),TD(DANCE_KC_YCDANCE),
/* 1 */ TD(DANCE_KC_YDDANCE),TD(DANCE_KC_YEDANCE),TD(DANCE_KC_YFDANCE),
/* 2 */ TD(DANCE_KC_YCTRLDANCE),TD(DANCE_KC_YALTDANCE),TD(DANCE_KC_YGUIDANCE)
)
  ,

[LAYER_LAYERA] = LAYOUT(
  
/* 0 */ _______,KC_G,TD(DANCE_KC_YHDANCE),
/* 1 */ KC_I,KC_J,KC_K,
/* 2 */ KC_L,KC_M,KC_N
)
  ,

[LAYER_LAYERB] = LAYOUT(
  
/* 0 */ KC_O,_______,KC_P,
/* 1 */ KC_Q,KC_R,KC_S,
/* 2 */ KC_T,KC_U,KC_V
)
  ,

[LAYER_LAYERC] = LAYOUT(
  
/* 0 */ KC_W,KC_X,_______,
/* 1 */ KC_Y,KC_Z,KC_COMM,
/* 2 */ KC_SCOLON,KC_LBRC,KC_RBRC
)
  ,

[LAYER_LAYERD] = LAYOUT(
  
/* 0 */ KC_QUOT,KC_DOT,KC_GRV,
/* 1 */ _______,KC_SLSH,KC_BSLS,
/* 2 */ KC_ESC,KC_TAB,KC_DELETE
)
  ,

[LAYER_LAYERE] = LAYOUT(
  
/* 0 */ KC_1,KC_2,KC_3,
/* 1 */ KC_4,_______,KC_6,
/* 2 */ KC_7,KC_8,KC_9
)
  ,

[LAYER_LAYERF] = LAYOUT(
  
/* 0 */ KC_MINS,TG(LAYER_NAVIGATION),KC_EQL,
/* 1 */ KC_0,KC_5,_______,
/* 2 */ UC_M_WC,UC_M_OS,TD(DANCE_KC_YMAPPINGDANCE)
)
  ,

[LAYER_NAVIGATION] = LAYOUT(
  
/* 0 */ KC_PGDN,TG(LAYER_NAVIGATION),KC_PGUP,
/* 1 */ KC_BTN1,KC_MS_U,KC_BTN2,
/* 2 */ KC_MS_L,KC_MS_D,KC_MS_R
)
  ,

[LAYER_COLOR] = LAYOUT(
  
/* 0 */ _______,RGB_TOG,_______,
/* 1 */ RGB_MOD,RGB_VAD,RGB_VAI,
/* 2 */ ZKC_BTL,RGB_HUD,RGB_HUI
)
  
};



void matrix_init_user(void) {
  #ifdef STENO_ENABLE
    steno_set_mode(STENO_MODE_GEMINI);
  #endif

}



bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  bool pressed = record->event.pressed;

  uint16_t newKeycode = map_code16_hash(keycode);
  if (newKeycode > 0) {
    if (pressed) {
      register_code(newKeycode);
    } else {
      unregister_code(newKeycode);
    }
    return false;
  }

  if (!pressed) {
    return true;
  }

  switch (keycode) {
    case ZKC_BTL:
      run_advanced(DO_BOOTLOADER);
      break;

    case UC_M_OS:
      zored_os = OS_MACOS;
      break;

    case UC_M_WC:
      zored_os = OS_WINDOWS;
      break;

    default:
      return true;
  }

  return false;
}


