#pragma once

#include "ogl.hpp"
#include <ev/core/logging_helpers.hpp>

namespace ev
{
template <typename T>
struct print_impl_t<ogl::vec2_t<T>>
{
    static inline void print(std::ostream& out, const ogl::vec2_t<T>& val)
    {
        out << "vec_2 [ " << val->x << " , " << val->y << " ]";
    }
};

template <typename T>
struct print_impl_t<ogl::vec3_t<T>>
{
    static inline void print(std::ostream& out, const ogl::vec3_t<T>& val)
    {
        out << "vec_3 [ " << val->x << " , " << val->y << " , " << val->z
            << " ]";
    }
};

template <typename T>
struct print_impl_t<ogl::vec4_t<T>>
{
    static inline void print(std::ostream& out, const ogl::vec4_t<T>& val)
    {
        out << "vec_4 [ " << val->x << " , " << val->y << " , " << val->z
            << " , " << val->w << " ]";
    }
};

template <size_t size, typename T>
struct print_impl_t<ogl::square_matrix_t<size, T>>
{
    static inline void print(std::ostream& out,
                             const ogl::square_matrix_t<size, T>& val)
    {
        out << "matrix_" << size << "x" << size << "_t {\n";
        for (size_t i = 0; i < size; ++i) {
            print_impl_t<ogl::vec_t<size, T>>::print(out, val.line(i));
            out << '\n';
        }
        out << "}";
    }
};
}
