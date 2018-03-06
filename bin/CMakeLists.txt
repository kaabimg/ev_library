
set(EXECUTABLE_OUTPUT_PATH ${BIN_DIR})
#####################################################

find_package(Boost REQUIRED system thread)
include_directories(${BOOST_INCLUDE_DIR})

#####################################################


add_executable (virtual_machine "vm.cpp")
target_link_libraries(virtual_machine ev_vm LLVM-3.9 rt dl pthread z m)

#####################################################

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core)
find_package(Qt5Widgets)

add_executable (test_bin test.cpp)
target_link_libraries(test_bin Qt5::Core Qt5::Widgets Boost::system Boost::thread evt)

#####################################################
include_directories("/home/mkaabi/install/pybind11/pybind11/include")
include_directories("/usr/include/python2.7/")
add_executable (pyb11 "pyb11.cpp")
target_link_libraries(pyb11 python2.7)
#######################################################
add_subdirectory( p4d )
