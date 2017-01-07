#pragma once

#include "execution_engine.hpp"
#include "data_fwd_declare.hpp"
#include "value.hpp"
#include "function.hpp"
#include "module.hpp"
#include "type.hpp"

#include <ev/core/logging.hpp>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/Mangler.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>

#include <unordered_map>

namespace ev {
namespace vm {
namespace jit {

struct context_t;

struct context_private_t {
    context_t*        interface;
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder{context};
    std::unordered_map<std::string, module_t> modules;
    execution_engine_t                  execution_engine;
    execution_engine_t::module_handle_t added_modules_handle;
    bool                                has_compiled_modules = false;
};

struct module_private_t {
    context_private_t*                context = nullptr;
    llvm::Module                      module;
    std::vector<function_t>           functions;
    std::vector<compilation_scope_t*> scope_stack;
    std::unordered_map<std::string, struct_t>        structs;
    std::unordered_map<struct_data_t, struct_info_t> structs_data;

    module_private_t(const std::string& name, context_private_t* c)
        : context(c), module(name, c->context) {}
};

struct function_private_t {
    module_private_t*        module = nullptr;
    function_data_t          data   = nullptr;
    function_creation_info_t creation_info;
    uintptr_t                function_ptr = 0;
};

struct block_private_t {
    function_private_t*        function = nullptr;
    block_data_t               data     = nullptr;
    std::vector<named_value_t> variables;
};

struct value_private_t {
    value_private_t() {}
    value_private_t(context_private_t* c, value_data_t d)
        : context(c), data(d) {}
    context_private_t* context = nullptr;
    value_data_t       data    = nullptr;
};

struct struct_private_t {
    struct_data_t     data   = nullptr;
    module_private_t* module = nullptr;
};
}
}
}  // namespace ev::vm::jit
