
include_directories(${BOOST_INCLUDE_DIR})

set (SOURCES parser/parser.cpp
             compiler/compiler.cpp
             jit/module.cpp
             jit/function.cpp
             jit/function_signature.cpp
             jit/context.cpp
             jit/value.cpp
             jit/type.cpp
             virtual_machine.cpp
             runtime_function.cpp
)


set (HEADERS  parser/ast.hpp
             parser/ast_adapted.hpp
              parser/parser.hpp
              compiler/compiler.hpp
              jit/module.hpp
              jit/function.hpp
              jit/function_signature.hpp
              jit/context.hpp
              jit/data_fwd_declare.hpp
              jit/value.hpp
              jit/private_data.hpp
              jit/object.hpp
              jit/execution_engine.hpp
              jit/type.hpp
              vm_exceptions.hpp
              vm_types.hpp
              vm_object.hpp
              virtual_machine.hpp
              runtime_function.hpp
)


include_directories("/usr/include/llvm-3.9/")
include_directories("/usr/lib/llvm-3.9/include/")

add_library(ev_vm SHARED ${SOURCES} ${HEADERS})
target_link_libraries (ev_vm LLVM-3.9 rt dl pthread z m)



