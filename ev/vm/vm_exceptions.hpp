#pragma once

#include <exception>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>

namespace ev {
namespace vm {

using text_position_t = boost::spirit::x3::position_tagged;

enum class error_type_e {
    syntax_error,
    variable_not_found,
    function_not_found,
    compile_error
};

struct compile_exception_t : std::exception {
    compile_exception_t(const std::string& what,
                        const text_position_t& where = text_position_t())
        : m_what(what), m_where(where) {}

    const char* what() const noexcept { return m_what.c_str(); }
    text_position_t where() const { return m_where; }

    virtual error_type_e type() const = 0;

private:
    std::string m_what;
    text_position_t m_where;
};

#define EV_VM_EXCEPTION(name, t)                           \
    name(const std::string& what,                          \
         const text_position_t& where = text_position_t()) \
        : compile_exception_t(what, where) {}              \
    error_type_e type() const { return t; }

struct syntax_error_t : compile_exception_t {
    EV_VM_EXCEPTION(syntax_error_t, error_type_e::syntax_error)
};

struct compile_error_t : compile_exception_t {
    EV_VM_EXCEPTION(compile_error_t, error_type_e::compile_error)
};
}
}
