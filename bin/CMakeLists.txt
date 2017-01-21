
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
target_link_libraries(ogl ev_ogl GLESv2)

##################################################################################

add_executable (vr vr.cpp)
target_link_libraries(vr ev_vlr ev_ogl GLESv2)

##################################################################################
#add_library(compute_cpp lm")

set(COMPUTECPP_PACKAGE_ROOT_DIR "/home/ghaith/data/development/install/sycl/ComputeCpp-CE-0.1.1-Linux/")


include(FindOpenCL)
include(/home/ghaith/data/development/install/sycl/computecpp-sdk/cmake/Modules/FindComputeCpp.cmake)

set(SOURCE_NAME "sycl")

include_directories(${COMPUTECPP_INCLUDE_DIRECTORY})

add_executable(${SOURCE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_NAME}.cpp)
target_compile_options(${SOURCE_NAME} PUBLIC ${HOST_COMPILER_OPTIONS})

add_sycl_to_target(${SOURCE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_NAME}.cpp
                    ${CMAKE_CURRENT_BINARY_DIR})
