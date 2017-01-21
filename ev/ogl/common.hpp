#pragma once

#include <ev/core/logging_helpers.hpp>
#include <ev/core/basic_types.hpp>

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <string>

namespace ev
{
namespace ogl
{
class gl_error_t : public std::exception
{
public:
    gl_error_t(GLuint error);
    gl_error_t(const std::string& error_str, GLuint error = 0);

    ~gl_error_t() noexcept {}
    const char* what() const noexcept { return m_error_string.c_str(); }
    GLuint error() const { return m_error; }
    static std::string get_error_string(GLuint error);

private:
    void build_error_string();

private:
    GLuint m_error = 0;
    std::string m_error_string;
};

inline void check_gl_error(const char* file, int line)
{
    GLuint error = glGetError();
    if (error != GL_NO_ERROR) {
        std::string error_str = gl_error_t::get_error_string(error);
        error_str.append("\n");
        error_str.append("Location : ");
        error_str.append(file);
        error_str.append("\n");
        error_str.append("Line : ");
        error_str.append(std::to_string(line));
        throw gl_error_t{error_str};
    }
}

using object_id_t = GLuint;

template <typename T>
inline void dummy_clear_function(T)
{
}

template <typename T, void (*clear_function)(T)>
class gl_object_wrapper_t : non_copyable_t
{
public:
    using wrapper_type = gl_object_wrapper_t<T, clear_function>;

    gl_object_wrapper_t(T id = 0) : m_id{id} {}
    ~gl_object_wrapper_t()
    {
        if (m_id) clear_function(m_id);
    }
    T gl_object() const { return m_id; }
    operator bool() const { return m_id != 0; }
protected:
    T m_id = 0;
};

class color_t
{
public:
    color_t() = default;
    color_t(float r, float g, float b, float a = 0)
        : m_r{r}, m_g{g}, m_b{b}, m_a{a}
    {
    }

    float red() const { return m_r; }
    float green() const { return m_g; }
    float blue() const { return m_b; }
    float alpha() const { return m_a; }
    void set_red(float r) { m_r = r; }
    void set_green(float g) { m_g = g; }
    void set_blue(float b) { m_b = b; }
    void set_alpha(float a) { m_a = a; }
    const float* data() const { return &m_a; }
private:
    float m_r = 0;
    float m_g = 0;
    float m_b = 0;
    float m_a = 0;
};

struct size_2d_t
{
    int32_t width  = 0;
    int32_t height = 0;

    bool operator==(const size_2d_t& another)
    {
        return width == another.width && height == another.height;
    }

    bool operator!=(const size_2d_t& another)
    {
        return width != another.width || height != another.height;
    }
};

struct position_t
{
    int32_t x = 0;
    int32_t y = 0;
};
}
}

#define EV_OGL_CALL(code) \
    code;                 \
    ev::ogl::check_gl_error(__FILE__, __LINE__)

EV_CUSTOM_PRINTER(ev::ogl::size_2d_t, os, data)
{
    os << "size_2d_t(" << data.width << "," << data.height << ")";
}
