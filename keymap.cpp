extern "C" {
#include QMK_KEYBOARD_H
}

enum Backlight: uint8_t {
#ifdef BACKLIGHT_BREATHING
    BREATHING,
#endif
    OFF,
    ON
};

enum Fn: uint8_t {
    BACKLIGHT = 0
};

static Backlight backlight = Backlight::OFF;

void action_function(keyrecord_t *record, uint8_t id, uint8_t opt) {
    switch(id) {
    case Fn::BACKLIGHT:
        if (record->event.pressed) {
            switch (backlight) {
            case Backlight::ON:
#ifdef BACKLIGHT_BREATHING
                breathing_enable();
                backlight = Backlight::BREATHING;
                break;
            case Backlight::BREATHING:
                breathing_disable();
#endif
                backlight_disable();
                backlight = Backlight::OFF;
                break;
            case Backlight::OFF:
                backlight_enable();
                backlight = Backlight::ON;
                break;
            }
        }
        break;
    }
}

const uint16_t fn_actions[] = {
    [0] = ACTION_LAYER_ONESHOT(1),
    [1] = ACTION_LAYER_ONESHOT(2),
    [2] = ACTION_FUNCTION(Fn::BACKLIGHT)
};

/* Keymap for truefox layout */
#define LAYOUT_truefox_simple( \
    K00, K10, K20, K30, K40, K50, K60, K70, K80, K01, K11, K21, K31, K41, K51, K61, \
    K71, K81, K02, K12, K22, K32, K42, K52, K62, K72, K82, K03, K13, K23,      K33, \
   FN43, K53, K63, K73, K83, K04, K14, K24, K34, K44, K54, K64,      K84,      K05, \
    K15,      K35, K45, K55, K65, K75, K85, K06, K16, K26, K36, K46,      K56, K66, \
    K76, K86, K07,                K17,           K27, K37,           K57, K67, K77  \
) { \
    { KC_##K00, KC_##K01, KC_##K02, KC_##K03, KC_##K04, KC_##K05, KC_##K06, KC_##K07 }, \
    { KC_##K10, KC_##K11, KC_##K12, KC_##K13, KC_##K14, KC_##K15, KC_##K16, KC_##K17 }, \
    { KC_##K20, KC_##K21, KC_##K22, KC_##K23, KC_##K24,    KC_NO, KC_##K26, KC_##K27 }, \
    { KC_##K30, KC_##K31, KC_##K32, KC_##K33, KC_##K34, KC_##K35, KC_##K36, KC_##K37 }, \
    { KC_##K40, KC_##K41, KC_##K42,     FN43, KC_##K44, KC_##K45, KC_##K46,    KC_NO }, \
    { KC_##K50, KC_##K51, KC_##K52, KC_##K53, KC_##K54, KC_##K55, KC_##K56, KC_##K57 }, \
    { KC_##K60, KC_##K61, KC_##K62, KC_##K63, KC_##K64, KC_##K65, KC_##K66, KC_##K67 }, \
    { KC_##K70, KC_##K71, KC_##K72, KC_##K73,    KC_NO, KC_##K75, KC_##K76, KC_##K77 }, \
    { KC_##K80, KC_##K81, KC_##K82, KC_##K83, KC_##K84, KC_##K85, KC_##K86,    KC_NO }  \
}

#define KC__ KC_TRNS
const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/* Layer 0: Base layer
 * ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
 * │Esc│ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 0 │ - │ = │ \ │ ` │Ins│
 * ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴───┼───┤
 * │ Tab │ Q │ W │ E │ R │ T │ Y │ U │ I │ O │ P │ [ │ ] │BSpc │Del│
 * ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴─────┼───┤
 * │ Fn0  │ A │ S │ D │ F │ G │ H │ J │ K │ L │ ; │ ' │ Enter  │Pau│
 * ├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────┬───┼───┤
 * │ LShift │ Z │ X │ C │ V │ B │ N │ M │ , │ . │ / │RShift│ ↑ │App│
 * ├────┬───┴┬──┴─┬─┴───┴───┴───┴───┴───┴──┬┴───┼───┴┬─┬───┼───┼───┤
 * │LCtl│LGui│LAlt│         Space          │RAlt│LCtl│ │ ← │ ↓ │ → │
 * └────┴────┴────┴────────────────────────┴────┴────┘ └───┴───┴───┘
 */

[0] = LAYOUT_truefox_simple( \
    ESC,    1,    2,    3,    4,    5,    6,    7,    8,    9,    0, MINS,  EQL,BSLS,GRV,  INS, \
     TAB,    Q,    W,    E,    R,    T,    Y,    U,    I,    O,    P, LBRC, RBRC,   BSPC,  DEL, \
   KC_FN0,    A,    S,    D,    F,    G,    H,    J,    K,    L, SCLN, QUOT,         ENT, PAUS, \
       LSFT,    Z,    X,    C,    V,    B,    N,    M, COMM,  DOT, SLSH,      RSFT,   UP,  APP, \
   LCTL, LGUI, LALT,                 SPC,                    RALT, RCTL,      LEFT, DOWN, RGHT  \
),
/* Layer 1: Function keys 
 * ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
 * │   │ F1│ F2│ F3│ F4│ F5│ F6│ F7│ F8│ F9│F10│F11│F12│   │   │PSc│
 * ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴───┼───┤
 * │     │   │   │   │   │   │   │   │   │   │   │   │   │     │   │
 * ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴─────┼───┤
 * │ Fn1  │   │   │   │   │   │   │   │   │   │   │   │        │   │
 * ├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────┬───┼───┤
 * │        │   │   │   │   │   │   │   │   │   │   │      │PgU│   │
 * ├────┬───┴┬──┴─┬─┴───┴───┴───┴───┴───┴──┬┴───┼───┴┬─┬───┼───┼───┤
 * │    │    │    │                        │    │    │ │Hom│PgD│End│
 * └────┴────┴────┴────────────────────────┴────┴────┘ └───┴───┴───┘
 */
[1] = LAYOUT_truefox_simple( \
      _,   F1,   F2,   F3,   F4,   F5,   F6,   F7,   F8,   F9,  F10,  F11,  F12,  _,   _, PSCR, \
       _,    _,    _,    _,    _,    _,    _,    _,    _,    _,    _,    _,    _,      _,    _, \
   KC_FN1,    _,    _,    _,    _,    _,    _,    _,    _,    _,    _,    _,           _,    _, \
          _,    _,    _,    _,    _,    _,    _,    _,    _,    _,    _,         _, PGUP,    _, \
      _,    _,    _,                   _,                       _,    _,      HOME, PGDN,  END  \
),
/* Layer 2: Magic keys 
 * ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
 * │   │Tg1│Tg2│   │   │   │   │   │   │   │   │   │   │   │   │RST│
 * ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴───┼───┤
 * │     │   │   │   │   │   │   │   │ ⏮ │ ⏯ │ ⏭ │   │   │     │   │
 * ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴─────┼───┤
 * │ CAPS │   │   │   │   │   │   │   │   │ ⏹ │   │   │        │   │
 * ├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────┬───┼───┤
 * │        │   │   │   │   │   │   │   │   │   │   │      │   │   │
 * ├────┬───┴┬──┴─┬─┴───┴───┴───┴───┴───┴──┬┴───┼───┴┬─┬───┼───┼───┤
 * │    │    │    │                        │    │    │ │   │   │   │
 * └────┴────┴────┴────────────────────────┴────┴────┘ └───┴───┴───┘
 */
#define _ KC_TRNS
#define Fn(x) ACTION_FUNCTION(x)
[2] = LAYOUT_truefox( \
      _,TG(1),TG(2),    _,    _,    _,    _,    _,      _,      _,      _, BL_DEC, BL_INC,KC_FN2,   _,RESET, \
       _,    _,    _,    _,    _,    _,    _,    _,KC_MPRV,KC_MPLY,KC_MNXT,KC_VOLD,KC_VOLU,   KC_MUTE,    _, \
  KC_CAPS,    _,    _,    _,    _,    _,    _,    _,      _,KC_MSTP,KC_BRID,KC_BRIU,                _,    _, \
   	      _,    _,    _,    _,    _,    _,    _,    _,      _,      _,      _,                _,    _,    _, \
      _,    _,    _,                   _,                           _,      _,                _,    _,    _  \
),
#undef _
#undef F
};
