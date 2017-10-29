#pragma once

#include <string_view>
#include <vector>

namespace ev {

inline std::vector<std::string_view> split_str(std::string_view str, auto&& predicate)
{
    std::vector<std::string_view> result;
    size_t last_pos = 0;
    const size_t size = str.size();
    for (size_t i = 0; i < size; ++i) {
        if (predicate(str[i])) {
            result.push_back({str.data() + last_pos, i - last_pos});
            last_pos = i + 1;
        }
    }
    if (last_pos < size)
        result.push_back({str.data() + last_pos, size - last_pos});
    else
        result.push_back({str.data() + last_pos, 0});

    return result;
}

inline std::vector<std::string_view> split_str(std::string_view str, char sep)
{
    return split_str(str, [sep](char c) { return c == sep; });
}
}
