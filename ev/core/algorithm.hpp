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

#define EV_MAKE_ARITY_FUNCTION(count)                                                            \
    template <typename T, typename = std::enable_if_t<is_brace_constructible<T, count>() &&      \
                                                      !is_brace_constructible<T, count + 1>() && \
                                                      !is_paren_constructible<T, count>()>>      \
    constexpr arity_type<count> arity()                                                          \
    {                                                                                            \
        return {};                                                                               \
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

template <typename T, size_t count>
struct object_member_count_type {
    using type = T;
    T& object;
    constexpr object_member_count_type(T& obj) : object(obj)
    {
    }

    constexpr size_t size() const
    {
        return count;
    }
};

template <typename T>
inline constexpr auto members_of(T& obj)
{
    return object_member_count_type<T, arity<T>()>(obj);
}

// clang-format off
template <typename T, size_t count>
inline void for_each(object_member_count_type<T, count>& obj, auto&& f)
{
    if constexpr (count == 1) {
        auto & [m0] = obj.object;
        f(m0);
    }
    else if constexpr (count == 2) {
        auto & [ m0, m1 ] = obj.object;
        f(m0);
        f(m1);
    }
    else if constexpr (count == 3) {
        auto & [ m0, m1, m2 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
    }
    else if constexpr (count == 4) {
        auto & [ m0, m1, m2, m3 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
        f(m3);
    }
    else if constexpr (count == 5) {
        auto & [ m0, m1, m2, m3, m4 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
        f(m3);
        f(m4);
    }
    else if constexpr (count == 6) {
        auto & [ m0, m1, m2, m3, m4, m5 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
        f(m3);
        f(m4);
        f(m5);
    }
    else if constexpr (count == 7) {
        auto & [ m0, m1, m2, m3, m4, m5, m6 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
        f(m3);
        f(m4);
        f(m5);
        f(m6);
    }
    else if constexpr (count == 8) {
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
    else if constexpr (count == 9) {
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
    else if constexpr (count == 10) {
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
template <typename T, size_t count>
inline void for_each(object_member_count_type<T, count>&& obj, auto&& f)
{
    if constexpr (count == 1) {
        auto & [m0] = obj.object;
        f(m0);
    }
    else if constexpr (count == 2) {
        auto & [ m0, m1 ] = obj.object;
        f(m0);
        f(m1);
    }
    else if constexpr (count == 3) {
        auto & [ m0, m1, m2 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
    }
    else if constexpr (count == 4) {
        auto & [ m0, m1, m2, m3 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
        f(m3);
    }
    else if constexpr (count == 5) {
        auto & [ m0, m1, m2, m3, m4 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
        f(m3);
        f(m4);
    }
    else if constexpr (count == 6) {
        auto & [ m0, m1, m2, m3, m4, m5 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
        f(m3);
        f(m4);
        f(m5);
    }
    else if constexpr (count == 7) {
        auto & [ m0, m1, m2, m3, m4, m5, m6 ] = obj.object;
        f(m0);
        f(m1);
        f(m2);
        f(m3);
        f(m4);
        f(m5);
        f(m6);
    }
    else if constexpr (count == 8) {
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
    else if constexpr (count == 9) {
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
    else if constexpr (count == 10) {
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
template <typename T, size_t count>
inline void for_each2(object_member_count_type<T, count>& obj, auto&& f)
{
    if constexpr (count == 1) {
        auto & [m0] = obj.object;
        f(0,m0);
    }
    else if constexpr (count == 2) {
        auto & [ m0, m1 ] = obj.object;
        f(0,m0);
        f(1,m1);
    }
    else if constexpr (count == 3) {
        auto & [ m0, m1, m2 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
    }
    else if constexpr (count == 4) {
        auto & [ m0, m1, m2, m3 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
        f(3,m3);
    }
    else if constexpr (count == 5) {
        auto & [ m0, m1, m2, m3, m4 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
        f(3,m3);
        f(4,m4);
    }
    else if constexpr (count == 6) {
        auto & [ m0, m1, m2, m3, m4, m5 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
        f(3,m3);
        f(4,m4);
        f(5,m5);
    }
    else if constexpr (count == 7) {
        auto & [ m0, m1, m2, m3, m4, m5, m6 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
        f(3,m3);
        f(4,m4);
        f(5,m5);
        f(6,m6);
    }
    else if constexpr (count == 8) {
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
    else if constexpr (count == 9) {
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
    else if constexpr (count == 10) {
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
template <typename T, size_t count>
inline void for_each2(object_member_count_type<T, count>&& obj, auto&& f)
{
    if constexpr (count == 1) {
        auto & [m0] = obj.object;
        f(0,m0);
    }
    else if constexpr (count == 2) {
        auto & [ m0, m1 ] = obj.object;
        f(0,m0);
        f(1,m1);
    }
    else if constexpr (count == 3) {
        auto & [ m0, m1, m2 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
    }
    else if constexpr (count == 4) {
        auto & [ m0, m1, m2, m3 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
        f(3,m3);
    }
    else if constexpr (count == 5) {
        auto & [ m0, m1, m2, m3, m4 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
        f(3,m3);
        f(4,m4);
    }
    else if constexpr (count == 6) {
        auto & [ m0, m1, m2, m3, m4, m5 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
        f(3,m3);
        f(4,m4);
        f(5,m5);
    }
    else if constexpr (count == 7) {
        auto & [ m0, m1, m2, m3, m4, m5, m6 ] = obj.object;
        f(0,m0);
        f(1,m1);
        f(2,m2);
        f(3,m3);
        f(4,m4);
        f(5,m5);
        f(6,m6);
    }
    else if constexpr (count == 8) {
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
    else if constexpr (count == 9) {
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
    else if constexpr (count == 10) {
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
