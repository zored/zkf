



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







void setMouseSpeed(uint8_t newMouseSpeed) {
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
  LAYER_HISTORY,
  LAYER_GAME,
  LAYER_GAMEMIRROR,
  LAYER_GAMENUMBERS
};

enum dance_keys {
  DANCE_KC_F1DANCE = 1,
  DANCE_KC_F2DANCE,
  DANCE_KC_F3DANCE,
  DANCE_KC_F4DANCE,
  DANCE_KC_F5DANCE,
  DANCE_KC_F6DANCE,
  DANCE_KC_F7DANCE,
  DANCE_KC_F8DANCE,
  DANCE_KC_F9DANCE,
  DANCE_KC_F10DANCE,
  DANCE_KC_SEMICOLONDANCE
};
enum dance_action_names {
  ACTION_SEQ__F1_1 = 1,
  ACTION_SEQ__F11_2,
  ACTION_SEQ__F2_5,
  ACTION_SEQ__F12_6,
  ACTION_SEQ__F3_9,
  ACTION_SEQ__F13_10,
  ACTION_SEQ__F4_13,
  ACTION_SEQ__F14_14,
  ACTION_SEQ__F5_17,
  ACTION_SEQ__F15_18,
  ACTION_SEQ__F6_21,
  ACTION_SEQ__F16_22,
  ACTION_SEQ__F7_25,
  ACTION_SEQ__F17_26,
  ACTION_SEQ__F8_29,
  ACTION_SEQ__F18_30,
  ACTION_SEQ__F9_33,
  ACTION_SEQ__F19_34,
  ACTION_SEQ__F10_37,
  ACTION_SEQ__F20_38,
  ACTION_SEQ__SCOLON_41,
  ACTION_SEQ__HOLD_LAYER_NAVIGATION_43,
  ACTION_SEQ__HOLD_LAYER_NAVIGATION2_44,
  ACTION_SEQ__TOGGLE_LAYER_NAVIGATION_45
};

static int dance_key_states[24] = {0};

void dance_flush_on_many_taps(qk_tap_dance_state_t *state) {
  if (state->count == 0) {
    return;
  }
  uint16_t dance_key = state->keycode - QK_TAP_DANCE;

  switch (dance_key) {
    
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
 

    case DANCE_KC_F10DANCE:
      if (state->count <= 2) {
        return;
      }
      break;
 

    case DANCE_KC_SEMICOLONDANCE:
      if (state->count <= 3) {
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
            dance_key_states[dance_key] = ACTION_SEQ__F1_1;
            return;
      
          case 2:
            code_down(KC_F11);
            dance_key_states[dance_key] = ACTION_SEQ__F11_2;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F1);
            dance_key_states[dance_key] = ACTION_SEQ__F1_1;
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
            dance_key_states[dance_key] = ACTION_SEQ__F2_5;
            return;
      
          case 2:
            code_down(KC_F12);
            dance_key_states[dance_key] = ACTION_SEQ__F12_6;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F2);
            dance_key_states[dance_key] = ACTION_SEQ__F2_5;
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
            dance_key_states[dance_key] = ACTION_SEQ__F3_9;
            return;
      
          case 2:
            code_down(KC_F13);
            dance_key_states[dance_key] = ACTION_SEQ__F13_10;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F3);
            dance_key_states[dance_key] = ACTION_SEQ__F3_9;
              code_up(KC_F3);
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
            dance_key_states[dance_key] = ACTION_SEQ__F4_13;
            return;
      
          case 2:
            code_down(KC_F14);
            dance_key_states[dance_key] = ACTION_SEQ__F14_14;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F4);
            dance_key_states[dance_key] = ACTION_SEQ__F4_13;
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
            dance_key_states[dance_key] = ACTION_SEQ__F5_17;
            return;
      
          case 2:
            code_down(KC_F15);
            dance_key_states[dance_key] = ACTION_SEQ__F15_18;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F5);
            dance_key_states[dance_key] = ACTION_SEQ__F5_17;
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
            dance_key_states[dance_key] = ACTION_SEQ__F6_21;
            return;
      
          case 2:
            code_down(KC_F16);
            dance_key_states[dance_key] = ACTION_SEQ__F16_22;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F6);
            dance_key_states[dance_key] = ACTION_SEQ__F6_21;
              code_up(KC_F6);
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
            dance_key_states[dance_key] = ACTION_SEQ__F7_25;
            return;
      
          case 2:
            code_down(KC_F17);
            dance_key_states[dance_key] = ACTION_SEQ__F17_26;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F7);
            dance_key_states[dance_key] = ACTION_SEQ__F7_25;
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
            dance_key_states[dance_key] = ACTION_SEQ__F8_29;
            return;
      
          case 2:
            code_down(KC_F18);
            dance_key_states[dance_key] = ACTION_SEQ__F18_30;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F8);
            dance_key_states[dance_key] = ACTION_SEQ__F8_29;
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
            dance_key_states[dance_key] = ACTION_SEQ__F9_33;
            return;
      
          case 2:
            code_down(KC_F19);
            dance_key_states[dance_key] = ACTION_SEQ__F19_34;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F9);
            dance_key_states[dance_key] = ACTION_SEQ__F9_33;
              code_up(KC_F9);
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
            dance_key_states[dance_key] = ACTION_SEQ__F10_37;
            return;
      
          case 2:
            code_down(KC_F20);
            dance_key_states[dance_key] = ACTION_SEQ__F20_38;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_F10);
            dance_key_states[dance_key] = ACTION_SEQ__F10_37;
              code_up(KC_F10);
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
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_NAVIGATION_43;
            return;
      
          case 2:
            layer_on(LAYER_NAVIGATION2);
    
            dance_key_states[dance_key] = ACTION_SEQ__HOLD_LAYER_NAVIGATION2_44;
            return;
      
          case 3:
            layer_invert(LAYER_NAVIGATION);
            dance_key_states[dance_key] = ACTION_SEQ__TOGGLE_LAYER_NAVIGATION_45;
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
            dance_key_states[dance_key] = ACTION_SEQ__SCOLON_41;
            return;
      
          default:
            for (int i=0; i < state->count; i++) {
              code_down(KC_SCOLON);
            dance_key_states[dance_key] = ACTION_SEQ__SCOLON_41;
              code_up(KC_SCOLON);
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
    case ACTION_SEQ__F1_1:
            code_up(KC_F1);
            break;
    
          case ACTION_SEQ__F11_2:
            code_up(KC_F11);
            break;
    
        
        case ACTION_SEQ__F2_5:
            code_up(KC_F2);
            break;
    
          case ACTION_SEQ__F12_6:
            code_up(KC_F12);
            break;
    
        
        case ACTION_SEQ__F3_9:
            code_up(KC_F3);
            break;
    
          case ACTION_SEQ__F13_10:
            code_up(KC_F13);
            break;
    
        
        case ACTION_SEQ__F4_13:
            code_up(KC_F4);
            break;
    
          case ACTION_SEQ__F14_14:
            code_up(KC_F14);
            break;
    
        
        case ACTION_SEQ__F5_17:
            code_up(KC_F5);
            break;
    
          case ACTION_SEQ__F15_18:
            code_up(KC_F15);
            break;
    
        
        case ACTION_SEQ__F6_21:
            code_up(KC_F6);
            break;
    
          case ACTION_SEQ__F16_22:
            code_up(KC_F16);
            break;
    
        
        case ACTION_SEQ__F7_25:
            code_up(KC_F7);
            break;
    
          case ACTION_SEQ__F17_26:
            code_up(KC_F17);
            break;
    
        
        case ACTION_SEQ__F8_29:
            code_up(KC_F8);
            break;
    
          case ACTION_SEQ__F18_30:
            code_up(KC_F18);
            break;
    
        
        case ACTION_SEQ__F9_33:
            code_up(KC_F9);
            break;
    
          case ACTION_SEQ__F19_34:
            code_up(KC_F19);
            break;
    
        
        case ACTION_SEQ__F10_37:
            code_up(KC_F10);
            break;
    
          case ACTION_SEQ__F20_38:
            code_up(KC_F20);
            break;
    
        case ACTION_SEQ__HOLD_LAYER_NAVIGATION_43:
            layer_off(LAYER_NAVIGATION);
            break;
    
          case ACTION_SEQ__HOLD_LAYER_NAVIGATION2_44:
            layer_off(LAYER_NAVIGATION2);
            break;
        case ACTION_SEQ__SCOLON_41:
            code_up(KC_SCOLON);
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
    [DANCE_KC_F1DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F2DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F3DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F4DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F5DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F6DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F7DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F8DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F9DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_F10DANCE] = DANCE_MODIFIER(),
  [DANCE_KC_SEMICOLONDANCE] = DANCE_MODIFIER()
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[LAYER_DEFAULT] = KEYMAP(
  
/* 0 */ KC_ESC,TD(DANCE_KC_F1DANCE),TD(DANCE_KC_F2DANCE),TD(DANCE_KC_F3DANCE),TD(DANCE_KC_F4DANCE),TD(DANCE_KC_F5DANCE),TD(DANCE_KC_F6DANCE),TD(DANCE_KC_F7DANCE),TD(DANCE_KC_F8DANCE),TD(DANCE_KC_F9DANCE),TD(DANCE_KC_F10DANCE),KC_MINS,KC_PPLS,KC_BSPC,
/* 1 */ KC_TAB,KC_Q,KC_W,KC_E,KC_R,KC_T,KC_Y,KC_U,KC_I,KC_O,KC_P,KC_LBRC,KC_RBRC,KC_BSLS,
/* 2 */ KC_CAPSLOCK,KC_A,KC_S,KC_D,KC_F,KC_G,KC_H,KC_J,KC_K,KC_L,TD(DANCE_KC_SEMICOLONDANCE),KC_QUOT,KC_ENT,
/* 3 */ KC_LSPO,KC_Z,KC_X,KC_C,KC_V,KC_B,KC_N,KC_M,KC_COMM,KC_DOT,KC_SLSH,KC_RSPC,
/* 4 */ KC_LEAD,_______,_______,KC_SPC,KC_ENT,_______,_______,KC_DO_BOOTLOADER
)
  ,

[LAYER_NAVIGATION] = KEYMAP(
  
/* 0 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 1 */ _______,_______,_______,KC_MS_U,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 2 */ _______,_______,KC_MS_L,KC_MS_D,KC_MS_R,_______,_______,_______,_______,_______,_______,_______,_______,
/* 3 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
/* 4 */ _______,_______,_______,_______,_______,_______,_______,_______
)
  ,

[LAYER_NAVIGATION2] = KEYMAP(
  
/* 0 */ _______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,_______,
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

      case KC_DO_BOOTLOADER:
        run_advanced(DO_BOOTLOADER);
        break;
  

    default:
      return true;
  }

  return false;
}


