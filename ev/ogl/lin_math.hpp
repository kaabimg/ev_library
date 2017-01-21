#pragma once

#include <array>
#include <algorithm>
#include <cmath>

#define EV_OGL_VECT_ALIAS(T, ...) \
    struct alias_t                \
    {                             \
        T __VA_ARGS__;            \
    }
namespace ev
{
namespace ogl
{
namespace detail
{
struct empty_alias_t
{
};
}
template <size_t size, typename T, typename Alias = detail::empty_alias_t>
struct vec_t
{
    using iterator       = T*;
    using const_iterator = const T*;
    using data_type      = T[size];

    static constexpr bool is_empty_alias =
        std::is_same<Alias, detail::empty_alias_t>::value;

    static_assert(is_empty_alias || (sizeof(Alias) == sizeof(data_type) &&
                                     std::alignment_of<Alias>::value ==
                                         std::alignment_of<data_type>::value),
                  "Invalid alias");

    constexpr size_t dimension() const { return size; }
    vec_t() { std::fill(begin(), end(), 0); }
    vec_t(std::initializer_list<T> values)
    {
        size_t min = std::min(size, values.size());
        typename std::initializer_list<T>::const_iterator it = values.begin();
        for (size_t i = 0; i < min; ++i, ++it) {
            d[i] = *it;
        }
    }
    iterator begin() { return &(d[0]); }
    iterator end() { return begin() + size; }
    const_iterator begin() const { return &(d[0]); }
    const_iterator end() const { return begin() + size; }
    T operator[](int i) const { return d[i]; }
    T& operator[](int i) { return d[i]; }
    void fill(T val) { std::fill(begin(), end(), val); }
    ///////////////////////////

    T norm() const
    {
        T result = 0;
        for (T elem : *this) result += elem * elem;
        return std::sqrt(result);
    }

    void normalize() { *this /= norm(); }
    /////
    vec_t operator+(const vec_t& another)
    {
        vec_t res;
        for (size_t i = 0; i < dimension(); ++i) {
            res[i] = another[i] + d[i];
        }
        return res;
    }
    vec_t operator+(T val)
    {
        vec_t res;
        for (size_t i = 0; i < dimension(); ++i) {
            res[i] = val + d[i];
        }
        return res;
    }

    vec_t& operator+=(const vec_t& another)
    {
        for (size_t i = 0; i < dimension(); ++i) {
            d[i] += another[i];
        }
        return *this;
    }
    vec_t& operator+=(T val)
    {
        for (size_t i = 0; i < dimension(); ++i) {
            d[i] += val;
        }
        return *this;
    }
    /////
    vec_t operator-(const vec_t& another)
    {
        vec_t res;
        for (size_t i = 0; i < dimension(); ++i) {
            res[i] = another[i] - d[i];
        }
        return res;
    }
    vec_t operator-(T val)
    {
        vec_t res;
        for (size_t i = 0; i < dimension(); ++i) {
            res[i] = val - d[i];
        }
        return res;
    }

    vec_t& operator-=(const vec_t& another)
    {
        for (size_t i = 0; i < dimension(); ++i) {
            d[i] -= another[i];
        }
        return *this;
    }
    vec_t& operator-=(T val)
    {
        for (size_t i = 0; i < dimension(); ++i) {
            d[i] -= val;
        }
        return *this;
    }
    /////
    vec_t operator*(const vec_t& another)
    {
        vec_t res;
        for (size_t i = 0; i < dimension(); ++i) {
            res[i] = another[i] * d[i];
        }
        return res;
    }
    vec_t operator*(T val)
    {
        vec_t res;
        for (size_t i = 0; i < dimension(); ++i) {
            res[i] = val * d[i];
        }
        return res;
    }

    vec_t& operator*=(const vec_t& another)
    {
        for (size_t i = 0; i < dimension(); ++i) {
            d[i] *= another[i];
        }
        return *this;
    }
    vec_t& operator*=(T val)
    {
        for (size_t i = 0; i < dimension(); ++i) {
            d[i] *= val;
        }
        return *this;
    }

    /////
    vec_t operator/(const vec_t& another)
    {
        vec_t res;
        for (size_t i = 0; i < dimension(); ++i) {
            res[i] = another[i] / d[i];
        }
        return res;
    }
    vec_t operator/(T val)
    {
        vec_t res;
        for (size_t i = 0; i < dimension(); ++i) {
            res[i] = val / d[i];
        }
        return res;
    }

    vec_t& operator/=(const vec_t& another)
    {
        for (size_t i = 0; i < dimension(); ++i) {
            d[i] /= another[i];
        }
        return *this;
    }
    vec_t& operator/=(T val)
    {
        for (size_t i = 0; i < dimension(); ++i) {
            d[i] /= val;
        }
        return *this;
    }

    ///////////////////////////
    Alias* operator->() { return reinterpret_cast<Alias*>(this); }
    const Alias* operator->() const
    {
        return reinterpret_cast<const Alias*>(this);
    }

    data_type d;
};

template <size_t size, typename T, typename Alias>
inline T distance(const vec_t<size, T, Alias>& v1,
                  const vec_t<size, T, Alias>& v2)
{
    return (v1 - v2).norm();
}

////////////
template <typename T>
struct vec2_alias_t
{
    T x, y;
};
template <typename T>
struct vec3_alias_t
{
    T x, y, z;
};
template <typename T>
struct vec4_alias_t
{
    T x, y, z, w;
};

template <typename T>
using vec2_t = vec_t<2, T, vec2_alias_t<T>>;

template <typename T>
using vec3_t = vec_t<3, T, vec3_alias_t<T>>;

template <typename T>
using vec4_t = vec_t<4, T, vec4_alias_t<T>>;

/////////////
struct quaternion_t
{
    vec3_t<float> vec;
    float angle;
    const float* data() const { return vec.begin(); }
};

///////////////////////////////////////////

template <size_t s, typename T = float>
struct square_matrix_t
{
    static constexpr size_t size          = s;
    static constexpr size_t element_count = s * s;

    using iterator       = T*;
    using const_iterator = const T*;

    iterator begin() { return &(d[0]); }
    iterator end() { return begin() + element_count; }
    const_iterator begin() const { return &(d[0]); }
    const_iterator end() const { return begin() + element_count; }
    const vec_t<s, T>& line(size_t l) const
    {
        return *reinterpret_cast<const vec_t<s, T>*>(d + l * size);
    }

    vec_t<s, T>& line(size_t l)
    {
        return *reinterpret_cast<vec_t<s, T>*>(&(d + l * s));
    }

    const T* data() const { return d; }
    T operator()(int i, int j) const { d[i * size + j]; }
    T& operator()(int i, int j) { return d[i * size + j]; }
    void fill(T val) { std::fill(begin(), end(), val); }
    static square_matrix_t identity()
    {
        square_matrix_t m;
        m.fill(0);
        for (int i = 0; i < size; ++i) {
            m(i, i) = 1;
        }
        return m;
    }

    ///
    T d[element_count];
};

using matrix2x2_t = square_matrix_t<2>;
using matrix3x3_t = square_matrix_t<3>;
using matrix4x4_t = square_matrix_t<4>;
}
}
