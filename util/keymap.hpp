#include <array>

#include <util/map-macro.h>
#include <util/ConstMap.hpp>
#include <util/layout.hpp>
#include <util/string.hpp>

using keycode_t = uint16_t;
using namespace std::literals;

template<size_t Count>
constexpr auto tokenize(const util::string& str) {
    std::array<util::string, Count> words{ 0 };
    size_t counter = 0;
    size_t pos = 0;
    do {
        pos += util::take_while(str.substr(pos), util::is_delimiter).size();
        if (pos >= str.size()) {
            break;
        }
        const auto word = util::take_while(str.substr(pos), [](auto&& chr) {
                return !util::is_delimiter(chr);
            });
        if (word.size() > 0) {
            words[counter++] = util::trim(word);
            pos += word.size();
        }
    } while (pos < str.size() && counter < Count);

    if (counter != Count) {
        failwith("Not enough keycodes");
    }
    return words;
}

namespace util {
    template<>
    constexpr int Compare<util::string>(const util::string& fst, const util::string& snd) {
        return util::compare_ci(fst, snd);
    }
}

#define KC1(x) std::pair{#x##sv, KC_##x}
#define KC2(k,v) std::pair{k##sv, v}

constexpr auto str2keycode = util::make_map<util::string, keycode_t>(
    MAP_LIST(KC1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
    MAP_LIST(KC1, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z),
    MAP_LIST(KC1, LSFT, LSHIFT, LCTL, LCTRL, LGUI, LALT, RALT, RGUI, RCTL, RCTRL, RSFT, RSHIFT),
    MAP_LIST(KC1, SPACE, TAB, DEL, INS, APP, ENTER, BSPC, ESC, BRK, PAUS, CAPS, PSCR),
    MAP_LIST(KC1, HOME, END, PGUP, PGDN),
    //MAP_LIST(KC1, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12),
    MAP_LIST(KC1, FN0, FN1, FN2, FN3, FN4, FN5, FN6, FN7, FN8),

    KC2("F1",  KC_F1),
    KC2("F2",  KC_F2),
    KC2("F3",  KC_F3),
    KC2("F4",  KC_F4),
    KC2("F5",  KC_F5),
    KC2("F6",  KC_F6),
    KC2("F7",  KC_F7),
    KC2("F8",  KC_F8),
    KC2("F9",  KC_F9),
    KC2("F10", KC_F10),
    KC2("F11", KC_F11),
    KC2("F12", KC_F12),


    KC2("-", KC_MINUS),
    KC2("=", KC_EQUAL),
    KC2("\\",KC_BSLASH),
    KC2("`", KC_GRAVE),

    KC2("[", KC_LBRACKET),
    KC2("]", KC_RBRACKET),

    KC2(";", KC_SCOLON),
    KC2("'", KC_QUOTE),

    KC2(",", KC_COMMA),
    KC2(".", KC_DOT),
    KC2("/", KC_SLASH),

    KC2("↑", KC_UP),
    KC2("←", KC_LEFT),
    KC2("↓", KC_DOWN),
    KC2("→", KC_RIGHT),
    KC2("⏮", KC_MEDIA_PREV_TRACK),
    KC2("⏯", KC_MEDIA_PLAY_PAUSE),
    KC2("⏭", KC_MEDIA_NEXT_TRACK),
    KC2("⏹", KC_MEDIA_STOP),
    KC2("🔅", BL_DEC),
    KC2("🔆", BL_INC),
    KC2("🔇", KC_MUTE),
    KC2("🔉", KC_VOLD),
    KC2("🔊", KC_VOLU),
    KC2("RST", RESET)
);

template<size_t N>
constexpr auto keycodes(const std::array<util::string, N>& keys) {
    std::array<keycode_t, N> codes{ 0 };
    for (size_t i = 0; i < keys.size(); ++i) {
        codes[i] = keys[i].size() ? str2keycode(keys[i]) : KC_TRANSPARENT;
    }
    return std::move(codes);
}

