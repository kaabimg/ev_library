cmake_minimum_required( VERSION 2.6 )


include_directories(${BOOST_INCLUDE_DIR})

set(EXECUTABLE_OUTPUT_PATH ${BIN_DIR})


set (SOURCES main.cpp 
	     parser.cpp 
	     compiler.cpp 
	     jit_types.cpp 
    	     virtual_machine.cpp
)

set (HEADERS  ast.h 
	      parser.h	 	
	      compiler.h 
	      jit_types.h 
	      virtual_machine.h
              vm_object.h
)


add_executable (virtual_machine ${SOURCES} ${HEADERS})




target_link_libraries (virtual_machine pthread "gccjit")

