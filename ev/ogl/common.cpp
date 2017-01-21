#include "common.hpp"
#include <boost/algorithm/string.hpp>
using namespace ev::ogl;

inline const char* erro_to_string(GLuint error)
{
}

gl_error_t::gl_error_t(GLuint error) : m_error{error}
{
    build_error_string();
}

gl_error_t::gl_error_t(const std::string& error_str, GLuint error)
    : m_error_string{error_str}, m_error{error}
{
}

#define __CASE_ERROR_CODE(code) \
    case code: error_string = #code; break

std::string gl_error_t::get_error_string(GLuint error)
{
    std::string error_string;
    switch (error)
    {
        __CASE_ERROR_CODE(GL_INVALID_ENUM);
        __CASE_ERROR_CODE(GL_INVALID_VALUE);
        __CASE_ERROR_CODE(GL_INVALID_OPERATION);
        __CASE_ERROR_CODE(GL_INVALID_FRAMEBUFFER_OPERATION);
        __CASE_ERROR_CODE(GL_OUT_OF_MEMORY);
        default: return "";
    }

    error_string.erase(error_string.begin(), error_string.begin() + 3);
    boost::algorithm::replace_all(error_string, "_", " ");
    boost::algorithm::to_lower(error_string);
    return error_string;
}

void gl_error_t::build_error_string()
{
    m_error_string = get_error_string(m_error);
}
