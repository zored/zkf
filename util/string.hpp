#pragma once
#include <string_view>

/**
 * Force bail-out from constexpr evaluation: [expr.const] p2.2
 */
void failwith(const std::string_view& msg) {

}

/* #define U32_STRINGS */

#ifdef U32_STRINGS
#   define STR(x) U##x##sv
#else
#   define STR(x) x##sv
#endif

namespace util {
    template<class CharT>
    struct ci_char_traits: public std::char_traits<CharT> {
        /* FIXME: ASCII-only to_upper implementation */
        constexpr static CharT to_upper(const CharT& c) {
            return (c >= 97 && c <= 122) ? (c - 32) : c;
        }
        constexpr static int compare(const CharT* fst, const CharT* snd, size_t n) {
            for (size_t i = 0; i < n; ++i) {
                if (to_upper(fst[i]) != to_upper(snd[i])) {
                    return to_upper(fst[i]) - to_upper(snd[i]);
                }
            }
            return 0;
        }
    };

    template<class CharT>
    using basic_string = std::basic_string_view<CharT, ci_char_traits<CharT>>;

    namespace literals {
        inline constexpr basic_string<char>
        operator ""sv(const char* str,  size_t len) noexcept {
            return basic_string<char>{str, len};
        }

        inline constexpr basic_string<char32_t>
        operator ""sv(const char32_t* str,  size_t len) noexcept {
            return basic_string<char32_t>{str, len};
        }
    } // literals

    using namespace util::literals;

    template<class CharT>
    constexpr const auto get_char(const basic_string<CharT>& str, const size_t pos = 0) {
        return std::pair{str[pos], 1};
    }

    /* Not quite correct UTF-8 decoder. */
    template <>
    constexpr const auto get_char<char>(const basic_string<char>& str, const size_t pos) {
        char32_t codePoint = str[pos];
        size_t seq = 0;
        if ((codePoint & 0x80) == 0) {
            seq = 1;
        } else if ((codePoint & 0xE0) == 0xC0) {
            seq = 2;
            codePoint = (codePoint & 0x1F) << 6;
        } else if ((codePoint & 0xF0) == 0xE0) {
            seq = 3;
            codePoint = (codePoint & 0x1F) << 12;
        } else if ((codePoint & 0xF8) == 0xF0) {
            seq = 4;
            codePoint = (codePoint & 0x07) << 18;
        } else {
            failwith("Invalid UTF-8 sequence");
        }

        if (seq > str.size()) {
            failwith("Incomplete UTF-8 sequence");
        }

        for (size_t i = 1; i < seq; ++i) {
            codePoint |= ((char32_t)str[pos + i] & 0x3F) << ((seq - i - 1) * 6);
        }
        return std::pair{codePoint, seq};
    }

    constexpr bool is_delimiter(const char32_t& c) {
        return (c >= 0x2500 && c <= 0x257F) || (c == '\n');
    }

    constexpr bool is_space(const char32_t& c) {
        return (c ==  ' ')
            || (c >=   0x09 && c <=   0x0D)
            || (c == 0x1680)||(c == 0x180E)
            || (c >= 0x2000 && c <= 0x2006)
            || (c >= 0x2008 && c <= 0x200A)
            || (c == 0x2028)||(c == 0x2029)
            || (c == 0x205F)||(c == 0x3000)
            ;
    }

    template<typename CharT>
    constexpr auto trim(const basic_string<CharT>& str) {
        size_t left = 0;
        while (left < str.size()) {
            const auto [code, len] = get_char(str, left);
            if (!is_space(code)) {
                break;
            }
            left += len;
        }
        if (left >= str.size()) {
            return STR("");
        }
        size_t right = str.size() - 1;
        /* FIXME: this only works correctly for char32_t */
        while(is_space(str[right])) {
            --right;
        }
        return str.substr(left, right - left + 1);
    }

    template<typename CharT, typename Fn>
    constexpr auto take_while(const basic_string<CharT>& str, const Fn& fn) {
        size_t pos = 0;
        while (pos < str.size()) {
            const auto [codePoint, len] = util::get_char(str, pos);
            if (!fn(codePoint)) {
                break;
            }
            pos += len;
        }
        return str.substr(0, pos);
    }

    /* GCC 7.x: basic_string_view::compare is not constexpr; */
    template<typename CharT>
    constexpr bool operator == (const basic_string<CharT>& fst, const basic_string<CharT>& snd) {
        const auto ret = basic_string<CharT>::traits_type::compare(fst.data(), snd.data(),
                std::min(fst.size(), snd.size()));
        return (ret == 0) && fst.size() == snd.size();
    }
} // util
