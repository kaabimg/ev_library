#pragma once

#include <ev/core/preprocessor.hpp>

#include <cstdint>

namespace ev {
namespace vm {

namespace jit {
struct value_t;
struct context_t;
struct function_t;
}

namespace ast {
struct number_t;
struct expression_t;
struct statement_t;
struct operand_t;
struct unary_t;
struct function_call_t;
struct function_declaration_t;
}

struct compiler_t {
    compiler_t(jit::context_t& context) : m_context(context) {}

    jit::function_t compile(const ast::statement_t&);

protected:
    jit::value_t build(bool v);
    jit::value_t build(std::int32_t v);
    jit::value_t build(std::int64_t v);
    jit::value_t build(float v);
    jit::value_t build(double v);
    jit::value_t build(const ast::number_t& n);

    jit::value_t build(const ast::expression_t& expression);
    jit::value_t build(const ast::operand_t& operand);
    jit::value_t build(const ast::unary_t& expression);
    jit::value_t build(const ast::function_call_t& func_call);

    jit::function_t build(const ast::function_declaration_t& function_dec);

    jit::function_t create_top_level_expression_function(
        const ast::expression_t& expression);

private:
    jit::context_t& m_context;

    EV_DISABLE_COPY(compiler_t)
};
}
}
