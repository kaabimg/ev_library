#ifndef EV_VM_JIT_PRIVATE_DATA_H
#define EV_VM_JIT_PRIVATE_DATA_H

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

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/RTDyldMemoryManager.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/LambdaResolver.h>
#include <llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/RuntimeDyld.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/ExecutionEngine/Orc/CompileOnDemandLayer.h>
#include <llvm/ExecutionEngine/Orc/JITSymbol.h>
#include <llvm/ExecutionEngine/Orc/IRTransformLayer.h>

#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>

#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Support/TargetSelect.h>

#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>


#include <ev/core/logging.h>


#include <unordered_map>

#include "data_fwd_declare.h"
#include "value.h"
#include "function.h"
#include "module.h"



namespace ev { namespace vm { namespace jit {

struct context_t;

struct context_private_t
{
    context_t* interface;
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder{context};
    std::unordered_map<std::string,module_t> modules;
};


struct module_private_t
{
    context_private_t* context = nullptr;
    llvm::IRBuilder<> builder;
    llvm::Module module;
    std::string name;

    std::vector<function_t> functions;

    std::vector<compilation_scope_t*> scope_stack;

    module_private_t(const std::string & name,context_private_t* c)
        :context(c),builder(c->context),module(name,c->context){}

};


struct function_private_t
{
    module_private_t* module = nullptr;
    function_data_t data =  nullptr;
    function_creation_info_t creation_info;
    std::vector<named_value_t> arguments;
};

struct block_private_t
{
    function_private_t* function = nullptr;
    block_data_t data = nullptr;
    std::vector<named_value_t> variables;
};


struct value_private_t {
    value_private_t(){}
    value_private_t(context_private_t* c,value_data_t d):context(c),data(d){}
    context_private_t* context = nullptr;
    value_data_t data = nullptr;

};


}}} // namespace ev::vm::jit






#endif//EV_VM_JIT_PRIVATE_DATA_H
