#pragma once
#include "logging.hpp"
#include "algorithm.hpp"
#include <vector>
#include <ostream>
#include <string>
#include <variant>
#include <optional>

namespace ev {

template <typename T>
struct print_impl<std::vector<T>> {
    static inline void print(std::ostream& out, const std::vector<T>& data)
    {
        out << '[';
        if (data.size()) {
            out << ' ';
            ev::print_impl<T>::print(out, data[0]);
            for (int i = 1; i < data.size(); ++i) {
                out << " , ";
                ev::print_impl<T>::print(out, data[i]);
            }
            out << ' ';
        }
        out << ']';
    }
};

template <typename... Args>
struct print_impl<std::tuple<Args...>> {
    static inline void print(std::ostream& out, const std::tuple<Args...>& t)
    {
        out << "tuple(";
        ev::for_each2(t, [&out](size_t index, auto&& elem) {
            out << (index == 0 ? "" : ", ");
            ev::print_impl<std::decay_t<decltype(elem)>>::print(out, elem);
        });
        out << ")";
    }
};

template <typename... Args>
struct print_impl<std::variant<Args...>> {
    static inline void print(std::ostream& out, const std::variant<Args...>& var)
    {
        std::visit(
            [&](auto&& elem) {
                out << "variant(";
                ev::print_impl<std::decay_t<decltype(elem)>>::print(out, elem);
                out << ")";
            },
            var);
    }
};

template <typename T>
struct print_impl<std::optional<T>> {
    static inline void print(std::ostream& out, const std::optional<T>& opt)
    {
        out << "optional(";
        if (opt)
            ev::print_impl<T>::print(out, *opt);
        else
            out << "null";
        out << ")";
    }
};
}
