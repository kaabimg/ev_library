
set(EXECUTABLE_OUTPUT_PATH ${BIN_DIR})

include_directories(${BOOST_INCLUDE_DIR})


add_executable (virtual_machine "vm.cpp")
target_link_libraries (virtual_machine  pthread)

target_link_libraries(virtual_machine ev_vm)

##################################################################################

include_directories("/usr/lib/llvm-3.8/include/")


add_executable (proto_bin proto.cpp)
target_link_libraries (proto_bin  LLVM-3.8 rt dl pthread z m)
