#pragma once

#include <ev/core/basic_types.hpp>

#include <cstdint>

namespace ev
{
namespace vm
{
namespace jit
{
struct value;
struct context;
struct function;
}

namespace ast
{
struct number;
struct expression;
struct statement;
struct operand;
struct unary;
struct function_call;
struct function_declaration;
}

struct compiler_t : non_copyable
{
    compiler_t(jit::context& context) : m_context(context) {}
    jit::function compile(const ast::statement&);

protected:
    jit::value build(bool v);
    jit::value build(std::int32_t v);
    jit::value build(std::int64_t v);
    jit::value build(float v);
    jit::value build(double v);
    jit::value build(const ast::number& n);

    jit::value build(const ast::expression& expression);
    jit::value build(const ast::operand& operand);
    jit::value build(const ast::unary& expression);
    jit::value build(const ast::function_call& func_call);

    jit::function build(const ast::function_declaration& function_dec);

    jit::function create_top_level_expression_function(
        const ast::expression& expression);

private:
    jit::context& m_context;
};
}
}
