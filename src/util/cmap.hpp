#pragma once
#include <array>
#include <utility>

namespace util {
    template<typename Key, typename Value, size_t Size>
    class cmap {
        std::array<std::pair<Key, Value>, Size> _data;

    public:
        template<class ...Items>
        constexpr cmap(Items&&... items)
            :_data{std::forward<Items>(items)...} {

        }
        constexpr const Value operator[] (const Key& key) const {
            for (const auto& elem: _data) {
                if (elem.first == key) {
                    return elem.second;
                }
            }
        }
    };

    template <typename Key, typename Value, typename ...Pairs>
    static constexpr auto make_cmap(Pairs const& ...pairs) {
        return cmap<Key, Value, sizeof...(Pairs)>{pairs...};
    }
} // util
