

include_directories(${BOOST_INCLUDE_DIR})

set(EXECUTABLE_OUTPUT_PATH ${BIN_DIR})



set (SOURCES parser/parser.cpp
             compiler/compiler.cpp
             jit/module.cpp
             jit/function.cpp
             jit/utils.cpp
             jit/context.cpp
             jit/value.cpp
             virtual_machine.cpp
             runtime_function.cpp
)


set (HEADERS  parser/ast.h
              parser/parser.h
              compiler/compiler.h
              jit/module.h
              jit/function.h
              jit/utils.h
              jit/context.h
              jit/data_fwd_declare.h
              jit/value.h
              jit/private_data.h
              jit/object.h
              jit/execution_engine.h
              vm_exceptions.h
              vm_object.h
              virtual_machine.h
              runtime_function.h
)


include_directories("/usr/include/llvm-3.9/")
include_directories("/usr/lib/llvm-3.9/include/")

add_library(ev_vm SHARED ${SOURCES} ${HEADERS})
target_link_libraries (ev_vm LLVM-3.9 rt dl pthread z m)



