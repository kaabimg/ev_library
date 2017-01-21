#include "shader.hpp"
#include <ev/core/file_utils.hpp>
#include <ev/core/scope_exit.hpp>

using namespace ev::ogl;

shader_t::shader_t(object_id_t id) : wrapper_type{id}
{
}

shader_t::shader_t(shader_t&& rhs)
{
    std::swap(m_id, rhs.m_id);
}

shader_t& shader_t::operator=(shader_t&& rhs)
{
    std::swap(m_id, rhs.m_id);
    return *this;
}

////

program_t::program_t()
{
    m_id = glCreateProgram();
    if (!m_id) throw gl_error_t("Failed to create program", glGetError());
}

program_t::program_t(object_id_t id) : wrapper_type{id}
{
}

program_t::program_t(program_t&& rhs)
{
    std::swap(m_id, rhs.m_id);
}

void program_t::create_shader(shader_type_e type, const std::string& code)
{
    object_id_t shader;
    GLint compiled;
    shader = glCreateShader((GLenum)type);
    if (shader == 0) throw gl_error_t("Failed to create shader");

    on_scope_exit_with_exception { glDeleteShader(shader); };
    const char* src = code.c_str();

    EV_OGL_CALL(glShaderSource(shader, 1, &src, NULL));
    EV_OGL_CALL(glCompileShader(shader));
    EV_OGL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled));

    if (!compiled) {
        GLint len = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        char error_str[len];

        if (len > 1) {
            glGetShaderInfoLog(shader, len, NULL, error_str);
        }
        gl_error_t error{
            std::string{(type == shader_type_e::vertex
                             ? "Failed to compile vertex shader : "
                             : "Failed to compile fragment shader : ")} +
            error_str};
        throw error;
    }

    EV_OGL_CALL(glAttachShader(m_id, shader));

    if (type == shader_type_e::vertex)
        m_vertex_shader = shader_t(shader);
    else
        m_fragment_shader = shader_t(shader);
}

void program_t::create_shader_from_file(shader_type_e type,
                                        const std::string& path)
{
    create_shader(type, load_text_file(path));
}

std::vector<std::string> program_t::get_uniform_names() const
{
    std::vector<std::string> names;
    int total = 0;
    EV_OGL_CALL(glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &total));
    for (int i = 0; i < total; ++i) {
        int name_len = -1, num = -1;
        GLenum type = GL_ZERO;
        char name[100];
        EV_OGL_CALL(glGetActiveUniform(m_id, GLuint(i), sizeof(name) - 1,
                                       &name_len, &num, &type, name));
        name[name_len] = 0;
        names.push_back(name);
    }
    return names;
}

std::vector<std::string> program_t::get_attribute_names() const
{
    std::vector<std::string> names;
    int total = 0;
    EV_OGL_CALL(glGetProgramiv(m_id, GL_ACTIVE_ATTRIBUTES, &total));
    for (int i = 0; i < total; ++i) {
        int name_len = -1, num = -1;
        GLenum type = GL_ZERO;
        char name[100];
        EV_OGL_CALL(glGetActiveAttrib(m_id, GLuint(i), sizeof(name) - 1,
                                      &name_len, &num, &type, name));
        name[name_len] = 0;
        names.push_back(name);
    }
    return names;
}

int program_t::get_uniform_location(const char* param) const
{
    return glGetUniformLocation(m_id, param);
}

void program_t::set_uniform(const char* name, const attribute_value_t&)
{
}

int program_t::get_attribute_location(const char* param) const
{
    return glGetAttribLocation(m_id, param);
}

void program_t::bind_attribute(int index, const char* name)
{
    EV_OGL_CALL(glBindAttribLocation(m_id, index, name));
}

void program_t::set_attribute(const char* name, const attribute_value_t&)
{
}

void program_t::link()
{
    glLinkProgram(m_id);
    GLint linked = 0;
    glGetProgramiv(m_id, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint len = 0;
        glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &len);
        char error_str[len];
        if (len > 1) {
            glGetProgramInfoLog(m_id, len, nullptr, error_str);
        }
        throw gl_error_t{std::string{"Failed to link program : "} + error_str};
    }
}
