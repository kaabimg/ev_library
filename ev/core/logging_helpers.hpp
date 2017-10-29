#pragma once
#include "logging.hpp"
#include <vector>
#include <ostream>
#include <string>

#define EV_CUSTOM_PRINTER(type, out, val)                             \
    namespace ev {                                                    \
    template <>                                                       \
    struct print_impl<type> {                                         \
        static inline void print(std::ostream& out, const type& val); \
    };                                                                \
    }                                                                 \
    void ev::print_impl<type>::print(std::ostream& out, const type& val)

#define EV_CUSTOM_TEMPLATE_PRINTER(type, out, val)                        \
    namespace ev {                                                        \
    template <typename T>                                                 \
    struct print_impl<type<T>> {                                          \
        static inline void print(std::ostream& out, const type<T>& data); \
    };                                                                    \
    }                                                                     \
    template <typename T>                                                 \
    void ev::print_impl<type<T>>::print(std::ostream& out, const type<T>& val)

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

////////////////////////////////////////////////
///
EV_CUSTOM_TEMPLATE_PRINTER(std::vector, ostream, data)
{
    ostream << '[';
    if (data.size()) {
        ostream << ' ';
        ev::print_impl<T>::print(ostream, data[0]);
        for (int i = 1; i < data.size(); ++i) {
            ostream << " , ";
            ev::print_impl<T>::print(ostream, data[i]);
        }
        ostream << ' ';
    }
    ostream << ']';
}
