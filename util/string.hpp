#pragma once
#include <string_view>

/**
 * Force bail-out from constexpr evaluation: [expr.const] p2.2
 */
void failwith(const std::string_view& msg) {

}

namespace util {
    using string = std::string_view;
    using namespace std::literals;

    /**
     * Not quite correct UTF-8 decoder.
     *
     * In order to filter Unicode characters by blocks we need
     * actual character codes instead of variable length sequences
     */
    constexpr const auto get_char(const string& str) {
        char32_t codePoint = str[0];
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
            codePoint |= ((char32_t)str[i] & 0x3F) << ((seq - i - 1) * 6);
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

    constexpr auto trim(const string& str) {
        size_t left = 0;
        while(is_space(str[left])) {
            ++left;
        }
        if (left >= str.size()) {
            return ""sv;
        }
        size_t right = str.size() - 1;
        while(is_space(str[right])) {
            --right;
        }
        return str.substr(left, right - left + 1);
    }

    template<typename Fn>
    constexpr auto take_while(const util::string& str, const Fn& fn) {
        size_t pos = 0;
        while (pos < str.size()) {
            const auto [codePoint, len] = util::get_char(str.substr(pos));
            if (!fn(codePoint)) {
                break;
            }
            pos += len;
        }
        return str.substr(0, pos);
    }

    constexpr char to_upper(const char32_t& c) {
        return (c >= 97 && c <= 122) ? (c - 32) : c;
    }

    constexpr int compare_ci(const string& fst, const string& snd) {
        for (size_t i = 0; i < std::min(fst.size(), snd.size()); ++i) {
            if (to_upper(fst[i]) != to_upper(snd[i])) {
                return to_upper(fst[i]) - to_upper(snd[i]);
            }
        }
        return fst.size() - snd.size();
    }
} // util
