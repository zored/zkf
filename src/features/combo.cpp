// Combos:
// - Unique only!
// - Don't forget to update COMBO_COUNT.

using namespace ZFK;
using namespace std;

combo_t* key_combos;
const int COMBO_COUNT;
ComboCollection* zfkCombos = NULL;

void ComboCollection::apply(void) {
  if (zfkCombos) {
    return;
  }

  zfkCombos = this;
  key_combos = getQmkCombos();
  COMBO_COUNT = getCount();
}

void process_combo_event(Index index, bool pressed) {
  if (zfkCombos) {
    zfkCombos->processQmkEvent(index, pressed);
  }
}


// const uint16_t PROGMEM combo_esc[] = {KC_Y, KC_U, COMBO_END};
// const uint16_t PROGMEM combo_right_arrow[] = {KC_N, KC_M, COMBO_END};
// const uint16_t PROGMEM combo_fat_right_arrow[] = {KC_F, KC_G, COMBO_END};
// const uint16_t PROGMEM combo_underscore[] = {KC_H, KC_J, COMBO_END};
// const uint16_t PROGMEM combo_quit[] = {KC_I, KC_O, COMBO_END};
// const uint16_t PROGMEM combo_backslash[] = {KC_K, KC_L, COMBO_END};


// enum {
//   CMB_ESC = 0,
//   CMB_RAR,
//   CMB_FRAR,
//   CMB_UND,
//   CMB_QUI,
//   CMB_BSLS,
// };

// combo_t key_combos[COMBO_COUNT] = {
//   [CMB_ESC] = COMBO_ACTION(combo_esc),
//   [CMB_RAR] = COMBO_ACTION(combo_right_arrow),
//   [CMB_FRAR] = COMBO_ACTION(combo_fat_right_arrow),
//   [CMB_UND] = COMBO_ACTION(combo_underscore),
//   [CMB_QUI] = COMBO_ACTION(combo_quit),
//   [CMB_BSLS] = COMBO_ACTION(combo_backslash),
// };

// void process_combo_event(uint8_t combo_index, bool pressed) {
//   if (!pressed) {
//     return;
//   }

//   switch(combo_index) {
//     case CMB_ESC:
//       tap_code(KC_ESC);
//       break;

//     case CMB_FRAR:
//       tap_code(KC_EQL);

//       // >
//       register_win_code(KC_LSHIFT);
//       tap_code(KC_DOT);
//       unregister_win_code(KC_LSHIFT);
//       break;

//     case CMB_RAR:
//       tap_code(KC_MINUS);

//       // >
//       register_win_code(KC_LSHIFT);
//       tap_code(KC_DOT);
//       unregister_win_code(KC_LSHIFT);
//       break;

//     case CMB_UND:
//       // _
//       register_win_code(KC_LSHIFT);
//       tap_code(KC_MINUS);
//       unregister_win_code(KC_LSHIFT);
//       break;

//     case CMB_QUI:
//       switch (get_os()) {
//         case OS_WINDOWS:
//           // alt+f4
//           register_win_code(KC_LALT);
//           tap_code(KC_F4);
//           unregister_win_code(KC_LALT);
//           break;

//         case OS_MACOS:
//           // cmd+q
//           register_code(KC_LCMD);
//           tap_code(KC_Q);
//           unregister_code(KC_LCMD);
//           break;
//       }
//       break;

//     case CMB_BSLS:
//       tap_code(KC_BSLS);
//       break;
//   }
// };