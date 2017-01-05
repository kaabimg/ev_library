#pragma once

#include <type_traits>

namespace ev {

template <typename Enum>
class flags_t {
public:
    using data_type = typename std::underlying_type<Enum>::type;

    flags_t() {}
    flags_t(Enum val) { m_data = static_cast<data_type>(val); }

    data_type data() const { return m_data; }

    static flags_t from_value(data_type val) {
        flags_t f;
        f.m_data = static_cast<data_type>(val);
        return f;
    }

    flags_t operator|(Enum val) {
        return from_value(m_data | static_cast<data_type>(val));
    }

    bool test(Enum val) { return (m_data & static_cast<data_type>(val)) != 0; }

    void clear(Enum val) { m_data &= ~static_cast<data_type>(val); }
    void enable(Enum val) { m_data |= static_cast<data_type>(val); }

private:
    data_type m_data = 0;
};
}

#define EV_FLAGS(Enum)                                   \
    inline ev::flags_t<Enum> operator|(Enum a, Enum b) { \
        return ev::flags_t<Enum>(a) | b;                 \
    }
