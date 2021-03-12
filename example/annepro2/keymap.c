



#include <stdint.h>
#include "annepro2.h"
#include "qmk_ap2_led.h"
#include "config.h"

#include "action_layer.h"
#include "version.h"

#ifdef STENO_ENABLE
 #include "keymap_steno.h"
#endif

enum custom_keycodes {
  ZKC_BTL = SAFE_RANGE,
  KC_DO_NEXT_LANGUAGE,
KC_DO_BOOTLOADER,


  // At the end:
  DYNAMIC_MACRO_RANGE,
};

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
  DO_MOUSE_SLOW, DO_MOUSE_FAST,
};






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
    case DO_FUTURE:
      switch (zored_os) {
        case OS_MACOS:
          tap_code16(G(KC_LBRACKET));
          break;
        case OS_WINDOWS:
          tap_code16(A(KC_LEFT));
          break;
      }
      break;
    case DO_PAST:
      switch (zored_os) {
        case OS_MACOS:
          tap_code16(G(KC_RBRACKET));
          break;
        case OS_WINDOWS:
          tap_code16(A(KC_RIGHT));
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
  LAYER_NAVIGATION,
  LAYER_NAVIGATION2,
  LAYER_EMOJI,
  LAYER_SYMBOL,
  LAYER_HISTORY,
  LAYER_GAME,
  LAYER_GAMEMIRROR,
  LAYER_GAMENUMBERS
};

enum dance_keys {
  DANCE_KC_CAPSDANCE = 1,
  DANCE_KC_ADANCE,
  DANCE_KC_SEMICOLONDANCE,
  DANCE_KC_ZDANCE,
  DANCE_KC_XDANCE,
  DANCE_KC_CDANCE,
  DANCE_KC_COMMADANCE,
  DANCE_KC_DOTDANCE,
  DANCE_KC_SLASHDANCE,
  DANCE_KC_PERCENTDANCE,
  DANCE_KC_CIRCUMFLEXDANCE,
  DANCE_KC_LEFTSQUAREBRACKETDANCE
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
  ACTION_SEQ__Z_16,
  ACTION_SEQ__LCTRL_18,
  ACTION_SEQ__LCTRL__LSHIFT_19,
  ACTION_SEQ__LCTRL__LALT__LGUI_20,
  ACTION_SEQ__X_22,
  ACTION_SEQ__LALT_24,
  ACTION_SEQ__LALT__LSHIFT_25,
  ACTION_SEQ__C_27,
  ACTION_SEQ__LGUI_29,
  ACTION_SEQ__LGUI__LSHIFT_30,
  ACTION_SEQ__COMM_32,
  ACTION_SEQ__RGUI_34,
  ACTION_SEQ__RGUI__RSHIFT_35,
  ACTION_SEQ__DOT_37,
  ACTION_SEQ__RALT_39,
  ACTION_SEQ__RALT__RSHIFT_40,
  ACTION_SEQ__SLSH_42,
  ACTION_SEQ__RCTRL_44,
  ACTION_SEQ__RCTRL__RSHIFT_45,
  ACTION_SEQ__LSHIFT__5_47,
  ACTION_SEQ__LCTRL_49,
  ACTION_SEQ__LCTRL__LSHIFT_50,
  ACTION_SEQ__LCTRL__LALT__LGUI_51,
  ACTION_SEQ__LSHIFT__6_53,
  ACTION_SEQ__LALT_55,
  ACTION_SEQ__LALT__LSHIFT_56,
  ACTION_SEQ__LBRC_58,
  ACTION_SEQ__LGUI_60,
  ACTION_SEQ__LGUI__LSHIFT_61
};

static int dance_key_states[38] = {0};

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
    
    case DANCE_KC_ZDANCE:
      if (state->pressed) {
        // Hold actions:
        switch (state->count) {
          case 1:
            code_down(KC_LCTRL);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL_18;
            return;
      
          case 2:
            code_down_2(KC_LCTRL, KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL__LSHIFT_19;
            return;
      
          case 3:
            code_down_3(KC_LCTRL, KC_LALT, KC_LGUI);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL__LALT__LGUI_20;
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
            dance_key_states[dance_key] = ACTION_SEQ__Z_16;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_Z);
            dance_key_states[dance_key] = ACTION_SEQ__Z_16;
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
            dance_key_states[dance_key] = ACTION_SEQ__LALT_24;
            return;
      
          case 2:
            code_down_2(KC_LALT, KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LALT__LSHIFT_25;
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
            dance_key_states[dance_key] = ACTION_SEQ__X_22;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_X);
            dance_key_states[dance_key] = ACTION_SEQ__X_22;
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
            dance_key_states[dance_key] = ACTION_SEQ__LGUI_29;
            return;
      
          case 2:
            code_down_2(KC_LGUI, KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LGUI__LSHIFT_30;
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
            dance_key_states[dance_key] = ACTION_SEQ__C_27;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_C);
            dance_key_states[dance_key] = ACTION_SEQ__C_27;
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
            dance_key_states[dance_key] = ACTION_SEQ__RGUI_34;
            return;
      
          case 2:
            code_down_2(KC_RGUI, KC_RSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__RGUI__RSHIFT_35;
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
            dance_key_states[dance_key] = ACTION_SEQ__COMM_32;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_COMM);
            dance_key_states[dance_key] = ACTION_SEQ__COMM_32;
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
            dance_key_states[dance_key] = ACTION_SEQ__RALT_39;
            return;
      
          case 2:
            code_down_2(KC_RALT, KC_RSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__RALT__RSHIFT_40;
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
            dance_key_states[dance_key] = ACTION_SEQ__DOT_37;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_DOT);
            dance_key_states[dance_key] = ACTION_SEQ__DOT_37;
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
            dance_key_states[dance_key] = ACTION_SEQ__RCTRL_44;
            return;
      
          case 2:
            code_down_2(KC_RCTRL, KC_RSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__RCTRL__RSHIFT_45;
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
            dance_key_states[dance_key] = ACTION_SEQ__SLSH_42;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_SLSH);
            dance_key_states[dance_key] = ACTION_SEQ__SLSH_42;
              code_up(KC_SLSH);
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
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL_49;
            return;
      
          case 2:
            code_down_2(KC_LCTRL, KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL__LSHIFT_50;
            return;
      
          case 3:
            code_down_3(KC_LCTRL, KC_LALT, KC_LGUI);
            dance_key_states[dance_key] = ACTION_SEQ__LCTRL__LALT__LGUI_51;
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
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__5_47;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down_2(KC_LSHIFT, KC_5);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__5_47;
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
            dance_key_states[dance_key] = ACTION_SEQ__LALT_55;
            return;
      
          case 2:
            code_down_2(KC_LALT, KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LALT__LSHIFT_56;
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
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__6_53;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down_2(KC_LSHIFT, KC_6);
            dance_key_states[dance_key] = ACTION_SEQ__LSHIFT__6_53;
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
            dance_key_states[dance_key] = ACTION_SEQ__LGUI_60;
            return;
      
          case 2:
            code_down_2(KC_LGUI, KC_LSHIFT);
            dance_key_states[dance_key] = ACTION_SEQ__LGUI__LSHIFT_61;
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
            dance_key_states[dance_key] = ACTION_SEQ__LBRC_58;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_LBRC);
            dance_key_states[dance_key] = ACTION_SEQ__LBRC_58;
              code_up(KC_LBRC);
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
    
        case ACTION_SEQ__LCTRL_18:
            code_up(KC_LCTRL);
            break;
    
          case ACTION_SEQ__LCTRL__LSHIFT_19:
            code_up_2(KC_LCTRL, KC_LSHIFT);
            break;
    
          case ACTION_SEQ__LCTRL__LALT__LGUI_20:
            code_up_3(KC_LCTRL, KC_LALT, KC_LGUI);
            break;
        case ACTION_SEQ__Z_16:
            code_up(KC_Z);
            break;
    
        case ACTION_SEQ__LALT_24:
            code_up(KC_LALT);
            break;
    
          case ACTION_SEQ__LALT__LSHIFT_25:
            code_up_2(KC_LALT, KC_LSHIFT);
            break;
        case ACTION_SEQ__X_22:
            code_up(KC_X);
            break;
    
        case ACTION_SEQ__LGUI_29:
            code_up(KC_LGUI);
            break;
    
          case ACTION_SEQ__LGUI__LSHIFT_30:
            code_up_2(KC_LGUI, KC_LSHIFT);
            break;
        case ACTION_SEQ__C_27:
            code_up(KC_C);
            break;
    
        case ACTION_SEQ__RGUI_34:
            code_up(KC_RGUI);
            break;
    
          case ACTION_SEQ__RGUI__RSHIFT_35:
            code_up_2(KC_RGUI, KC_RSHIFT);
            break;
        case ACTION_SEQ__COMM_32:
            code_up(KC_COMM);
            break;
    
        case ACTION_SEQ__RALT_39:
            code_up(KC_RALT);
            break;
    
          case ACTION_SEQ__RALT__RSHIFT_40:
            code_up_2(KC_RALT, KC_RSHIFT);
            break;
        case ACTION_SEQ__DOT_37:
            code_up(KC_DOT);
            break;
    
        case ACTION_SEQ__RCTRL_44:
            code_up(KC_RCTRL);
            break;
    
          case ACTION_SEQ__RCTRL__RSHIFT_45:
            code_up_2(KC_RCTRL, KC_RSHIFT);
            break;
        case ACTION_SEQ__SLSH_42:
            code_up(KC_SLSH);
            break;
    
        case ACTION_SEQ__LCTRL_49:
            code_up(KC_LCTRL);
            break;
    
          case ACTION_SEQ__LCTRL__LSHIFT_50:
            code_up_2(KC_LCTRL, KC_LSHIFT);
            break;
    
          case ACTION_SEQ__LCTRL__LALT__LGUI_51:
            code_up_3(KC_LCTRL, KC_LALT, KC_LGUI);
            break;
        case ACTION_SEQ__LSHIFT__5_47:
            code_up_2(KC_LSHIFT, KC_5);
            break;
    
        case ACTION_SEQ__LALT_55:
            code_up(KC_LALT);
            break;
    
          case ACTION_SEQ__LALT__LSHIFT_56:
            code_up_2(KC_LALT, KC_LSHIFT);
            break;
        case ACTION_SEQ__LSHIFT__6_53:
            code_up_2(KC_LSHIFT, KC_6);
            break;
    
        case ACTION_SEQ__LGUI_60:
            code_up(KC_LGUI);
            break;
    
          case ACTION_SEQ__LGUI__LSHIFT_61:
            code_up_2(KC_LGUI, KC_LSHIFT);
            break;
        case ACTION_SEQ__LBRC_58:
            code_up(KC_LBRC);
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
  [DANCE_KC_ZDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_XDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_CDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_COMMADANCE] = DANCE_MODIFIER(),
  [DANCE_KC_DOTDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_SLASHDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_PERCENTDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_CIRCUMFLEXDANCE] = DANCE_MODIFIER(),
  [DANCE_KC_LEFTSQUAREBRACKETDANCE] = DANCE_MODIFIER()
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[LAYER_DEFAULT] = KEYMAP(
  
/* 0 */ KC_ESC,KC_1,KC_2,KC_3,KC_4,KC_5,KC_6,KC_7,KC_8,KC_9,KC_0,KC_MINS,KC_PPLS,KC_BSPC,
/* 1 */ KC_TAB,KC_Q,KC_W,KC_E,KC_R,KC_T,KC_Y,KC_U,KC_I,KC_O,KC_P,KC_LBRC,KC_RBRC,KC_BSLS,
/* 2 */ TD(DANCE_KC_CAPSDANCE),TD(DANCE_KC_ADANCE),KC_S,KC_D,KC_F,KC_G,KC_H,KC_J,KC_K,KC_L,TD(DANCE_KC_SEMICOLONDANCE),KC_QUOT,KC_ENT,
/* 3 */ KC_LSPO,TD(DANCE_KC_ZDANCE),TD(DANCE_KC_XDANCE),TD(DANCE_KC_CDANCE),KC_V,KC_B,KC_N,KC_M,TD(DANCE_KC_COMMADANCE),TD(DANCE_KC_DOTDANCE),TD(DANCE_KC_SLASHDANCE),KC_RSPC,
/* 4 */ KC_LEAD,_______,_______,KC_SPC,KC_ENT,_______,_______,KC_DO_BOOTLOADER
)
  ,

[LAYER_NAVIGATION] = KEYMAP(
  
/* 0 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 1 */ _______,_______,KC_BTN2,KC_MS_U,KC_BTN1,_______,_______,_______,KC_AP_LED_NEXT_INTENSITY,_______,_______,_______,_______,_______,
/* 2 */ _______,_______,KC_MS_L,KC_MS_D,KC_MS_R,_______,KC_LEFT,KC_DOWN,KC_UP,KC_RGHT,_______,_______,_______,
/* 3 */ _______,_______,_______,_______,_______,_______,KC_WH_L,KC_WH_D,KC_WH_U,KC_WH_R,_______,_______,
/* 4 */ _______,_______,_______,_______,_______,_______,_______,_______
)
  ,

[LAYER_NAVIGATION2] = KEYMAP(
  
/* 0 */ _______,KC_AP2_BT1,KC_AP2_BT2,KC_AP2_BT3,KC_AP2_BT4,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 1 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 2 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 3 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 4 */ _______,_______,_______,_______,_______,_______,_______,_______
)
  ,

[LAYER_EMOJI] = KEYMAP(
  
/* 0 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 1 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 2 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 3 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 4 */ _______,_______,_______,_______,_______,_______,_______,_______
)
  ,

[LAYER_SYMBOL] = KEYMAP(
  
/* 0 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 1 */ _______,KC_EXLM,KC_HASH,KC_LCBR,KC_RCBR,KC_SLSH,KC_EQL,KC_7,KC_8,KC_9,KC_AMPR,KC_PIPE,_______,_______,
/* 2 */ _______,KC_AT,KC_DLR,KC_LPRN,KC_RPRN,KC_GRV,KC_PPLS,KC_4,KC_5,KC_6,KC_PAST,_______,_______,
/* 3 */ _______,TD(DANCE_KC_PERCENTDANCE),TD(DANCE_KC_CIRCUMFLEXDANCE),TD(DANCE_KC_LEFTSQUAREBRACKETDANCE),KC_RBRC,KC_TILD,KC_MINS,KC_1,KC_2,KC_3,KC_SLSH,_______,
/* 4 */ _______,_______,_______,_______,_______,_______,_______,_______
)
  ,

[LAYER_HISTORY] = KEYMAP(
  
/* 0 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 1 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 2 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 3 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 4 */ _______,_______,_______,_______,_______,_______,_______,_______
)
  ,

[LAYER_GAME] = KEYMAP(
  
/* 0 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 1 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 2 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 3 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 4 */ _______,_______,_______,_______,_______,_______,_______,_______
)
  ,

[LAYER_GAMEMIRROR] = KEYMAP(
  
/* 0 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 1 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 2 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 3 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 4 */ _______,_______,_______,_______,_______,_______,_______,_______
)
  ,

[LAYER_GAMENUMBERS] = KEYMAP(
  
/* 0 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 1 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 2 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 3 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 4 */ _______,_______,_______,_______,_______,_______,_______,_______
)
  
};

const uint16_t keymaps_size = sizeof(keymaps);



void matrix_init_user(void) {
  #ifdef STENO_ENABLE
    steno_set_mode(STENO_MODE_GEMINI);
  #endif

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
  bool pressed = record->event.pressed;



  if (!pressed) {
    return true;
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
  

    default:
      return true;
  }

  return false;
}


