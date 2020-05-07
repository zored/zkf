


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
LAYER_DEFAULT = 0,
  LAYER_COLOR
};

enum dance_keys {
  DANCE_KC_YMD1DANCE = 1,
  DANCE_KC_YMD2DANCE,
  DANCE_KC_YMD3DANCE,
  DANCE_KC_YMD4DANCE,
  DANCE_KC_YMD5DANCE,
  DANCE_KC_YMD6DANCE,
  DANCE_KC_YMD7DANCE,
  DANCE_KC_YMD8DANCE,
  DANCE_KC_YMD9DANCE
};
enum dance_action_names {
  ACTION_SEQ__1_1 = 1,
  ACTION_SEQ__0_2,
  ACTION_SEQ__SPC_3,
  ACTION_SEQ__HOLD_LAYER_COLOR_5,
  ACTION_SEQ__2_7,
  ACTION_SEQ__A_8,
  ACTION_SEQ__B_9,
  ACTION_SEQ__C_10,
  ACTION_SEQ__3_13,
  ACTION_SEQ__D_14,
  ACTION_SEQ__E_15,
  ACTION_SEQ__F_16,
  ACTION_SEQ__4_19,
  ACTION_SEQ__G_20,
  ACTION_SEQ__H_21,
  ACTION_SEQ__I_22,
  ACTION_SEQ__5_25,
  ACTION_SEQ__J_26,
  ACTION_SEQ__K_27,
  ACTION_SEQ__L_28,
  ACTION_SEQ__6_31,
  ACTION_SEQ__M_32,
  ACTION_SEQ__N_33,
  ACTION_SEQ__O_34,
  ACTION_SEQ__7_37,
  ACTION_SEQ__P_38,
  ACTION_SEQ__Q_39,
  ACTION_SEQ__R_40,
  ACTION_SEQ__S_41,
  ACTION_SEQ__8_44,
  ACTION_SEQ__T_45,
  ACTION_SEQ__U_46,
  ACTION_SEQ__V_47,
  ACTION_SEQ__9_50,
  ACTION_SEQ__W_51,
  ACTION_SEQ__X_52,
  ACTION_SEQ__Y_53,
  ACTION_SEQ__Z_54
};

static int dance_key_states[38] = {0};

void dance_flush_on_many_taps(qk_tap_dance_state_t *state) {
  if (state->count == 0) {
    return;
  }
  uint16_t dance_key = state->keycode - QK_TAP_DANCE;

  switch (dance_key) {
    
    case DANCE_KC_YMD1DANCE:
      if (state->count <= 3) {
        return;
      }
      break;
 

    case DANCE_KC_YMD2DANCE:
      if (state->count <= 4) {
        return;
      }
      break;
 

    case DANCE_KC_YMD3DANCE:
      if (state->count <= 4) {
        return;
      }
      break;
 

    case DANCE_KC_YMD4DANCE:
      if (state->count <= 4) {
        return;
      }
      break;
 

    case DANCE_KC_YMD5DANCE:
      if (state->count <= 4) {
        return;
      }
      break;
 

    case DANCE_KC_YMD6DANCE:
      if (state->count <= 4) {
        return;
      }
      break;
 

    case DANCE_KC_YMD7DANCE:
      if (state->count <= 5) {
        return;
      }
      break;
 

    case DANCE_KC_YMD8DANCE:
      if (state->count <= 4) {
        return;
      }
      break;
 

    case DANCE_KC_YMD9DANCE:
      if (state->count <= 5) {
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
    
    case DANCE_KC_YMD1DANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            layer_on(LAYER_COLOR);
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_COLOR_5;
            return;
      
          default:
            
            return;
        }
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_1);
            dance_key_states[dance_key] = ACTION_SEQ__1_1;
            return;
      
          case 2:
            code_down(KC_0);
            dance_key_states[dance_key] = ACTION_SEQ__0_2;
            return;
      
          case 3:
            code_down(KC_SPC);
            dance_key_states[dance_key] = ACTION_SEQ__SPC_3;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_1);
            dance_key_states[dance_key] = ACTION_SEQ__1_1;
              code_up(KC_1);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_YMD2DANCE:
      if (state->pressed) {
        // Hold actions:
        return;
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_2);
            dance_key_states[dance_key] = ACTION_SEQ__2_7;
            return;
      
          case 2:
            code_down(KC_A);
            dance_key_states[dance_key] = ACTION_SEQ__A_8;
            return;
      
          case 3:
            code_down(KC_B);
            dance_key_states[dance_key] = ACTION_SEQ__B_9;
            return;
      
          case 4:
            code_down(KC_C);
            dance_key_states[dance_key] = ACTION_SEQ__C_10;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_2);
            dance_key_states[dance_key] = ACTION_SEQ__2_7;
              code_up(KC_2);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_YMD3DANCE:
      if (state->pressed) {
        // Hold actions:
        return;
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_3);
            dance_key_states[dance_key] = ACTION_SEQ__3_13;
            return;
      
          case 2:
            code_down(KC_D);
            dance_key_states[dance_key] = ACTION_SEQ__D_14;
            return;
      
          case 3:
            code_down(KC_E);
            dance_key_states[dance_key] = ACTION_SEQ__E_15;
            return;
      
          case 4:
            code_down(KC_F);
            dance_key_states[dance_key] = ACTION_SEQ__F_16;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_3);
            dance_key_states[dance_key] = ACTION_SEQ__3_13;
              code_up(KC_3);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_YMD4DANCE:
      if (state->pressed) {
        // Hold actions:
        return;
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_4);
            dance_key_states[dance_key] = ACTION_SEQ__4_19;
            return;
      
          case 2:
            code_down(KC_G);
            dance_key_states[dance_key] = ACTION_SEQ__G_20;
            return;
      
          case 3:
            code_down(KC_H);
            dance_key_states[dance_key] = ACTION_SEQ__H_21;
            return;
      
          case 4:
            code_down(KC_I);
            dance_key_states[dance_key] = ACTION_SEQ__I_22;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_4);
            dance_key_states[dance_key] = ACTION_SEQ__4_19;
              code_up(KC_4);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_YMD5DANCE:
      if (state->pressed) {
        // Hold actions:
        return;
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_5);
            dance_key_states[dance_key] = ACTION_SEQ__5_25;
            return;
      
          case 2:
            code_down(KC_J);
            dance_key_states[dance_key] = ACTION_SEQ__J_26;
            return;
      
          case 3:
            code_down(KC_K);
            dance_key_states[dance_key] = ACTION_SEQ__K_27;
            return;
      
          case 4:
            code_down(KC_L);
            dance_key_states[dance_key] = ACTION_SEQ__L_28;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_5);
            dance_key_states[dance_key] = ACTION_SEQ__5_25;
              code_up(KC_5);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_YMD6DANCE:
      if (state->pressed) {
        // Hold actions:
        return;
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_6);
            dance_key_states[dance_key] = ACTION_SEQ__6_31;
            return;
      
          case 2:
            code_down(KC_M);
            dance_key_states[dance_key] = ACTION_SEQ__M_32;
            return;
      
          case 3:
            code_down(KC_N);
            dance_key_states[dance_key] = ACTION_SEQ__N_33;
            return;
      
          case 4:
            code_down(KC_O);
            dance_key_states[dance_key] = ACTION_SEQ__O_34;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_6);
            dance_key_states[dance_key] = ACTION_SEQ__6_31;
              code_up(KC_6);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_YMD7DANCE:
      if (state->pressed) {
        // Hold actions:
        return;
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_7);
            dance_key_states[dance_key] = ACTION_SEQ__7_37;
            return;
      
          case 2:
            code_down(KC_P);
            dance_key_states[dance_key] = ACTION_SEQ__P_38;
            return;
      
          case 3:
            code_down(KC_Q);
            dance_key_states[dance_key] = ACTION_SEQ__Q_39;
            return;
      
          case 4:
            code_down(KC_R);
            dance_key_states[dance_key] = ACTION_SEQ__R_40;
            return;
      
          case 5:
            code_down(KC_S);
            dance_key_states[dance_key] = ACTION_SEQ__S_41;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_7);
            dance_key_states[dance_key] = ACTION_SEQ__7_37;
              code_up(KC_7);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_YMD8DANCE:
      if (state->pressed) {
        // Hold actions:
        return;
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_8);
            dance_key_states[dance_key] = ACTION_SEQ__8_44;
            return;
      
          case 2:
            code_down(KC_T);
            dance_key_states[dance_key] = ACTION_SEQ__T_45;
            return;
      
          case 3:
            code_down(KC_U);
            dance_key_states[dance_key] = ACTION_SEQ__U_46;
            return;
      
          case 4:
            code_down(KC_V);
            dance_key_states[dance_key] = ACTION_SEQ__V_47;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_8);
            dance_key_states[dance_key] = ACTION_SEQ__8_44;
              code_up(KC_8);
            }
            return;
        }
      }
      break;
    
    case DANCE_KC_YMD9DANCE:
      if (state->pressed) {
        // Hold actions:
        return;
      }
      else {
        // Tap actions:
        switch (state->count) {
          case 1:
            code_down(KC_9);
            dance_key_states[dance_key] = ACTION_SEQ__9_50;
            return;
      
          case 2:
            code_down(KC_W);
            dance_key_states[dance_key] = ACTION_SEQ__W_51;
            return;
      
          case 3:
            code_down(KC_X);
            dance_key_states[dance_key] = ACTION_SEQ__X_52;
            return;
      
          case 4:
            code_down(KC_Y);
            dance_key_states[dance_key] = ACTION_SEQ__Y_53;
            return;
      
          case 5:
            code_down(KC_Z);
            dance_key_states[dance_key] = ACTION_SEQ__Z_54;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_9);
            dance_key_states[dance_key] = ACTION_SEQ__9_50;
              code_up(KC_9);
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
    case ACTION_SEQ__HOLD_LAYER_COLOR_5:
            layer_off(LAYER_COLOR);
            break;
        case ACTION_SEQ__1_1:
            code_up(KC_1);
            break;
    
          case ACTION_SEQ__0_2:
            code_up(KC_0);
            break;
    
          case ACTION_SEQ__SPC_3:
            code_up(KC_SPC);
            break;
    
        
        case ACTION_SEQ__2_7:
            code_up(KC_2);
            break;
    
          case ACTION_SEQ__A_8:
            code_up(KC_A);
            break;
    
          case ACTION_SEQ__B_9:
            code_up(KC_B);
            break;
    
          case ACTION_SEQ__C_10:
            code_up(KC_C);
            break;
    
        
        case ACTION_SEQ__3_13:
            code_up(KC_3);
            break;
    
          case ACTION_SEQ__D_14:
            code_up(KC_D);
            break;
    
          case ACTION_SEQ__E_15:
            code_up(KC_E);
            break;
    
          case ACTION_SEQ__F_16:
            code_up(KC_F);
            break;
    
        
        case ACTION_SEQ__4_19:
            code_up(KC_4);
            break;
    
          case ACTION_SEQ__G_20:
            code_up(KC_G);
            break;
    
          case ACTION_SEQ__H_21:
            code_up(KC_H);
            break;
    
          case ACTION_SEQ__I_22:
            code_up(KC_I);
            break;
    
        
        case ACTION_SEQ__5_25:
            code_up(KC_5);
            break;
    
          case ACTION_SEQ__J_26:
            code_up(KC_J);
            break;
    
          case ACTION_SEQ__K_27:
            code_up(KC_K);
            break;
    
          case ACTION_SEQ__L_28:
            code_up(KC_L);
            break;
    
        
        case ACTION_SEQ__6_31:
            code_up(KC_6);
            break;
    
          case ACTION_SEQ__M_32:
            code_up(KC_M);
            break;
    
          case ACTION_SEQ__N_33:
            code_up(KC_N);
            break;
    
          case ACTION_SEQ__O_34:
            code_up(KC_O);
            break;
    
        
        case ACTION_SEQ__7_37:
            code_up(KC_7);
            break;
    
          case ACTION_SEQ__P_38:
            code_up(KC_P);
            break;
    
          case ACTION_SEQ__Q_39:
            code_up(KC_Q);
            break;
    
          case ACTION_SEQ__R_40:
            code_up(KC_R);
            break;
    
          case ACTION_SEQ__S_41:
            code_up(KC_S);
            break;
    
        
        case ACTION_SEQ__8_44:
            code_up(KC_8);
            break;
    
          case ACTION_SEQ__T_45:
            code_up(KC_T);
            break;
    
          case ACTION_SEQ__U_46:
            code_up(KC_U);
            break;
    
          case ACTION_SEQ__V_47:
            code_up(KC_V);
            break;
    
        
        case ACTION_SEQ__9_50:
            code_up(KC_9);
            break;
    
          case ACTION_SEQ__W_51:
            code_up(KC_W);
            break;
    
          case ACTION_SEQ__X_52:
            code_up(KC_X);
            break;
    
          case ACTION_SEQ__Y_53:
            code_up(KC_Y);
            break;
    
          case ACTION_SEQ__Z_54:
            code_up(KC_Z);
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
    [DANCE_KC_YMD1DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_YMD2DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_YMD3DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_YMD4DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_YMD5DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_YMD6DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_YMD7DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_YMD8DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_YMD9DANCE] = DANCE_MODIFIER()
};

enum custom_keycodes {
  ZKC_BTL = SAFE_RANGE,

  // At the end:
  DYNAMIC_MACRO_RANGE,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[LAYER_DEFAULT] = LAYOUT(
  
/* 0 */ TD(DANCE_KC_YMD1DANCE),TD(DANCE_KC_YMD2DANCE),TD(DANCE_KC_YMD3DANCE),
/* 1 */ TD(DANCE_KC_YMD4DANCE),TD(DANCE_KC_YMD5DANCE),TD(DANCE_KC_YMD6DANCE),
/* 2 */ TD(DANCE_KC_YMD7DANCE),TD(DANCE_KC_YMD8DANCE),TD(DANCE_KC_YMD9DANCE)
)
  ,

[LAYER_COLOR] = LAYOUT(
  
/* 0 */ _______,RGB_HUD,RGB_HUI,
/* 1 */ _______,RGB_VAD,RGB_VAI,
/* 2 */ ZKC_BTL,RGB_TOG,RGB_MOD
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


