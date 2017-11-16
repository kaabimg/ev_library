#pragma once

#include <iostream>

namespace ev {
namespace detail {
// clang-format off
static const char* g_debug_prefix    = "[debug]";
static const char* g_info_prefix     = "[info]";
static const char* g_warning_prefix  = "[warning]";
static const char* g_error_prefix    = "[error]";
static const char* g_critical_prefix = "[critical]";
// clang-format on
}
template <typename T>
struct print_impl {
    static inline void print(std::ostream& out, const T& t)
    {
        out << t;
    }
};

struct printer {
    printer(std::ostream& out, const char* prefix = nullptr) : stream(out)
    {
        if (prefix) stream << prefix;
    }
    ~printer()
    {
        stream << std::endl;
    }

    template <typename T>
    printer& operator<<(const T& d)
    {
        stream << ' ';
        print_impl<T>::print(stream, d);
        return *this;
    }

    printer& write(const char* data, size_t size)
    {
        stream.write(data, size);
        return *this;
    }

private:
    std::ostream& stream;
};

inline printer debug(std::ostream& out = std::cout)
{
    return printer(out, detail::g_debug_prefix);
}

inline printer warning(std::ostream& out = std::cout)
{
    return printer(out, detail::g_warning_prefix);
}
inline printer error(std::ostream& out = std::cout)
{
    return printer(out, detail::g_error_prefix);
}

inline printer critical(std::ostream& out = std::cout)
{
    return printer(out, detail::g_critical_prefix);
}

inline printer info(std::ostream& out = std::cout)
{
    return printer(out, detail::g_info_prefix);
}

}  // ev

#define EV_CUSTOM_PRINTER(type, out, val)                             \
    namespace ev {                                                    \
    template <>                                                       \
    struct print_impl<type> {                                         \
        static inline void print(std::ostream& out, const type& val); \
    };                                                                \
    }                                                                 \
    inline void ev::print_impl<type>::print(std::ostream& out, const type& val)

#define EV_CUSTOM_TEMPLATE_PRINTER(type, out, val)                        \
    namespace ev {                                                        \
    template <typename T>                                                 \
    struct print_impl<type<T>> {                                          \
        static inline void print(std::ostream& out, const type<T>& data); \
    };                                                                    \
    }                                                                     \
    template <typename T>                                                 \
    inline void ev::print_impl<type<T>>::print(std::ostream& out, const type<T>& val)

///////////////////////////////////////////////
///
EV_CUSTOM_PRINTER(bool, ostream, data)
{
    ostream << (data ? "true" : "false");
}

///////////////////////////////////////////////
///
EV_CUSTOM_PRINTER(std::string, ostream, data)
{
    ostream << '\'';
    ostream << data.c_str();
    ostream << "'";
}
