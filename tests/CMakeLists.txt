cmake_minimum_required( VERSION 2.6 )


set(EXECUTABLE_OUTPUT_PATH ${TEST_DIR})
add_executable (core_tests core.cpp)
add_executable (proto_bin proto.cpp)


set(CATCH_DIR "/home/ghaith/data/development/installs/Catch/Catch/include/")

include_directories(${CATCH_DIR})

target_link_libraries (core_tests pthread)
target_link_libraries (proto_bin pthread)

