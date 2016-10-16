cmake_minimum_required( VERSION 2.6 )

include(../config.cmake)


set(EXECUTABLE_OUTPUT_PATH ${BIN_DIR})


set (SOURCES main.cpp 
	     parser.cpp 
	     compiler.cpp 
	     jit_types.cpp 
	     jit_debug.cpp 	
    	     virtual_machine.cpp
)

set (HEADERS  ast.h 
	      parser.h	 	
	      compiler.h 
	      jit_types.h 
	      jit_debug.h 
	      virtual_machine.h
)


#add_executable (virtual_machine ${SOURCES} ${HEADERS})



set (LIBJIT_INSTALL_DIR  /home/ghaith/data/development/installs/libjit/install )

#INCLUDEPATH += $$LIBJIT_INSTALL_DIR/include
#LIBS += -L$$LIBJIT_INSTALL_DIR/lib/ -ljitplus -ljit -ljitdynamic
