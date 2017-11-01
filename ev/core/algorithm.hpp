#pragma once

#include <tuple>
#include <type_traits>

namespace ev {

// clang-format off
namespace detail {
template <size_t index, size_t max>
constexpr void for_each_tuple(auto&& t, auto&& f)
{
    f(std::get<index>(std::forward<decltype(t)>(t)));
    if constexpr(index + 1 < max)
        detail::for_each_tuple<index + 1, max>(
            std::forward<decltype(t)>(t),
            std::forward<decltype(f)>(f)
        );
}
}

template<typename ...Ts,typename F>
inline constexpr void for_each(std::tuple<Ts...>& tuple, F&& f)
{
    constexpr size_t tuple_size =
            std::tuple_size<typename std::decay<std::tuple<Ts...>>::type>::value;
    if constexpr(tuple_size > 0)
    {
        detail::for_each_tuple<0,tuple_size>(
            tuple, std::forward<F>(f));
    }
}

template<typename ...Ts,typename F>
inline constexpr void for_each(const std::tuple<Ts...>& tuple, F&& f)
{
    constexpr size_t tuple_size =
            std::tuple_size<typename std::decay<std::tuple<Ts...>>::type>::value;
    if constexpr(tuple_size > 0)
    {
        detail::for_each_tuple<0,tuple_size>(
            tuple, std::forward<F>(f));
    }
}

template<typename ...Ts,typename F>
inline constexpr void for_each(std::tuple<Ts...>&& tuple, F&& f)
{
    constexpr size_t tuple_size =
            std::tuple_size<typename std::decay<std::tuple<Ts...>>::type>::value;
    if constexpr(tuple_size > 0)
    {
        detail::for_each_tuple<0,tuple_size>(
            std::move(tuple), std::forward<F>(f));
    }
}
// clang-format on
}
