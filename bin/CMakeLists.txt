
set(EXECUTABLE_OUTPUT_PATH ${BIN_DIR})
find_package(Boost REQUIRED system thread)

find_package(Boost REQUIRED system thread)
include_directories(${BOOST_INCLUDE_DIR})

add_executable (virtual_machine "vm.cpp")
target_link_libraries(virtual_machine ev_vm LLVM-3.9 rt dl pthread z m)

add_executable (test_bin test.cpp)
target_link_libraries(test_bin Boost::system Boost::thread)

add_subdirectory( p4d )
