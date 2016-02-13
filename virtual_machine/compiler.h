#ifndef EV_COMPILER_H
#define EV_COMPILER_H

#include "jit_types.h"

namespace ev { namespace vm {

struct context_t;

namespace ast { struct statement_t; }


struct compiler_t {
    function_t compile(const ast::statement_t & ,context_t&);

};

}}

#endif // EV_COMPILER_H
