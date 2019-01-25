#pragma once
#include <array>
#include <utility>

namespace util {
    template<typename Value>
    constexpr int Compare(const Value& fst, const Value& snd) {
        if (fst == snd) {
            return 0;
        } else if (fst > snd) {
            return 1;
        } else {
            return -1;
        }
    }

    template<typename Key, typename Value, size_t Size>
    class ConstMap {

        std::array<std::pair<Key, Value>, Size> _data;
    public:
        template<class ...Items>
        constexpr ConstMap(Items&&... items)
            :_data{std::forward<Items>(items)...} {

        }
        constexpr const Value operator[] (const Key& key) const {
            for (const auto& elem: _data) {
                if (Compare(elem.first, key) == 0) {
                    return elem.second;
                }
            }
        }
    };

    template <typename Key, typename Value, typename ...Pairs>
    static constexpr auto make_map(Pairs const& ...pairs) {
        return ConstMap<Key, Value, sizeof...(Pairs)>{pairs...};
    }
} // util
