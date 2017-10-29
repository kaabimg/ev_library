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
struct number_t;
struct expression_t;
struct statement_t;
struct operand_t;
struct unary_t;
struct function_call_t;
struct function_declaration_t;
}

struct compiler_t : non_copyable
{
    compiler_t(jit::context& context) : m_context(context) {}
    jit::function compile(const ast::statement_t&);

protected:
    jit::value build(bool v);
    jit::value build(std::int32_t v);
    jit::value build(std::int64_t v);
    jit::value build(float v);
    jit::value build(double v);
    jit::value build(const ast::number_t& n);

    jit::value build(const ast::expression_t& expression);
    jit::value build(const ast::operand_t& operand);
    jit::value build(const ast::unary_t& expression);
    jit::value build(const ast::function_call_t& func_call);

    jit::function build(const ast::function_declaration_t& function_dec);

    jit::function create_top_level_expression_function(
        const ast::expression_t& expression);

private:
    jit::context& m_context;
};
}
}
