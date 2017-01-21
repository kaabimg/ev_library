#pragma once

#include "common.hpp"
#include "lin_math.hpp"

#include <boost/variant.hpp>

#include <string>

namespace ev
{
namespace ogl
{
enum class shader_type_e
{
    vertex   = GL_VERTEX_SHADER,
    fragment = GL_FRAGMENT_SHADER
};

class shader_t : public gl_object_wrapper_t<object_id_t, glDeleteShader>
{
public:
    shader_t(object_id_t id);
    shader_t(shader_t&&);
    shader_t& operator=(shader_t&&);
};

enum class attribute_type
{
    f           = GL_FLOAT,
    f2          = GL_FLOAT_VEC2,
    f3          = GL_FLOAT_VEC3,
    f4          = GL_FLOAT_VEC4,
    quaternion  = GL_FLOAT_VEC4,
    matrix4x4_t = GL_FLOAT_MAT4,
    color       = GL_FLOAT_VEC4

};

using attribute_value_t = boost::variant<float,
                                         vec2_t<float>,
                                         vec3_t<float>,
                                         vec3_t<float>,
                                         vec4_t<float>,
                                         quaternion_t,
                                         matrix4x4_t,
                                         color_t>;

class program_t : public gl_object_wrapper_t<object_id_t, glDeleteProgram>
{
public:
    program_t();
    program_t(object_id_t id);
    program_t(program_t&&);

    void create_shader(shader_type_e type, const std::string& code);

    void create_shader_from_file(shader_type_e type, const std::string& path);

    std::vector<std::string> get_uniform_names() const;
    int get_uniform_location(const char* param) const;
    void set_uniform(const char* name, const attribute_value_t&);

    std::vector<std::string> get_attribute_names() const;
    int get_attribute_location(const char* param) const;
    void bind_attribute(int index, const char* name);
    void set_attribute(const char* name, const attribute_value_t&);

    void link();

private:
    shader_t m_vertex_shader{0};
    shader_t m_fragment_shader{0};
};
}
}

EV_CUSTOM_PRINTER(ev::ogl::shader_type_e, ostream, data)
{
    if (data == ev::ogl::shader_type_e::vertex)
        ostream << "vertex";
    else
        ostream << "fragment";
}
