#pragma once
#include "logging.hpp"
#include "algorithm.hpp"
#include <vector>
#include <ostream>
#include <string>

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

namespace ev {

template <typename... Args>
struct print_impl<std::tuple<Args...>> {
    static inline void print(std::ostream& out, const std::tuple<Args...>& t)
    {
        out << "(";
        ev::for_each2(t, [&out](size_t index, auto&& elem) {
            out << (index == 0 ? " " : ", ");
            ev::print_impl<std::decay_t<decltype(elem)>>::print(out, elem);
        });
        out << " )";
    }
};
}
