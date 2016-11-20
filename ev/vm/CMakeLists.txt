

include_directories(${BOOST_INCLUDE_DIR})

set(EXECUTABLE_OUTPUT_PATH ${BIN_DIR})



set (SOURCES parser.cpp
	     compiler.cpp 
    	     virtual_machine.cpp
             jit/module.cpp
             jit/function.cpp
             jit/utils.cpp
             jit/context.cpp
             jit/value.cpp

)


set (HEADERS  ast.h 
	      parser.h	 	
	      compiler.h 
	      virtual_machine.h
              vm_object.h
              compile_exceptions.h
              jit/module.h
              jit/function.h
              jit/utils.h
              jit/context.h
              jit/data_fwd_declare.h
              jit/value.h
              jit/private_data.h
              jit/object.h
              jit/execution_engine.h


)

include_directories("/usr/lib/llvm-3.8/include/")

add_library(ev_vm SHARED ${SOURCES} ${HEADERS})
target_link_libraries (ev_vm LLVM-3.8 rt dl pthread z m)



