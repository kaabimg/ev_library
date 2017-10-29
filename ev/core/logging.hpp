#pragma once

#include <iostream>

namespace ev
{
namespace detail
{
static const char* g_debug_prefix    = "[debug...]";
static const char* g_info_prefix     = "[info....]";
static const char* g_warning_prefix  = "[warning.]";
static const char* g_error_prefix    = "[error...]";
static const char* g_critical_prefix = "[critical]";
}
template <typename T>
struct print_impl
{
    static inline void print(std::ostream& out, const T& t) { out << t; }
};

struct printer
{
    printer(std::ostream& out, const char* prefix = nullptr) : stream(out)
    {
        if (prefix) stream << prefix;
    }
    ~printer() { stream << std::endl; }
    template <typename T>
    printer& operator[](const T& d)
    {
        stream << '[';
        print_impl<T>::print(stream, d);
        stream << ']';
        return *this;
    }

    template <typename T>
    printer& operator<<(const T& d)
    {
        stream << ' ';
        print_impl<T>::print(stream, d);
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
