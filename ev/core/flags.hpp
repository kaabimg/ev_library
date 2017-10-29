#pragma once

#include <type_traits>

namespace ev {
template <typename Enum>
class flags {
public:
    using data_type = typename std::underlying_type<Enum>::type;

    flags()
    {
    }
    flags(Enum val)
    {
        m_data = static_cast<data_type>(val);
    }
    data_type data() const
    {
        return m_data;
    }
    static flags from_value(data_type val)
    {
        flags f;
        f.m_data = static_cast<data_type>(val);
        return f;
    }

    flags operator|(Enum val)
    {
        return from_value(m_data | static_cast<data_type>(val));
    }

    bool test(Enum val)
    {
        return (m_data & static_cast<data_type>(val)) != 0;
    }
    void clear(Enum val)
    {
        m_data &= ~static_cast<data_type>(val);
    }
    void enable(Enum val)
    {
        m_data |= static_cast<data_type>(val);
    }

private:
    data_type m_data = 0;
};

template <typename Enum>
constexpr typename std::underlying_type<Enum>::type as_int(Enum val)
{
    return static_cast<typename std::underlying_type<Enum>::type>(val);
}
}

#define EV_FLAGS(Enum)                               \
    inline ev::flags<Enum> operator|(Enum a, Enum b) \
    {                                                \
        return ev::flags<Enum>(a) | b;               \
    }
