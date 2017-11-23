#pragma once

#include "traits.hpp"

#include <algorithm>

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


template <size_t index, size_t max>
constexpr void for_each_tuple2(auto&& t, auto&& f)
{
    f(index,std::get<index>(std::forward<decltype(t)>(t)));
    if constexpr(index + 1 < max)
        detail::for_each_tuple2<index + 1, max>(
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


template<typename ...Ts,typename F>
inline constexpr void for_each2(const std::tuple<Ts...>& tuple, F&& f)
{
    constexpr size_t tuple_size =
            std::tuple_size<typename std::decay<std::tuple<Ts...>>::type>::value;
    if constexpr(tuple_size > 0)
    {
        detail::for_each_tuple2<0,tuple_size>(
            tuple, std::forward<F>(f));
    }
}

template<typename ...Ts,typename F>
inline constexpr void for_each2(std::tuple<Ts...>&& tuple, F&& f)
{
    constexpr size_t tuple_size =
            std::tuple_size<typename std::decay<std::tuple<Ts...>>::type>::value;
    if constexpr(tuple_size > 0)
    {
        detail::for_each_tuple2<0,tuple_size>(
            std::move(tuple), std::forward<F>(f));
    }
}

// clang-format on
}

namespace ev {

template <typename T>
inline constexpr auto members_of(T& obj)
{
    return object_arity_type<T, arity<T>()>(obj);
}

template <typename T>
inline constexpr auto members_of(T&& obj)
{
    return object_arity_type<T, arity<T>()>(obj);
}

// clang-format off
template <typename T, size_t Arity>
inline void for_each(object_arity_type<T, Arity>& obj, auto&& f)
{
    static_assert(Arity <= 10, "Extend me");

    if constexpr (Arity == 1) {
        auto & [m0] = obj.object;
        f(m0);
    }
    else if constexpr (Arity == 2) {
        auto & [ m0, m1 ] = obj.object;
        f(m0);
        f(m1);
    }
    else if constexpr (Arity == 3) {
        auto & [ m0, m1, m2 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
    }
    else if constexpr (Arity == 4) {
        auto & [ m0, m1, m2, m3 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
        f(m3);
    }
    else if constexpr (Arity == 5) {
        auto & [ m0, m1, m2, m3, m4 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
        f(m3);
        f(m4);
    }
    else if constexpr (Arity == 6) {
        auto & [ m0, m1, m2, m3, m4, m5 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
        f(m3);
        f(m4);
        f(m5);
    }
    else if constexpr (Arity == 7) {
        auto & [ m0, m1, m2, m3, m4, m5, m6 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
        f(m3);
        f(m4);
        f(m5);
        f(m6);
    }
    else if constexpr (Arity == 8) {
        auto & [ m0, m1, m2, m3, m4, m5, m6, m7 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
        f(m3);
        f(m4);
        f(m5);
        f(m6);
        f(m7);
    }
    else if constexpr (Arity == 9) {
        auto & [ m0, m1, m2, m3, m4, m5, m6, m7, m8 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
        f(m3);
        f(m4);
        f(m5);
        f(m6);
        f(m7);
        f(m8);
    }
    else if constexpr (Arity == 10) {
        auto & [ m0, m1, m2, m3, m4, m5, m6, m7, m8, m9 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
        f(m3);
        f(m4);
        f(m5);
        f(m6);
        f(m7);
        f(m8);
        f(m9);
    }
}
template <typename T, size_t Arity>
inline void for_each(object_arity_type<T, Arity>&& obj, auto&& f)
{
    static_assert(Arity <= 10, "Extend me");

    if constexpr (Arity == 1) {
        auto & [m0] = obj.object;
        f(m0);
    }
    else if constexpr (Arity == 2) {
        auto & [ m0, m1 ] = obj.object;
        f(m0);
        f(m1);
    }
    else if constexpr (Arity == 3) {
        auto & [ m0, m1, m2 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
    }
    else if constexpr (Arity == 4) {
        auto & [ m0, m1, m2, m3 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
        f(m3);
    }
    else if constexpr (Arity == 5) {
        auto & [ m0, m1, m2, m3, m4 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
        f(m3);
        f(m4);
    }
    else if constexpr (Arity == 6) {
        auto & [ m0, m1, m2, m3, m4, m5 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
        f(m3);
        f(m4);
        f(m5);
    }
    else if constexpr (Arity == 7) {
        auto & [ m0, m1, m2, m3, m4, m5, m6 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
        f(m3);
        f(m4);
        f(m5);
        f(m6);
    }
    else if constexpr (Arity == 8) {
        auto & [ m0, m1, m2, m3, m4, m5, m6, m7 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
        f(m3);
        f(m4);
        f(m5);
        f(m6);
        f(m7);
    }
    else if constexpr (Arity == 9) {
        auto & [ m0, m1, m2, m3, m4, m5, m6, m7, m8 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
        f(m3);
        f(m4);
        f(m5);
        f(m6);
        f(m7);
        f(m8);
    }
    else if constexpr (Arity == 10) {
        auto & [ m0, m1, m2, m3, m4, m5, m6, m7, m8, m9 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
        f(m3);
        f(m4);
        f(m5);
        f(m6);
        f(m7);
        f(m8);
        f(m9);
    }
}
template <typename T, size_t Arity>
inline void for_each2(object_arity_type<T, Arity>& obj, auto&& f)
{
    static_assert(Arity <= 10, "Extend me");

    if constexpr (Arity == 1) {
        auto & [m0] = obj.object;
        f(0,m0);
    }
    else if constexpr (Arity == 2) {
        auto & [ m0, m1 ] = obj.object;
        f(0,m0);
        f(1,m1);
    }
    else if constexpr (Arity == 3) {
        auto & [ m0, m1, m2 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
    }
    else if constexpr (Arity == 4) {
        auto & [ m0, m1, m2, m3 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
        f(3,m3);
    }
    else if constexpr (Arity == 5) {
        auto & [ m0, m1, m2, m3, m4 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
        f(3,m3);
        f(4,m4);
    }
    else if constexpr (Arity == 6) {
        auto & [ m0, m1, m2, m3, m4, m5 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
        f(3,m3);
        f(4,m4);
        f(5,m5);
    }
    else if constexpr (Arity == 7) {
        auto & [ m0, m1, m2, m3, m4, m5, m6 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
        f(3,m3);
        f(4,m4);
        f(5,m5);
        f(6,m6);
    }
    else if constexpr (Arity == 8) {
        auto & [ m0, m1, m2, m3, m4, m5, m6, m7 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
        f(3,m3);
        f(4,m4);
        f(5,m5);
        f(6,m6);
        f(7,m7);
    }
    else if constexpr (Arity == 9) {
        auto & [ m0, m1, m2, m3, m4, m5, m6, m7, m8 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
        f(3,m3);
        f(4,m4);
        f(5,m5);
        f(6,m6);
        f(7,m7);
        f(8,m8);
    }
    else if constexpr (Arity == 10) {
        auto & [ m0, m1, m2, m3, m4, m5, m6, m7, m8, m9 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
        f(3,m3);
        f(4,m4);
        f(5,m5);
        f(6,m6);
        f(7,m7);
        f(8,m8);
        f(9,m9);
    }
}
template <typename T, size_t Arity>
inline void for_each2(object_arity_type<T, Arity>&& obj, auto&& f)
{
    static_assert(Arity <= 10, "Extend me");

    if constexpr (Arity == 1) {
        auto & [m0] = obj.object;
        f(0,m0);
    }
    else if constexpr (Arity == 2) {
        auto & [ m0, m1 ] = obj.object;
        f(0,m0);
        f(1,m1);
    }
    else if constexpr (Arity == 3) {
        auto & [ m0, m1, m2 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
    }
    else if constexpr (Arity == 4) {
        auto & [ m0, m1, m2, m3 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
        f(3,m3);
    }
    else if constexpr (Arity == 5) {
        auto & [ m0, m1, m2, m3, m4 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
        f(3,m3);
        f(4,m4);
    }
    else if constexpr (Arity == 6) {
        auto & [ m0, m1, m2, m3, m4, m5 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
        f(3,m3);
        f(4,m4);
        f(5,m5);
    }
    else if constexpr (Arity == 7) {
        auto & [ m0, m1, m2, m3, m4, m5, m6 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
        f(3,m3);
        f(4,m4);
        f(5,m5);
        f(6,m6);
    }
    else if constexpr (Arity == 8) {
        auto & [ m0, m1, m2, m3, m4, m5, m6, m7 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
        f(3,m3);
        f(4,m4);
        f(5,m5);
        f(6,m6);
        f(7,m7);
    }
    else if constexpr (Arity == 9) {
        auto & [ m0, m1, m2, m3, m4, m5, m6, m7, m8 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
        f(3,m3);
        f(4,m4);
        f(5,m5);
        f(6,m6);
        f(7,m7);
        f(8,m8);
    }
    else if constexpr (Arity == 10) {
        auto & [ m0, m1, m2, m3, m4, m5, m6, m7, m8, m9 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
        f(3,m3);
        f(4,m4);
        f(5,m5);
        f(6,m6);
        f(7,m7);
        f(8,m8);
        f(9,m9);
    }
}
// clang-format on
}
