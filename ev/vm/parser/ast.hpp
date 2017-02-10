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

struct unary_t;
struct expression_t;
struct identifier_t;
struct function_call_t;

struct identifier_t : x3::position_tagged
{
    identifier_t(const std::string& name = "") : value(name) {}
    std::string value;
};

struct variable_t : identifier_t
{
    variable_t(const std::string& name = "") : identifier_t(name) {}
};

enum class operand_type_e
{
    number,
    variable,
    unary_expression,
    expression,
    function_call,
};

enum class number_type_e
{
    i32,
    i64,
    r32,
    r64
};

struct number_t : x3::position_tagged
{
    boost::variant<int32_t, int64_t, float, double> value;

    template <typename T>
    T get() const
    {
        return boost::get<T>(value);
    }

    number_type_e type() const
    {
        return static_cast<number_type_e>(value.which());
    }
};

struct operand_t : x3::variant<number_t,
                               variable_t,
                               x3::forward_ast<unary_t>,
                               x3::forward_ast<expression_t>,
                               x3::forward_ast<function_call_t> >
{
    using base_type::base_type;
    using base_type::operator=;

    operand_type_e type() const
    {
        return static_cast<operand_type_e>(get().which());
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

enum class operator_type_e
{
    plus,
    minus,
    times,
    divide,
    positive,
    negative,
};

struct unary_t : x3::position_tagged
{
    operator_type_e op;
    operand_t operand;
};

struct operation_t : x3::position_tagged
{
    operator_type_e op;
    operand_t operand;
};

struct expression_t : x3::position_tagged
{
    operand_t first;
    std::vector<operation_t> rest;
};

struct function_call_t : x3::position_tagged
{
    identifier_t name;
    std::vector<expression_t> arguments;
};

struct variable_declaration_t : x3::position_tagged
{
    identifier_t type_name;
    identifier_t variable_name;
};

struct anonymous_function_declaration_t : x3::position_tagged
{
    std::vector<variable_declaration_t> arguments;
    identifier_t return_type;
    expression_t expression;
};

struct function_declaration_t : x3::position_tagged
{
    identifier_t name;
    std::vector<variable_declaration_t> arguments;
    identifier_t return_type;
    expression_t expression;
};

struct struct_t
{
    identifier_t name;
    std::vector<variable_declaration_t> fields;
};

enum class statement_type_e
{
    function_declaration,
    anonymous_function_declaration,
    expression
};

struct statement_t : x3::variant<function_declaration_t,
                                 anonymous_function_declaration_t,
                                 expression_t>
{
    using base_type::base_type;
    using base_type::operator=;

    statement_type_e type() const
    {
        return static_cast<statement_type_e>(get().which());
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
