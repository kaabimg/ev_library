#pragma once

namespace ev { namespace vm {

namespace jit {
struct context_t;
struct function_t;
}

namespace ast { struct statement_t; }


struct compiler_t {
    jit::function_t compile(const ast::statement_t & , jit::context_t &context);

};

}}

