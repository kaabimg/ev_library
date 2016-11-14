

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


)



set(LLVM_DIR "/home/ghaith/data/development/installs/llvm/install")
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${LLVM_DIR}/lib")
include_directories("${LLVM_DIR}/include")
link_directories("${LLVM_DIR}/lib")


find_library( LIB_LLVMX86Disassembler    LLVMX86Disassembler )
find_library( LIB_LLVMX86AsmParser       LLVMX86AsmParser )
find_library( LIB_LLVMX86CodeGen         LLVMX86CodeGen )
find_library( LIB_LLVMSelectionDAG       LLVMSelectionDAG )
find_library( LIB_LLVMAsmPrinter         LLVMAsmPrinter )
find_library( LIB_LLVMDebugInfoCodeView  LLVMDebugInfoCodeView )
find_library( LIB_LLVMCodeGen            LLVMCodeGen )
find_library( LIB_LLVMScalarOpts         LLVMScalarOpts )
find_library( LIB_LLVMInstCombine        LLVMInstCombine )
find_library( LIB_LLVMInstrumentation    LLVMInstrumentation )
find_library( LIB_LLVMTransformUtils     LLVMTransformUtils )
find_library( LIB_LLVMBitWriter          LLVMBitWriter )
find_library( LIB_LLVMX86Desc            LLVMX86Desc )
find_library( LIB_LLVMMCDisassembler     LLVMMCDisassembler )
find_library( LIB_LLVMX86Info            LLVMX86Info )
find_library( LIB_LLVMX86AsmPrinter      LLVMX86AsmPrinter )
find_library( LIB_LLVMX86Utils           LLVMX86Utils )
find_library( LIB_LLVMMCJIT              LLVMMCJIT )
find_library( LIB_LLVMExecutionEngine    LLVMExecutionEngine )
find_library( LIB_LLVMTarget             LLVMTarget )
find_library( LIB_LLVMAnalysis           LLVMAnalysis )
find_library( LIB_LLVMProfileData        LLVMProfileData )
find_library( LIB_LLVMRuntimeDyld        LLVMRuntimeDyld )
find_library( LIB_LLVMObject             LLVMObject )
find_library( LIB_LLVMMCParser           LLVMMCParser )
find_library( LIB_LLVMBitReader          LLVMBitReader )
find_library( LIB_LLVMMC                 LLVMMC )
find_library( LIB_LLVMCore               LLVMCore )
find_library( LIB_LLVMSupport            LLVMSupport  )


set (LLVM_LIBS
    ${LIB_LLVMX86Disassembler}
    ${LIB_LLVMX86AsmParser}
    ${LIB_LLVMX86CodeGen}
    ${LIB_LLVMCore}
    ${LIB_LLVMSupport}
    ${LIB_LLVMMC}
    ${LIB_LLVMBitReader}
    ${LIB_LLVMMCParser}
    ${LIB_LLVMObject}
    ${LIB_LLVMRuntimeDyld}
    ${LIB_LLVMProfileData}
    ${LIB_LLVMAnalysis}
    ${LIB_LLVMTarget}
    ${LIB_LLVMExecutionEngine}
    ${LIB_LLVMMCJIT}
    ${LIB_LLVMX86Utils}
    ${LIB_LLVMX86AsmPrinter}
    ${LIB_LLVMX86Info}
    ${LIB_LLVMMCDisassembler}
    ${LIB_LLVMX86Desc}
    ${LIB_LLVMBitWriter}
    ${LIB_LLVMTransformUtils}
    ${LIB_LLVMInstrumentation}
    ${LIB_LLVMInstCombine}
    ${LIB_LLVMScalarOpts}
    ${LIB_LLVMCodeGen}
    ${LIB_LLVMDebugInfoCodeView}
    ${LIB_LLVMAsmPrinter}
    ${LIB_LLVMSelectionDAG}
    ${LIB_LLVMTarget}
    )

add_library(ev_vm SHARED ${SOURCES} ${HEADERS})


target_link_libraries (ev_vm ${LLVM_LIBS} rt dl pthread z m)



