#pragma once

#include <tuple>
#include <functional>

namespace ev {

template <class T>
struct function_traits : function_traits<decltype(&std::decay_t<T>::operator())> {
};

// f(...), f
template <class R, class... Args>
struct function_traits<R(Args...)> {
    using type = R(Args...);
    using result_type = R;
    using arguments = std::tuple<Args...>;
    static constexpr size_t arg_count = std::tuple_size_v<arguments>;
    template <size_t index>
    using argument_type = std::tuple_element_t<index, arguments>;
};

// f(...), &f
template <class R, class... Args>
struct function_traits<R (*)(Args...)> {
    using type = R(Args...);
    using result_type = R;
    using arguments = std::tuple<Args...>;
    static constexpr size_t arg_count = std::tuple_size_v<arguments>;
    template <size_t index>
    using argument_type = std::tuple_element_t<index, arguments>;
};

// std::function
template <class R, class... Args>
struct function_traits<std::function<R(Args...)>> {
    using type = R(Args...);
    using result_type = R;
    using arguments = std::tuple<Args...>;
    static constexpr size_t arg_count = std::tuple_size_v<arguments>;
    template <size_t index>
    using argument_type = std::tuple_element_t<index, arguments>;
};

// R Type::f(...), &Type::f
template <class T, class R, class... Args>
struct function_traits<R (T::*)(Args...)> {
    using object_type = T;
    using type = R (T::*)(Args...);
    using result_type = R;

    using arguments = std::tuple<Args...>;

    static constexpr size_t arg_count = std::tuple_size_v<arguments>;

    template <size_t index>
    using argument_type = std::tuple_element_t<index, arguments>;
};

// R Type::f(...) const, &Type::f
template <class T, class R, class... Args>
struct function_traits<R (T::*)(Args...) const> {
    using object_type = T;
    using type = R (T::*)(Args...) const;
    using result_type = R;

    using arguments = std::tuple<Args...>;

    static constexpr size_t arg_count = std::tuple_size_v<arguments>;

    template <size_t index>
    using argument_type = std::tuple_element_t<index, arguments>;
};
}
