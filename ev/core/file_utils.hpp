#pragma once

#include <fstream>

namespace ev
{
inline std::string load_text_file(const std::string& path)
{
    std::ifstream input_file{path};
    if (input_file) {
        return std::string{std::istreambuf_iterator<char>(input_file),
                           std::istreambuf_iterator<char>()};
    }
    throw std::runtime_error("Failed to read " + path);
}
}
