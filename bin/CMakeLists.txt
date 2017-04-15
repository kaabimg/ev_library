
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

find_package(OpenCL)
if (OpenCL_FOUND)
    add_executable (proto_bin proto.cpp)
    target_link_libraries(proto_bin ev_ocl)
endif()



######################################################################################

add_executable (mem_c mem_c.cpp)
target_link_libraries(mem_c memcached)

##################################################################################
#add_library(compute_cpp lm")

#set(COMPUTECPP_PACKAGE_ROOT_DIR "/home/ghaith/data/development/install/sycl/ComputeCpp-CE-0.1.1-Linux/")


#include(FindOpenCL)
#include(/home/ghaith/data/development/install/sycl/computecpp-sdk/cmake/Modules/FindComputeCpp.cmake)

#set(SOURCE_NAME "sycl")

#include_directories(${COMPUTECPP_INCLUDE_DIRECTORY})

#add_executable(${SOURCE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_NAME}.cpp)
#target_compile_options(${SOURCE_NAME} PUBLIC ${HOST_COMPILER_OPTIONS})

#add_sycl_to_target(${SOURCE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_NAME}.cpp
#                    ${CMAKE_CURRENT_BINARY_DIR})



#################################################################################

add_executable (atk atk.cpp)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5Core)
find_package(Qt5Widgets)
qt5_use_modules(atk Core Widgets)
target_link_libraries(atk ev_atk ${QT_LIBRARIES})


#################################################################################

#FIND_PACKAGE(CUDA REQUIRED)
#INCLUDE(FindCUDA)
#FILE(GLOB SOURCES  "*.cu" "cuda.cpp" "*.c" "*.h")
#cuda_add_executable(cuda ${SOURCES})

##LIST(APPEND CMAKE_CXX_FLAGS "-O3 -ffast-math -Wall")

##LIST(APPEND CUDA_NVCC_FLAGS --compiler-options -fno-strict-aliasing -lineinfo -use_fast_math -Xptxas -dlcm=cg)
##LIST(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_20,code=sm_20)
##LIST(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_30,code=sm_30)
##LIST(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_35,code=sm_35)

##TARGET_LINK_LIBRARIES(cuda /usr/local/cuda/lib64/libcudart.so)


add_subdirectory( net )


