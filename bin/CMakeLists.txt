
set(EXECUTABLE_OUTPUT_PATH ${BIN_DIR})

include_directories(${BOOST_INCLUDE_DIR})


add_executable (virtual_machine "vm.cpp")
target_link_libraries(virtual_machine ev_vm LLVM-3.9 rt dl pthread z m pthread)

##################################################################################
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
add_executable (proto_bin proto.cpp)
target_link_libraries(proto_bin ev_ocl)


######################################################################################

add_executable (mem_c mem_c.cpp)
target_link_libraries(mem_c memcached)

##################################################################################

add_executable (ogl ogl.cpp)
target_link_libraries(ogl EGL GLESv2)
