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
struct context;

struct context_private {
    context* interface;
    llvm::LLVMContext llvm_context;
    llvm::IRBuilder<> builder{llvm_context};
    std::unordered_map<std::string, module> modules;
    execution_engine exec_engine;
    execution_engine::module_handle added_modules_handle;
    bool has_compiled_modules = false;
};

struct module_private {
    context_private* context = nullptr;
    llvm::Module module;
    std::vector<function> functions;
    std::vector<compilation_scope*> scope_stack;
    std::unordered_map<std::string, structure> structs;
    std::unordered_map<structure_data, structure_info> structs_data;

    module_private(const std::string& name, context_private* c)
        : context(c), module(name, c->llvm_context)
    {
    }
};

struct function_private {
    module_private* module = nullptr;
    function_data data = nullptr;
    function_creation_info creation_info;
    uintptr_t function_ptr = 0;
};

struct block_private {
    function_private* function = nullptr;
    block_data data = nullptr;
    std::vector<named_value> variables;
};

struct value_private {
    value_private()
    {
    }
    value_private(context_private* c, value_data d) : context(c), data(d)
    {
    }
    context_private* context = nullptr;
    value_data data = nullptr;
};

struct structure_private {
    structure_data data = nullptr;
    module_private* module = nullptr;
};
}
}
}  // namespace ev::vm::jit
