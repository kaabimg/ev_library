
set(EXECUTABLE_OUTPUT_PATH ${BIN_DIR})

include_directories(${BOOST_INCLUDE_DIR})


add_executable (virtual_machine "vm.cpp")
target_link_libraries(virtual_machine ev_vm LLVM-3.9 rt dl pthread z m pthread)

##################################################################################

add_executable (proto_bin proto.cpp)
