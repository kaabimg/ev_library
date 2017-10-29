#ifndef EV_VM_JIT_DATA_H
#define EV_VM_JIT_DATA_H

namespace llvm {
class Value;
class Function;
class BasicBlock;
class Type;
class StructType;
}

#include <memory>

namespace ev {
namespace vm {
namespace jit {
using value_data = llvm::Value*;
using function_data = llvm::Function*;
using block_data = llvm::BasicBlock*;
using type_data = llvm::Type*;
using structure_data = llvm::StructType*;
}
}
}

#endif  // EV_VM_JIT_DATA_H
