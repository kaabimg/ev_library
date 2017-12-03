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
        _data = static_cast<data_type>(val);
    }
    data_type data() const
    {
        return _data;
    }
    static flags from_value(data_type val)
    {
        flags f;
        f._data = static_cast<data_type>(val);
        return f;
    }

    flags operator|(Enum val)
    {
        return from_value(_data | static_cast<data_type>(val));
    }

    bool test(Enum val)
    {
        return (_data & static_cast<data_type>(val)) != 0;
    }
    void clear(Enum val)
    {
        _data &= ~static_cast<data_type>(val);
    }
    void enable(Enum val)
    {
        _data |= static_cast<data_type>(val);
    }

private:
    data_type _data = 0;
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
