#pragma once

#include <type_traits>

namespace ev {

template <typename T>
struct type_debugger;

template <typename T>
struct type {
    using inner_type = T;

    inline constexpr auto decay() const
    {
        return type<std::decay_t<T>>();
    }

    inline constexpr auto add_pointer() const
    {
        return type<std::add_pointer_t<T>>();
    }
    inline constexpr auto remove_pointer() const
    {
        return type<std::remove_pointer_t<T>>();
    }

    inline constexpr auto remove_reference() const
    {
        return type<std::remove_reference_t<T>>();
    }

    inline constexpr auto add_cv() const
    {
        return type<std::add_cv_t<T>>();
    }

    inline constexpr auto add_const() const
    {
        return type<std::add_const_t<T>>();
    }

    inline constexpr auto add_volatile() const
    {
        return type<std::add_volatile_t<T>>();
    }

    inline constexpr auto remove_cv() const
    {
        return type<std::remove_cv_t<T>>();
    }

    inline constexpr auto remove_const() const
    {
        return type<std::remove_const_t<T>>();
    }

    inline constexpr auto remove_volatile() const
    {
        return type<std::remove_volatile_t<T>>();
    }

    inline constexpr bool is_pod() const
    {
        return std::is_pod<T>::value;
    }

    inline constexpr bool is_class() const
    {
        return std::is_class<T>::value;
    }

    inline constexpr bool is_pointer() const
    {
        return std::is_pointer<T>::value;
    }

    inline constexpr bool is_reference() const
    {
        return std::is_reference<T>::value;
    }

    inline constexpr bool is_const() const
    {
        return std::is_const<T>::value;
    }

    template <typename O>
    inline constexpr bool operator==(const type<O>&) const
    {
        return std::is_same<T, O>::value;
    }

    template <typename D>
    inline constexpr bool is_base_of() const
    {
        return std::is_base_of<T, D>::value;
    }

    inline constexpr bool is_trivially_copyable() const
    {
        return std::is_trivially_copyable<T>::value;
    }
    inline constexpr bool is_trivially_default_constructible() const
    {
        return std::is_trivially_default_constructible<T>::value;
    }

    inline constexpr void debug() const
    {
        // compile time error
        type_debugger<T>();
    }
};

auto inline constexpr type_of(auto&& val)
{
    return type<decltype(val)>();
}

}
