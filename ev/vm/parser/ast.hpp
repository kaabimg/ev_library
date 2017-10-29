#pragma once

#include <string>
#include <vector>

#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>

namespace ev
{
namespace vm
{
namespace ast
{
namespace x3 = boost::spirit::x3;

struct unary;
struct expression;
struct identifier;
struct function_call;

struct identifier : x3::position_tagged
{
    identifier(const std::string& name = "") : value(name) {}
    std::string value;
};

struct variable : identifier
{
    variable(const std::string& name = "") : identifier(name) {}
};

enum class operand_type
{
    number,
    variable,
    unary_expression,
    expression,
    function_call,
};

enum class number_type
{
    i32,
    i64,
    r32,
    r64
};

struct number : x3::position_tagged
{
    boost::variant<int32_t, int64_t, float, double> value;

    template <typename T>
    T get() const
    {
        return boost::get<T>(value);
    }

    number_type type() const
    {
        return static_cast<number_type>(value.which());
    }
};

struct operand : x3::variant<number,
                               variable,
                               x3::forward_ast<unary>,
                               x3::forward_ast<expression>,
                               x3::forward_ast<function_call> >
{
    using base_type::base_type;
    using base_type::operator=;

    operand_type type() const
    {
        return static_cast<operand_type>(get().which());
    }
    template <typename T>
    T& as()
    {
        return boost::get<T>(get());
    }
    template <typename T>
    const T& as() const
    {
        return boost::get<T>(get());
    }
};

enum class operator_type
{
    plus,
    minus,
    times,
    divide,
    positive,
    negative,
};

struct unary : x3::position_tagged
{
    operator_type op_type;
    operand op;
};

struct operation : x3::position_tagged
{
    operator_type op_type;
    operand op;
};

struct expression : x3::position_tagged
{
    operand first;
    std::vector<operation> rest;
};

struct function_call : x3::position_tagged
{
    identifier name;
    std::vector<expression> arguments;
};

struct variable_declaration : x3::position_tagged
{
    identifier type_name;
    identifier variable_name;
};

struct anonymous_function_declaration : x3::position_tagged
{
    std::vector<variable_declaration> arguments;
    identifier return_type;
    expression expr;
};

struct function_declaration : x3::position_tagged
{
    identifier name;
    std::vector<variable_declaration> arguments;
    identifier return_type;
    expression expr;
};

struct structure
{
    identifier name;
    std::vector<variable_declaration> fields;
};

enum class statement_type
{
    function_declaration,
    anonymous_function_declaration,
    expression
};

struct statement : x3::variant<function_declaration,
                                 anonymous_function_declaration,
                                 expression>
{
    using base_type::base_type;
    using base_type::operator=;

    statement_type type() const
    {
        return static_cast<statement_type>(get().which());
    }

    template <typename T>
    T& as()
    {
        return boost::get<T>(get());
    }
    template <typename T>
    const T& as() const
    {
        return boost::get<T>(get());
    }
};
}
}
}
