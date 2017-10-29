#include "compiler.hpp"

#include <iostream>
#include <sstream>

#include <ev/core/logging.hpp>
#include <ev/core/scope_guard.hpp>

#include "../parser/ast.hpp"
#include "../jit/context.hpp"
#include "../jit/function.hpp"
#include "../jit/module.hpp"
#include "../jit/type.hpp"

#include "../vm_exceptions.hpp"

using namespace ev::vm;

namespace x3 = boost::spirit::x3;

jit::function compiler_t::compile(const ast::statement& statement)
{
    switch (statement.type())
    {
        case ast::statement_type::expression:
        {
            return create_top_level_expression_function(
                statement.as<ast::expression>());
            break;
        }
        case ast::statement_type::function_declaration:
        {
            return build(statement.as<ast::function_declaration>());
            break;
        }

        default: break;
    }

    throw compile_error_t("unknown statement type");
}

jit::value compiler_t::build(bool v)
{
    return m_context.new_constant<bool>(v);
}

jit::value compiler_t::build(int32_t v)
{
    return m_context.new_constant<int32_t>(v);
}

jit::value compiler_t::build(int64_t v)
{
    return m_context.new_constant<int64_t>(v);
}

jit::value compiler_t::build(float v)
{
    return m_context.new_constant<float>(v);
}

jit::value compiler_t::build(double v)
{
    return m_context.new_constant<double>(v);
}

jit::value compiler_t::build(const ast::number& n)
{
    switch (n.type())
    {
        case ast::number_type::i32: return build(n.get<int32_t>());
        case ast::number_type::i64: return build(n.get<int64_t>());
        case ast::number_type::r32: return build(n.get<float>());
        case ast::number_type::r64: return build(n.get<double>());
        default: throw compile_error_t("Unknown number type", n); break;
    }
}

jit::value compiler_t::build(const ast::expression& expression)
{
    jit::value lhs = build(expression.first);

    if (expression.rest.empty()) {
        return lhs;
    }

    for (const ast::operation& operation : expression.rest) {
        jit::value rhs = build(operation.op);

        if (!lhs.is_number() || !rhs.is_number()) {
            throw compile_error_t("Not a number", expression);
        }

        switch (operation.op_type)
        {
            case ast::operator_type::plus: lhs   = lhs + rhs; break;
            case ast::operator_type::minus: lhs  = lhs - rhs; break;
            case ast::operator_type::times: lhs  = lhs * rhs; break;
            case ast::operator_type::divide: lhs = lhs / rhs; break;
            default: break;
        }
    }
    return lhs;
}

jit::value compiler_t::build(const ast::operand& operand)
{
    switch (operand.type())
    {
        case ast::operand_type::number:
            return build(operand.as<ast::number>());
        case ast::operand_type::variable:
        {
            jit::value var = m_context.main_module().find_variable(
                operand.as<ast::variable>().value);
            if (var) return var;
            throw compile_error_t("Variable " +
                                      operand.as<ast::variable>().value +
                                      " not found",
                                  operand.as<ast::variable>());
        }

        case ast::operand_type::unary_expression:
            return build(operand.as<x3::forward_ast<ast::unary>>().get());

        case ast::operand_type::expression:
            return build(
                operand.as<x3::forward_ast<ast::expression>>().get());

        case ast::operand_type::function_call:
            return build(
                operand.as<x3::forward_ast<ast::function_call>>().get());

        default: throw compile_error_t("unknown operand");
    }
}

jit::value compiler_t::build(const ast::unary& expression)
{
    jit::value rhs = build(expression.op);
    switch (expression.op_type)
    {
        case ast::operator_type::positive: return rhs;
        case ast::operator_type::negative: return -rhs;
        default: throw compile_error_t("Unknown unary oprator", expression);
    }
}

jit::value compiler_t::build(const ast::function_call& func_call)
{
    jit::module current_module = m_context.main_module();

    std::vector<jit::value_data> args(func_call.arguments.size());

    jit::function func =
        current_module.find_function(func_call.name.value, args.size());

    if (!func) {
        throw compile_error_t("function " + func_call.name.value + " not found",
                              func_call);
    }

    size_t i = 0;
    for (const ast::expression& expression : func_call.arguments) {
        args[i] = build(expression).cast_to(func.arg_type_at(i)).data();
        ++i;
    }

    return m_context.main_module().new_call(func, std::move(args));
}

jit::function compiler_t::build(
    const ast::function_declaration& function_dec)
{
    jit::module current_module = m_context.main_module();  // TODO

    jit::function_creation_info info;
    info.name        = function_dec.name.value;
    info.return_type = function_dec.return_type.value;
    info.arg_types.resize(function_dec.arguments.size());

    int i = 0;
    for (const ast::variable_declaration& arg : function_dec.arguments) {
        info.arg_types[i++] = arg.type_name.value;
    }

    info.arg_names.resize(function_dec.arguments.size());

    i = 0;
    for (auto& arg : function_dec.arguments) {
        info.arg_names[i++] = arg.variable_name.value;
    }

    if (m_context.main_module().find_function(info)) {
        throw compile_error_t("Function " + function_dec.name.value + " : " +
                              info.to_string() + " already exists");
    }

    jit::function function = m_context.main_module().new_function(info);

    on_scope(success)
    {
        m_context.main_module().remove_function(function);
    };

    m_context.main_module().push_scope(function);

    jit::block main_block = function.new_block("entry");

    m_context.main_module().push_scope(main_block);

    on_scope_exit
    {
        m_context.main_module().pop_scope();
        m_context.main_module().pop_scope();
    };

    main_block.set_as_insert_point();
    jit::value return_value = build(function_dec.expr);
    if (return_value.get_type().kind() != function.return_type().kind()) {
        return_value = return_value.cast_to(function.return_type());
    }

    main_block.set_return(return_value);

    std::string error_str;
    bool ok = function.finalize(&error_str);
    if (!ok) {
        throw compile_error_t(error_str);
    }
    return function;
}

jit::function compiler_t::create_top_level_expression_function(
    const ast::expression& expression)
{
    int i = 0;
    jit::function_creation_info info;
    static const char* name_prefix = "__expression_";
    info.name                      = name_prefix + std::to_string(i);
    info.return_type = builtin_type_names.find(type_kind::r64)->second;

    while (m_context.main_module().find_function(info.name, 0)) {
        ++i;
        info.name = name_prefix + std::to_string(i);
    }

    jit::function function = m_context.main_module().new_function(info);

    m_context.main_module().push_scope(function);

    jit::block main_block = function.new_block("entry");

    m_context.main_module().push_scope(main_block);

    main_block.set_as_insert_point();

    jit::value value =
        build(expression).cast_to(m_context.get_builtin_type(type_kind::r64));

    main_block.set_return(value);

    m_context.main_module().pop_scope();
    m_context.main_module().pop_scope();

    std::string error_str;
    bool ok = function.finalize(&error_str);
    if (!ok) {
        throw compile_error_t(error_str);
    }
    return function;
}
