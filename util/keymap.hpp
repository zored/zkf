#include <array>

#include <util/ConstMap.hpp>
#include <util/macros.hpp>
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

constexpr auto str2keycode = util::make_map<util::string, keycode_t>(
#   define KC1(x) std::pair{#x##sv, KC_##x}
#   define KC2(k,v) std::pair{k##sv, v}
    map_list(KC1, // alnum
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        1, 2, 3, 4, 5, 6, 7, 8, 9, 0
    ),
    map_list(KC1, // modifiers
        CAPS, LSFT, LSHIFT, LCTL, LCTRL, LGUI, LALT, RALT, RGUI, RCTL, RCTRL, RSFT, RSHIFT
    ),
    map_list(KC1,
        INS, DEL, HOME, END, PGUP, PGDN, LEFT, UP, DOWN, RGHT, RIGHT,
        SPACE, TAB, APP, ENTER, BSPC, ESC, BRK, PAUS, PSCR
    ),
    map_list(KC1,
         FN0,  FN1,  FN2,  FN3,  FN4,  FN5,  FN6,  FN7,  FN8,  FN9, FN10, FN11,
        FN12, FN13, FN14, FN15, FN16, FN17, FN18, FN19, FN20, FN21, FN22, FN23,
        FN24, FN25, FN26, FN27, FN28, FN29, FN30, FN31
    ),
    map_list(KC2,
        // conflicts with some tmk macros defined as F0...Fxx
        ( "F1",  KC_F1), ( "F2",  KC_F2), ( "F3",  KC_F3), ( "F4",  KC_F4),
        ( "F5",  KC_F5), ( "F6",  KC_F6), ( "F7",  KC_F7), ( "F8",  KC_F8),
        ( "F9",  KC_F9), ("F10", KC_F10), ("F11", KC_F11), ("F12", KC_F12),

        ("F13", KC_F13), ("F14", KC_F14), ("F15", KC_F15), ("F16", KC_F16),
        ("F17", KC_F17), ("F18", KC_F18), ("F19", KC_F19), ("F20", KC_F20),
        ("F21", KC_F21), ("F22", KC_F22), ("F23", KC_F23), ("F24", KC_F24),

        ("-", KC_MINUS),
        ("=", KC_EQUAL),
        ("\\",KC_BSLASH),
        ("`", KC_GRAVE),

        ("[", KC_LBRACKET),
        ("]", KC_RBRACKET),

        (";", KC_SCOLON),
        ("'", KC_QUOTE),

        (",", KC_COMMA),
        (".", KC_DOT),
        ("/", KC_SLASH),

        ("↑", KC_UP),
        ("←", KC_LEFT),
        ("↓", KC_DOWN),
        ("→", KC_RIGHT),
        ("⏮", KC_MEDIA_PREV_TRACK),
        ("⏯", KC_MEDIA_PLAY_PAUSE),
        ("⏭", KC_MEDIA_NEXT_TRACK),
        ("⏹", KC_MEDIA_STOP),
        ("🔅", BL_DEC),
        ("🔆", BL_INC),
        ("🔇", KC_MUTE),
        ("🔉", KC_VOLD),
        ("🔊", KC_VOLU),
        ("RST", RESET)
    )
#undef KC1
#undef KC2
);

template<size_t N>
constexpr auto keycodes(const std::array<util::string, N>& keys) {
    std::array<keycode_t, N> codes{ 0 };
    for (size_t i = 0; i < keys.size(); ++i) {
        codes[i] = keys[i].size() ? str2keycode[keys[i]] : KC_TRANSPARENT;
    }
    return std::move(codes);
}

