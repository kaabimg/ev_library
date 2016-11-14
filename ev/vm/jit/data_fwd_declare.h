#ifndef EV_VM_JIT_DATA_H
#define EV_VM_JIT_DATA_H


namespace llvm {
class Value;
class Function;
class BasicBlock;
class Type;

}

#include<memory>


namespace ev { namespace vm { namespace jit {

using value_data_t = llvm::Value*;
using function_data_t = llvm::Function*;
using type_data_t = llvm::Type*;
using block_data_t = llvm::BasicBlock*;


}}}

#endif//EV_VM_JIT_DATA_H
