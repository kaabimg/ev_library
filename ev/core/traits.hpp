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

//// Arity

struct wildcard_type {
    template <typename T, typename = std::enable_if_t<!std::is_lvalue_reference<T>::value>>
    operator T &&() const;

    template <typename T, typename = std::enable_if_t<std::is_copy_constructible<T>::value>>
    operator T&() const;
};

namespace detail {
template <size_t N = 0>
static constexpr const wildcard_type wildcard{};
template <typename T, size_t... I>
inline constexpr auto is_brace_constructible(std::index_sequence<I...>, T*)
    -> decltype(T{wildcard<I>...}, std::true_type{})
{
    return {};
}

template <size_t... I>
inline constexpr std::false_type is_brace_constructible(std::index_sequence<I...>, ...)
{
    return {};
}

template <typename T, typename U>
struct is_paren_constructible;

template <typename T, size_t... I>
struct is_paren_constructible<T, std::index_sequence<I...>>
    : std::is_constructible<T, decltype(wildcard<I>)...> {
};
}

template <typename T, size_t N>
constexpr auto is_brace_constructible()
    -> decltype(detail::is_brace_constructible(std::make_index_sequence<N>{},
                                               static_cast<T*>(nullptr)))
{
    return {};
}

template <typename T, size_t N>
constexpr auto is_paren_constructible()
    -> detail::is_paren_constructible<T, std::make_index_sequence<N>>
{
    return {};
}

template <size_t N>
using arity_type = std::integral_constant<size_t, N>;

#define EV_MAKE_ARITY_FUNCTION(count)                                                \
    template <typename T,                                                            \
              typename = std::enable_if_t<is_brace_constructible<T, count>() &&      \
                                          !is_brace_constructible<T, count + 1>() && \
                                          !is_paren_constructible<T, count>()>>      \
    constexpr arity_type<count> arity()                                              \
    {                                                                                \
        return {};                                                                   \
    }

template <typename T, typename = std::enable_if_t<std::is_class<T>{} && std::is_empty<T>{}>>
constexpr arity_type<0> arity()
{
    return {};
}

EV_MAKE_ARITY_FUNCTION(1)
EV_MAKE_ARITY_FUNCTION(2)
EV_MAKE_ARITY_FUNCTION(3)
EV_MAKE_ARITY_FUNCTION(4)
EV_MAKE_ARITY_FUNCTION(5)
EV_MAKE_ARITY_FUNCTION(6)
EV_MAKE_ARITY_FUNCTION(7)
EV_MAKE_ARITY_FUNCTION(8)
EV_MAKE_ARITY_FUNCTION(9)
EV_MAKE_ARITY_FUNCTION(10)
EV_MAKE_ARITY_FUNCTION(11)
EV_MAKE_ARITY_FUNCTION(12)
EV_MAKE_ARITY_FUNCTION(13)
EV_MAKE_ARITY_FUNCTION(14)
EV_MAKE_ARITY_FUNCTION(15)
EV_MAKE_ARITY_FUNCTION(16)
EV_MAKE_ARITY_FUNCTION(17)
EV_MAKE_ARITY_FUNCTION(18)
EV_MAKE_ARITY_FUNCTION(19)
EV_MAKE_ARITY_FUNCTION(20)
EV_MAKE_ARITY_FUNCTION(21)
EV_MAKE_ARITY_FUNCTION(22)
EV_MAKE_ARITY_FUNCTION(23)
EV_MAKE_ARITY_FUNCTION(24)
EV_MAKE_ARITY_FUNCTION(25)

template <typename T, size_t Arity>
struct object_arity_type {
    using type = T;
    T& object;
    constexpr object_arity_type(T& obj) : object(obj)
    {
    }

    constexpr size_t arity() const
    {
        return Arity;
    }
};

namespace detail {
template <typename F,
          typename... Args,
          typename = decltype(std::declval<F&&>()(std::declval<Args&&>()...))>
constexpr auto is_valid(int)
{
    return true;
}

template <typename F, typename... Args>
constexpr auto is_valid(...)
{
    return false;
}

template <typename F>
struct is_valid_function {
    template <typename... Args>
    constexpr auto operator()(Args&&...) const
    {
        return is_valid<F, Args&&...>(int{});
    }
};
}

struct is_valid_type {
    template <typename F>
    constexpr auto operator()(F&&) const
    {
        return detail::is_valid_function<F&&>{};
    }

    template <typename F, typename... Args>
    constexpr auto operator()(F&&, Args&&...) const
    {
        return detail::is_valid<F&&, Args&&...>(int{});
    }
};

constexpr is_valid_type is_valid{};
}

#define EV_HAS_MEMBER(member) [](auto&& x) -> decltype((void)x.member) {}
